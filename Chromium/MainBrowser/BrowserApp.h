#pragma once

#include "include/cef_app.h"

class BrowserApp : public CefApp, public CefBrowserProcessHandler
{
public:
	BrowserApp();
	~BrowserApp();

private:
	//impl CefApp
//	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE;

//	virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) OVERRIDE;

//	virtual CefRefPtr<CefResourceBundleHandler> GetResourceBundleHandler() OVERRIDE;

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE
	{
		return this;
	}

//	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE;






	//impl CefBrowserProcessHandler
//	virtual void OnContextInitialized() OVERRIDE;


//	virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE;


//	virtual void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;


//	virtual CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE;


//	virtual void OnScheduleMessagePumpWork(int64 delay_ms) OVERRIDE;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(BrowserApp);
	DISALLOW_COPY_AND_ASSIGN(BrowserApp);

};

