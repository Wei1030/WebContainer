// MainBrowser.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <atomic>
#include <chrono>    // std::chrono::seconds
#include <thread> 

#include "sRPC/sRPC.h"
#include "BrowserApp.h"
#include "ClientHandler.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

int MODULE_API Load(const void* data_in, unsigned int size_in, COMPLETIONPROC proc, void* usr)
{
	int iRetVal = -1;

	do 
	{
		if (nullptr == data_in || size_in < sizeof(WebContainer_Load_Req))
		{
			break;
		}

		WebContainer_Load_Req* pLoadInfo = new(std::nothrow) WebContainer_Load_Req;
		if (nullptr == pLoadInfo)
		{
			break;
		}

		memcpy(pLoadInfo, data_in, sizeof(WebContainer_Load_Req));

		if (-1 == ClientHandler::GetInstance()->ExecuteLoadOnUIThread(pLoadInfo))
		{
			delete pLoadInfo;
		}

		iRetVal = 0;

	} while (0);

	if (proc)
	{
		WebContainer_Load_Rsp rsp = { iRetVal };
		proc(rsp.iError, &rsp, sizeof(rsp), usr);
	}
	return iRetVal;
}

int MODULE_API CallJs(const void* data_in, unsigned int size_in, COMPLETIONPROC proc, void* usr)
{
	int iRetVal = -1;

	do
	{
		if (nullptr == data_in || size_in < sizeof(WebContainer_CallJs_Req))
		{
			break;
		}

		char* pData = new(std::nothrow) char[size_in+1];		
		if (nullptr == pData)
		{
			break;
		}
		memcpy(pData, data_in, size_in);
		pData[size_in] = 0;		

		if (-1 == ClientHandler::GetInstance()->ExecuteCallJsOnUIThread((WebContainer_CallJs_Req*)pData))
		{
			delete pData;
		}

		iRetVal = 0;

	} while (0);

	if (proc)
	{
		WebContainer_CallJs_Rsp rsp = { iRetVal };
		proc(rsp.iError, &rsp, sizeof(rsp), usr);
	}
	return iRetVal;
}

static std::atomic<int>  g_ref(0);
/*
1、有客户端连接;type = 0
2、远程客户端主动关闭或出现异常导致的连接断开;type = 1
*/
int SRPC_API AcptEventProc(void* instance, int type, void* usr)
{
	if (0 == type)
	{
		g_ref++;
	}
	else if(1 == type)
	{
		if (0 == --g_ref)
		{
			CefPostTask(TID_UI,	base::Bind(&CefQuitMessageLoop));
		}		
	}	

	return 0;
}

void* SRPC_API RequesterChecker(void* usr)
{
	for (int i =0; 0 == g_ref && i< 5;++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	if (0 == g_ref)
	{
		CefPostTask(TID_UI, base::Bind(&CefQuitMessageLoop));
	}

	return 0;
}

int InitSRPC(const std::string& dir)
{
	SRPC_INIT_BEGIN()
		EXPORT_PROC("WebContainer.Load", &Load)		//载入url
		EXPORT_PROC("WebContainer.CallJs", &CallJs)	//调用js方法
	SRPC_INIT_END()

	if (SRPC_INIT_FAIL)
	{
		return -1;
	}
	
	SRPC_SetAcptEventCb(NULL,&AcptEventProc,NULL);

	std::string strCfgPath = dir + "\\MainBrowser.xml";
	if (-1 == SRPC_InitFrame(strCfgPath.c_str(), 1))
	{
		return -1;
	}
	
	//5秒内发起者未连接则退出
	return SRPC_ThreadPoolWork(NULL,&RequesterChecker,NULL);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	
	// Structure for passing command-line arguments.
	// The definition of this structure is platform-specific.
	// 传递命令行参数的结构体。
	// 这个结构体的定义与平台相关。
	CefMainArgs main_args(hInstance);

	// Parse command-line arguments.
	// 解析命令行
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());

	//从命令行取得主程序路径（第一个参数）
	std::string strProgramDir	= command_line->GetProgram();
	std::string::size_type pos	= strProgramDir.rfind('\\');
	if (std::string::npos == pos )
	{
		return -1;
	}
	strProgramDir.resize(pos);

	//子程序路径
	std::string strSubProcessPath = strProgramDir + "\\BrowserSubProcess.exe";

	// Populate this structure to customize CEF behavior.
	// 填充这个结构体，用于定制CEF的行为。
	CefSettings settings;

	// Specify the path for the sub-process executable.
	// 指定子进程的执行路径
	CefString(&settings.browser_subprocess_path).FromString(strSubProcessPath);

	// Optional implementation of the CefApp interface.
	// 可选择性地实现CefApp接口
	CefRefPtr<BrowserApp> app(new BrowserApp);
	// Initialize CEF in the main process.
	// 在主进程中初始化CEF 
	CefInitialize(main_args, settings, app.get(),NULL);

	if (-1 == InitSRPC(strProgramDir))
	{		
		CefShutdown();
		return -1;
	}

	// Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
	// 执行消息循环，此时会堵塞，直到CefQuitMessageLoop()函数被调用。
	CefRunMessageLoop();

	// Shut down CEF.
	// 关闭CEF
	CefShutdown();

	SRPC_Uninit();
    return 0;
}