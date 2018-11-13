#pragma once
#include <string>
#include <map>
#include <thread>
#include "AnyTaskQueue.hpp"
#include "WebContainerAPI.h"
#include "sRPC/sRPC.h"
#include "WebEngineMgr.h"

struct WebContainerInfo;
struct WebContainer_GetInstance_Notify;

class InvokeQueue
{
public:
	static InvokeQueue& GetInstance();

	int RegCbForJsCall(const std::string& strMethod, Proc_FromJs fn);
	int Initialize();
	int Uninitialize();

	int SetWebStartCb(WebStartCb fn, void* usr) {
		m_pfnWebStartCb = fn; 
		m_pfnWebStartCbUsrData = usr;
		return 0;
	}

	int PostWork_StartWebEngine(const WebContainerInfo* pWebInfo);

	int PostWork_WebInstanceNotify(const WebContainer_GetInstance_Notify* pNotify);

	int PostWork_WebContainerError(void* pInst);

	int PostWork_StopWebEngine(int iEngineType);

	int PostWork_LoadUrl(int iEngineType, const char* url, void* hWnd);

	int PostWork_CallJs(int iEngineType, void* hWnd, const char* method, const char* data, unsigned int size);
	
private:
	InvokeQueue();
	~InvokeQueue();
	InvokeQueue(const InvokeQueue &) = delete;
	void operator=(const InvokeQueue&) = delete;

	int initSRPCLib();
	int initIPC();
	int uninitIPC();
	int uninitSRPCLib();

	void startWebEngine(const WebContainerInfo* pWebInfo);
	void webInstanceNotify(const WebContainer_GetInstance_Notify* pNotify);
	void webContainerError(void* pInst);
	void reConnWebContainer();
	void reConnWebEngine(WebEnginePtr pWeb);
	void stopWebEngine(int iEngineType);
	void loadUrl(int iEngineType,ReqPtr pReq);
	void callJs(int iEngineType, WebContainer_CallJs_Req* pReq, size_t size);
	
private:
	static int MODULE_API WebInstanceNotify(const void* data_in, unsigned int size_in, COMPLETIONPROC proc, void* usr);
	static int SRPC_API OnWebContainerError(void* instance, void* usr);

private:
	AnyTaskQueue	m_queue;
private:
	std::map<std::string, Proc_FromJs> m_map;
	int	m_initialized;
	std::string m_strLocalAddr;

	void* m_localServer;
	void* m_instWebContainer;

	WebStartCb m_pfnWebStartCb;
	void*	m_pfnWebStartCbUsrData;

	WebEngineMgr m_engineMgr;
};

