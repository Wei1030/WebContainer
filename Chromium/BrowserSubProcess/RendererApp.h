#pragma once

#include "include/cef_app.h"
#include "CefV8HandlerImpl.h"

class RendererApp : public CefApp, public CefRenderProcessHandler
{
public:
	RendererApp();
	~RendererApp();

private:
	//impl CefApp
//	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE;


//	virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) OVERRIDE;


//	virtual CefRefPtr<CefResourceBundleHandler> GetResourceBundleHandler() OVERRIDE;


//	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;


	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE
	{
		return this;
	}





	//impl CefRenderProcessHandler
//	virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;


	virtual void OnWebKitInitialized() OVERRIDE;


//	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;


//	virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;


//	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;


//	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;


	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;


//	virtual void OnUncaughtException(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception, CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;


//	virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) OVERRIDE;


	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;
private:
	CefRefPtr<CefV8HandlerImpl> m_v8Handler;
private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(RendererApp);
	DISALLOW_COPY_AND_ASSIGN(RendererApp);

};

