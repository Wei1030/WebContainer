#include "CefV8HandlerImpl.h"
#include <functional>

CefV8HandlerImpl::CefV8HandlerImpl()
{
}


CefV8HandlerImpl::~CefV8HandlerImpl()
{
	
}

bool CefV8HandlerImpl::Execute(const CefString& name,	//JavaScript调用的C++方法名字
	CefRefPtr<CefV8Value> object,						//JavaScript调用者对象
	const CefV8ValueList& arguments,					//JavaScript传递的参数
	CefRefPtr<CefV8Value>& retval,						//需要返回给JavaScript的值设置给这个对象
	CefString& exception)								//通知异常信息给JavaScript
{
	// In the CefV8Handler::Execute implementation for “registerJavascriptFunction”.
	bool handle = false;
	if (name == "registerJavascriptFunction")
	{
		//保存JavaScript设置的回调函数
		if (arguments.size() == 2
			&& arguments[0]->IsString()
			&& arguments[1]->IsFunction())
		{
			std::string message_name = arguments[0]->GetStringValue();
			CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
			int browser_id = context->GetBrowser()->GetIdentifier();
			callback_map_.insert(
				std::make_pair(std::make_pair(message_name, browser_id),
					std::make_pair(context, arguments[1])));
			handle = true;			
		}
	}

	if (name == "sendMessage")
	{
		//处理SendMessage，
		if (arguments.size() == 2
			&& arguments[0]->IsString()
			&& arguments[1]->IsString())
		{			
			//将消息发送给Browser进程
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("sendMessage");
			// Retrieve the argument list object.
			CefRefPtr<CefListValue> args = msg->GetArgumentList();
			// Populate the argument values.
			args->SetSize(2);
			args->SetString(0, arguments[0]->GetStringValue());
			args->SetString(1, arguments[1]->GetStringValue());

			// Send the process message to the browser process.
			CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);

			retval = CefV8Value::CreateInt(0);

			handle = true;			
		}		
	}

	// 如果没有处理，则发异常信息给js
	if (!handle)
	{
		exception = "not implement function";
	}

	return true;
}

void CefV8HandlerImpl::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	// Remove any JavaScript callbacks registered for the context that has been released.
	if (!callback_map_.empty())
	{
		CallbackMap::iterator it = callback_map_.begin();
		for (; it != callback_map_.end();) {
			if (it->second.first->IsSame(context))
				callback_map_.erase(it++);
			else
				++it;
		}
	}
}

bool CefV8HandlerImpl::CallJsFunction(CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args)
{
	if (args->GetSize()<2
		|| args->GetType(0) != VTYPE_STRING
		|| args->GetType(1) != VTYPE_STRING)
	{
		return false;
	}
	
	const CefString& message_name = args->GetString(0);
	CallbackMap::const_iterator it = callback_map_.find(
		std::make_pair(message_name.ToString(),
			browser->GetIdentifier()));
	if (callback_map_.end() == it)
	{
		return false;
	}

	bool handled = false;
	// Execute the registered JavaScript callback if any.
	// Keep a local reference to the objects. The callback may remove itself
	// from the callback map.
	CefRefPtr<CefV8Context> context = it->second.first;
	CefRefPtr<CefV8Value> callback = it->second.second;

	// Enter the context.
	context->Enter();

	CefV8ValueList arguments;

	// First argument is the message name.
	//arguments.push_back(CefV8Value::CreateString(message_name));

	// Second argument is the message arguments.
	arguments.push_back(CefV8Value::CreateString(args->GetString(1)));

	// Execute the callback.
	CefRefPtr<CefV8Value> retval = callback->ExecuteFunction(NULL, arguments);
	if (retval.get()) {
		if (retval->IsBool())
			handled = retval->GetBoolValue();
	}

	// Exit the context.
	context->Exit();
		
	return handled;
}
