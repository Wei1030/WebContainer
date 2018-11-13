#pragma once
#include <map>
#include "include/cef_client.h"

#include "InnerProtocol.h"

class ClientHandler : public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefDownloadHandler,
	public CefContextMenuHandler
{
public:
	ClientHandler();
	~ClientHandler();
	static ClientHandler* GetInstance();

	int ExecuteLoadOnUIThread(WebContainer_Load_Req* pLoadInfo);
	int ExecuteCallJsOnUIThread(WebContainer_CallJs_Req* pCallInfo);
protected:
	
	//impl CefClient
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE { return this; }


//	virtual CefRefPtr<CefDialogHandler> GetDialogHandler() OVERRIDE;


	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE { return this; }


	virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE { return this; }


//	virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE;


//	virtual CefRefPtr<CefFindHandler> GetFindHandler() OVERRIDE;


//	virtual CefRefPtr<CefFocusHandler> GetFocusHandler() OVERRIDE;


//	virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE;


//	virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE;


	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }


	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }


//	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;


//	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE;


	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;




	//impl CefDisplayHandler
// 	virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;
// 
// 
// 	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
// 
// 
// 	virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) OVERRIDE;
// 
// 
// 	virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) OVERRIDE;
// 
// 
// 	virtual bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) OVERRIDE;
// 
// 
// 	virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) OVERRIDE;
// 
// 
// 	virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level, const CefString& message, const CefString& source, int line) OVERRIDE;
// 
// 
// 	virtual bool OnAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& new_size) OVERRIDE;
// 
// 
// 	virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress) OVERRIDE;






	//impl CefLifeSpanHandler
// 	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, 
// 		const CefString& target_url, const CefString& target_frame_name, WindowOpenDisposition target_disposition,
// 		bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client,
// 		CefBrowserSettings& settings, bool* no_javascript_access) OVERRIDE;


//	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;


//	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;


	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	





	//impl CefLoadHandler
//	virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) OVERRIDE;


	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) OVERRIDE;


	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;


	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, 
		const CefString& errorText, const CefString& failedUrl) OVERRIDE;






	//impl CefDownloadHandler
	virtual void OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, 
		const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;


// 	virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, 
// 		CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;





	//impl CefContextMenuHandler
// 	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
// 		CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE;
// 
// 
// 	virtual bool RunContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, 
// 		CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback) OVERRIDE;
// 
// 
// 	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, 
// 		int command_id, EventFlags event_flags) OVERRIDE;
// 
// 
// 	virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;

private:
	void load(WebContainer_Load_Req* pLoadInfo);
	void notifyMsg(CefRefPtr<CefBrowser> browser, const std::string& strMethod,const std::string& strArgs);
	void callJs(WebContainer_CallJs_Req* pCallInfo);

private:
	typedef std::map<void*, std::pair<void*,CefRefPtr<CefBrowser>>> BrowserMap;
	BrowserMap	browser_map_;
private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(ClientHandler);
	DISALLOW_COPY_AND_ASSIGN(ClientHandler);
};

