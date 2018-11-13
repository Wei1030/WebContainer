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
		//����JavaScript��Ҫ���õ�Cpp����
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

	// JavaScript�����app.registerJavascriptFunctionʱ���ͻ�ȥͨ��CefRegisterExtensionע���CefV8Handler�б������
	// �ҵ�"v8/app"��Ӧ��CefV8HandlerImpl���͵�������Execute����
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

