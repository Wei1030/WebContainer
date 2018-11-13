#pragma once
#include <map>
#include "include/cef_v8.h"

class CefV8HandlerImpl : public CefV8Handler
{
public:
	CefV8HandlerImpl();
	~CefV8HandlerImpl();

public:
	/**
	 *	CefV8Handler Methods, Which will be called when the V8 extension
	 *  is called in the Javascript environment
	 */
	virtual bool Execute(const CefString& name
		, CefRefPtr<CefV8Value> object
		, const CefV8ValueList& arguments
		, CefRefPtr<CefV8Value>& retval
		, CefString& exception);

	void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);

	bool CallJsFunction(CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args);

private:
	// Map of message callbacks.
	typedef std::map<std::pair<std::string, int>,
		std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value> > >
		CallbackMap;
	CallbackMap callback_map_;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CefV8HandlerImpl);
	DISALLOW_COPY_AND_ASSIGN(CefV8HandlerImpl);
};
