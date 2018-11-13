#include "RendererApp.h"



RendererApp::RendererApp()
	: m_v8Handler(new CefV8HandlerImpl)
{
}


RendererApp::~RendererApp()
{
}

void RendererApp::OnWebKitInitialized()
{
	std::string app_code =
		//-----------------------------------
		//声明JavaScript里要调用的Cpp方法
		"var app;"
		"if (!app)"
		"  app = {};"
		"(function() {"

		//  Send message 
		"  app.sendMessage = function(name, arguments) {"
		"    native function sendMessage();"
		"    return sendMessage(name, arguments);"
		"  };"

		// Registered Javascript Function, which will be called by Cpp
		"  app.registerJavascriptFunction = function(name,callback) {"
		"    native function registerJavascriptFunction();"
		"    return registerJavascriptFunction(name,callback);"
		"  };"

		"})();";
	//------------------------------------

	// Register app extension module

	// JavaScript里调用app.registerJavascriptFunction时，就会去通过CefRegisterExtension注册的CefV8Handler列表里查找
	// 找到"v8/app"对应的CefV8HandlerImpl，就调用它的Execute方法
	m_v8Handler = new CefV8HandlerImpl();
	CefRegisterExtension("v8/app", app_code, m_v8Handler);
}

void RendererApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	if (m_v8Handler) 
	{
		m_v8Handler->OnContextReleased(browser, frame, context);
	}
}

bool RendererApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	const std::string& messageName = message->GetName();
	if (messageName == "call_js"
		&& m_v8Handler)
	{
		m_v8Handler->CallJsFunction(browser, message->GetArgumentList());
		return true;
	}
	return false;
}

