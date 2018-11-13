#include "ClientHandler.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "sRPC/sRPC.h"
namespace {

	ClientHandler* g_instance = NULL;

}  // namespace

ClientHandler* ClientHandler::GetInstance()
{
	return g_instance;
}

int ClientHandler::ExecuteLoadOnUIThread(WebContainer_Load_Req* pLoadInfo)
{
	if (!CefCurrentlyOn(TID_UI)) 
	{
		// Execute on the UI thread.		
		return CefPostTask(TID_UI,
			base::Bind(&ClientHandler::load, this, pLoadInfo)) ? 0 : -1;
	}

	load(pLoadInfo);
	return 0;
}

int ClientHandler::ExecuteCallJsOnUIThread(WebContainer_CallJs_Req* pCallInfo)
{
	if (!CefCurrentlyOn(TID_UI))
	{
		// Execute on the UI thread.		
		return CefPostTask(TID_UI,
			base::Bind(&ClientHandler::callJs, this, pCallInfo)) ? 0 : -1;
	}

	callJs(pCallInfo);
	return 0;
}

ClientHandler::ClientHandler() 
{
	g_instance = this;
}


ClientHandler::~ClientHandler()
{
}

bool ClientHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	const CefString& messageName = message->GetName();
	if (messageName == "MSG_SEARCH_FINISH")
	{
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		if (args->GetSize() >= 2
			&& args->GetType(0) == VTYPE_STRING
			&& args->GetType(1) == VTYPE_STRING)
		{
			notifyMsg(browser, args->GetString(0), args->GetString(1));
		}
		return true;
	}
	return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	BrowserMap::iterator bit = browser_map_.begin();
	for (; bit != browser_map_.end(); ++bit)
	{
		if (bit->second.second->IsSame(browser))
		{
			SRPC_DestroyClient(bit->second.first);
			browser_map_.erase(bit);
			break;
		}
	}
}

void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type)
{

}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{

}

void ClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser, 
	CefRefPtr<CefFrame> frame, 
	ErrorCode errorCode,
	const CefString& errorText, 
	const CefString& failedUrl)
{
	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

void ClientHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, 
	const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback)
{

}

void ClientHandler::load(WebContainer_Load_Req* pLoadInfo)
{
	CEF_REQUIRE_UI_THREAD();

	do
	{		
		BrowserMap::const_iterator it = browser_map_.find(pLoadInfo->hWnd);
		if (it != browser_map_.end())
		{
			it->second.second->GetMainFrame()->LoadURL(pLoadInfo->szUrl);
			break;
		}

		void* inst = SRPC_CreateClient(FD_LOCAL, pLoadInfo->szRspAddr, 0);
		if (nullptr == inst)
		{
			break;
		}

		// Specify CEF browser settings here.
		CefBrowserSettings browser_settings;

		// Information used when creating the native window.
		CefWindowInfo window_info;

		RECT rcBrowser = { 0 };
		GetClientRect((HWND)pLoadInfo->hWnd, &rcBrowser);

		// On Windows we need to specify certain flags that will be passed to
		// CreateWindowEx().
		window_info.SetAsChild((HWND)pLoadInfo->hWnd, rcBrowser);

		// Create the first browser window.
		CefRefPtr<CefBrowser> b = CefBrowserHost::CreateBrowserSync(window_info, this, pLoadInfo->szUrl, browser_settings, NULL);
		if (b)
		{
			browser_map_.insert(std::make_pair(pLoadInfo->hWnd,
												std::make_pair(inst,b)));
		}
	} while (0);	

	delete pLoadInfo;
}

void ClientHandler::notifyMsg(CefRefPtr<CefBrowser> browser, const std::string& strMethod, const std::string& strArgs)
{
	CEF_REQUIRE_UI_THREAD();
	BrowserMap::iterator bit = browser_map_.begin();
	for (; bit != browser_map_.end(); ++bit)
	{
		if (bit->second.second->IsSame(browser))
		{
			SRPC_AsyncCall(bit->second.first, strMethod.c_str(), strArgs.data(), strArgs.length(), 0, 0);
			break;
		}
	}
}

void ClientHandler::callJs(WebContainer_CallJs_Req* pCallInfo)
{
	CEF_REQUIRE_UI_THREAD();

	do
	{
		if (browser_map_.empty())
		{
			break;
		}

		BrowserMap::const_iterator it = browser_map_.find(pCallInfo->hWnd);
		if (it == browser_map_.end())
		{			
			break;
		}

		//将消息发送给Renderer进程
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("call_js");
		// Retrieve the argument list object.
		CefRefPtr<CefListValue> args = msg->GetArgumentList();
		// Populate the argument values.
		args->SetSize(2);
		args->SetString(0, pCallInfo->szMethod);
		args->SetString(1, pCallInfo->szJsonArgs);

		it->second.second->SendProcessMessage(PID_RENDERER, msg);
		
	} while (0);

	delete (char*)pCallInfo;
}
