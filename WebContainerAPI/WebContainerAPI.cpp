// WebContainerAPI.cpp : 定义 DLL 应用程序的导出函数。
//
#if (defined(_WIN32) || defined(_WIN64))
#define _CRT_SECURE_NO_WARNINGS
#include "header.h"
#else
#include <unistd.h>
#endif

#include "WebContainerAPI.h"
#include "InvokeQueue.h"


WEBCONTAINERAPI_API int WebContainerAPI_RegCbForJsCall(const char* method, Proc_FromJs fn)
{
	return InvokeQueue::GetInstance().RegCbForJsCall(method, fn);
}

WEBCONTAINERAPI_API int WebContainerAPI_Init(void)
{
	return InvokeQueue::GetInstance().Initialize();
}

WEBCONTAINERAPI_API int WebContainerAPI_SetWebStartCb(WebStartCb fn, void* usr)
{
	return InvokeQueue::GetInstance().SetWebStartCb(fn, usr);
}

WEBCONTAINERAPI_API int WebContainerAPI_Start(const WebContainerInfo* pWebInfo)
{
	return InvokeQueue::GetInstance().PostWork_StartWebEngine(pWebInfo);
}

WEBCONTAINERAPI_API int WebContainerAPI_Stop(WebEngineType iEngineType)
{
	return InvokeQueue::GetInstance().PostWork_StopWebEngine(iEngineType);
}

WEBCONTAINERAPI_API int WebContainerAPI_LoadUrl(WebEngineType type, const char* url, void* hWnd)
{
	return InvokeQueue::GetInstance().PostWork_LoadUrl(type, url, hWnd);
}

WEBCONTAINERAPI_API int WebContainerAPI_CallJs(WebEngineType type, void* hWnd, const char* method, const char* data, unsigned int size)
{
	return InvokeQueue::GetInstance().PostWork_CallJs(type, hWnd, method, data, size);
}

WEBCONTAINERAPI_API int WebContainerAPI_Uninit(void)
{
	return InvokeQueue::GetInstance().Uninitialize();
}

