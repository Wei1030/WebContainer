#if (defined(_WIN32) || defined(_WIN64))
#define _CRT_SECURE_NO_WARNINGS
#include "header.h"
#else
#include <unistd.h>
#endif
#include <chrono>    // std::chrono::seconds
#include "InvokeQueue.h"
#include "SystemTraits.h"
#include "InnerProtocol.h"

int MODULE_API InvokeQueue::WebInstanceNotify(const void* data_in, unsigned int size_in, COMPLETIONPROC proc, void* usr)
{
	int iRetVal = -1;

	do
	{
		if (nullptr == data_in || size_in < sizeof(WebContainer_GetInstance_Notify))
		{
			break;
		}	

		iRetVal = InvokeQueue::GetInstance().PostWork_WebInstanceNotify((WebContainer_GetInstance_Notify*)data_in);

	} while (0);
	
	return iRetVal;
}

int SRPC_API InvokeQueue::OnWebContainerError(void* instance, void* usr)
{
	return InvokeQueue::GetInstance().PostWork_WebContainerError(instance);
}

InvokeQueue& InvokeQueue::GetInstance()
{
	static InvokeQueue instance;
	return instance;
}

int InvokeQueue::RegCbForJsCall(const std::string& strMethod, Proc_FromJs fn)
{
	if (strMethod.length() >= MODULE_KEY_MAXLENTH)
	{
		return -1;
	}
	return m_map.insert(std::make_pair(strMethod, fn)).second ? 0 : -1;
}

int InvokeQueue::Initialize()
{
	if (m_initialized)
	{
		m_initialized++;
		return 0;
	}

	if (initSRPCLib())
	{
		return -1;
	}

	if (initIPC())
	{
		return -1;
	}

	if (false == m_queue.Start())
	{
		return -1;
	}
	
	m_initialized++;
	return 0;
}

int InvokeQueue::Uninitialize()
{
	if (m_initialized == 0)
	{
		return -1;
	}

	if (--m_initialized)
	{
		return -1;
	}

	m_queue.Stop();
	uninitIPC();
	uninitSRPCLib();
	return 0;
}

int InvokeQueue::PostWork_StartWebEngine(const WebContainerInfo* pWebInfo)
{
	if (pWebInfo->uiSize < sizeof(WebContainerInfo))
	{
		return -1;
	}
	WebContainerInfo* p = new(std::nothrow) WebContainerInfo;
	if (nullptr == p)
	{
		return -2;
	}

	memcpy(p, pWebInfo, sizeof(WebContainerInfo));

	m_queue.PostTask(&InvokeQueue::startWebEngine,this,p);
	
	return 0;
}

int InvokeQueue::PostWork_WebInstanceNotify(const WebContainer_GetInstance_Notify* pNotify)
{
	WebContainer_GetInstance_Notify* pNtf = new(std::nothrow) WebContainer_GetInstance_Notify;
	if (nullptr == pNtf)
	{
		return -1;
	}
	memcpy(pNtf, pNotify, sizeof(WebContainer_GetInstance_Notify));

	m_queue.PostTask(&InvokeQueue::webInstanceNotify, this, pNtf);

	return 0;
}

int InvokeQueue::PostWork_WebContainerError(void* pInst)
{
	m_queue.PostTask(&InvokeQueue::webContainerError, this, pInst);
	return 0;
}

int InvokeQueue::PostWork_StopWebEngine(int iEngineType)
{
	m_queue.PostTask(&InvokeQueue::stopWebEngine, this, iEngineType);
	return 0;
}

int InvokeQueue::PostWork_LoadUrl(int iEngineType, const char* url, void* hWnd)
{
	if (nullptr == url || strlen(url) >= URL_MAX_LENTH)
	{
		return -1;
	}

	ReqPtr pReq(new(std::nothrow) WebContainer_Load_Req);
	if (!pReq)
	{
		return -1;
	}
	strncpy(pReq->szUrl, url, URL_MAX_LENTH);
	pReq->hWnd = hWnd;
	strncpy(pReq->szRspAddr, m_strLocalAddr.c_str(), ADDR_MAX_LENTH);

	m_queue.PostTask(&InvokeQueue::loadUrl, this, iEngineType, pReq);
	return 0;
}

int InvokeQueue::PostWork_CallJs(int iEngineType, void* hWnd, const char* method, const char* data, unsigned int size)
{
	if (nullptr == method || strlen(method) >= METHOD_MAX_LENTH)
	{
		return -1;
	}

	size_t tSize = sizeof(WebContainer_CallJs_Req) + size;
	char* pData = new(std::nothrow) char[tSize];
	if (nullptr == pData)
	{
		return -1;
	}
	WebContainer_CallJs_Req* pReq = (WebContainer_CallJs_Req*)pData;
	pReq->hWnd = hWnd;
	strncpy(pReq->szMethod, method, METHOD_MAX_LENTH);

	if (data && size)
	{
		memcpy(pReq->szJsonArgs, data, size);
	}

	m_queue.PostTask(&InvokeQueue::callJs, this, iEngineType, pReq, tSize);
	return 0;
}

InvokeQueue::InvokeQueue()
	: m_initialized(0)
	, m_localServer(nullptr)
	, m_instWebContainer(nullptr)
	, m_pfnWebStartCb(nullptr)
	, m_pfnWebStartCbUsrData(nullptr)
{
	m_strLocalAddr = SystemTraits::GetExeName();
	m_strLocalAddr += "_WebContainerRequester";
	char szPid[32] = { 0 };
	snprintf(szPid, 31, "%d", _getpid());
	m_strLocalAddr += szPid;
}

InvokeQueue::~InvokeQueue()
{
}

int InvokeQueue::initSRPCLib()
{
	size_t total = m_map.size();
	PROCINFO* p = new PROCINFO[total + 1];
	if (nullptr == p)
	{
		return -1;
	}
	size_t i = 0;
	for (auto it = m_map.begin();
		it != m_map.end();
		it++, i++)
	{
		strncpy(p[i].szKey, it->first.c_str(), MODULE_KEY_MAXLENTH);
		p[i].pAddr = (Module_Proc)it->second;
	}

	strncpy(p[total].szKey, "WebInstanceNotify", MODULE_KEY_MAXLENTH);
	p[total].pAddr = &InvokeQueue::WebInstanceNotify;

	int srpc_ret = SRPC_Init(p, total + 1);
	delete p;
	
	return srpc_ret;	
}

int InvokeQueue::initIPC()
{
	SRPC_SetConnErrCb(0,&InvokeQueue::OnWebContainerError,this);

	m_localServer = SRPC_CreateServer(FD_LOCAL, m_strLocalAddr.c_str(), 0);
	if (nullptr == m_localServer)
	{
		return -1;
	}

	m_instWebContainer = SRPC_CreateClient(FD_LOCAL, "WebContainer{60FEC64F-41F8-4EBD-A4DC-01A64E4A0C43}", 0);
	if (nullptr == m_instWebContainer)
	{
		return -1;
	}

	return 0;
}

int InvokeQueue::uninitIPC()
{
	if (m_instWebContainer)
	{
		SRPC_DestroyClient(m_instWebContainer);
		m_instWebContainer = nullptr;
	}

	if (m_localServer)
	{
		SRPC_DestroyServer(m_localServer);
		m_localServer = nullptr;
	}
	return 0;
}

int InvokeQueue::uninitSRPCLib()
{
	SRPC_Uninit();
	return 0;
}

void InvokeQueue::startWebEngine(const WebContainerInfo* pWebInfo)
{
	int iError = -1;
	do 
	{
		if (0 == m_initialized)
		{
			iError = 1;
			break;
		}

		if (m_engineMgr.IsRequested(pWebInfo->iEngineType))
		{
			iError = 2;
			break;
		}

		if (nullptr == m_instWebContainer)
		{
			iError = 3;
			break;
		}

		WebEnginePtr pEngine(new(std::nothrow) WebEngine);
		if (!pEngine)
		{
			iError = 4;
			break;
		}

		WebContainer_GetInstance_Req req = { 0 };
		req.iEngineType = pWebInfo->iEngineType;
		req.bPrivate = pWebInfo->bPrivate;
		strncpy(req.szRspAddr, m_strLocalAddr.c_str(), ADDR_MAX_LENTH);

		pEngine->bPrivate = req.bPrivate;
		pEngine->iEngineType = req.iEngineType;
		pEngine->inst = nullptr;
		m_engineMgr.AddWebEngine(pEngine->iEngineType, pEngine);

		if (-1 == SRPC_AsyncCall(m_instWebContainer, "WebContainer.GetInstance", &req, sizeof(req), 0, 0))
		{
			iError = 5;
			m_engineMgr.DelWebEngine(pEngine->iEngineType);
			break;
		}
		iError = 0;

	} while (0);

// 	if (m_pfnWebStartCb)
// 	{
// 		m_pfnWebStartCb(iError,pWebInfo->iEngineType,m_pfnWebStartCbUsrData);
// 	}

	delete pWebInfo;
}

void InvokeQueue::webInstanceNotify(const WebContainer_GetInstance_Notify* pNotify)
{
	int iState = -1;
	WebEnginePtr p;
	do 
	{
		if (0 == m_initialized)
		{
			iState = 1;
			break;
		}

		if (!m_engineMgr.IsRequested(pNotify->iEngineType))
		{
			iState = 1;
			break;
		}

		p = m_engineMgr.FindWebInst(pNotify->iEngineType);

		void* pInst = SRPC_CreateClient(FD_LOCAL, pNotify->szEngineAddr, 0, 0);
		if (nullptr == pInst)
		{
			break;
		}
		
		if (p)
		{
			if (p->inst)
			{
				SRPC_DestroyClient(p->inst);
			}
			p->inst = pInst;
			for (auto it = p->mapReqs.begin();
				it != p->mapReqs.end();
				it++)
			{
				SRPC_AsyncCall(p->inst, "WebContainer.Load", it->second.get(), sizeof(WebContainer_Load_Req), NULL, 0);				
			}
			iState = 0;
		}

	} while (0);	

	if (m_pfnWebStartCb)
	{
		m_pfnWebStartCb(iState, (WebEngineType)pNotify->iEngineType, m_pfnWebStartCbUsrData);
	}

	delete pNotify;

	if (-1 == iState && p)
	{
		m_queue.PostDelayTask(1000, &InvokeQueue::reConnWebEngine, this, p);
	}

	
}

void InvokeQueue::webContainerError(void* pInst)
{	
	SRPC_DestroyClient(pInst);

	if (pInst == m_instWebContainer)
	{
		m_instWebContainer = nullptr;		
		m_queue.PostDelayTask(1000, &InvokeQueue::reConnWebContainer, this);				
		return;
	}

	WebEnginePtr p = m_engineMgr.IsEngineExist(pInst);
	if (p)
	{
		p->inst = nullptr;
		m_queue.PostDelayTask(1000, &InvokeQueue::reConnWebEngine, this, p);
		return;
	}
}

void InvokeQueue::reConnWebContainer()
{
	if (0 == m_initialized)
	{
		return;
	}

	if (m_instWebContainer)
	{
		return;
	}

	m_instWebContainer = SRPC_CreateClient(FD_LOCAL, "WebContainer{60FEC64F-41F8-4EBD-A4DC-01A64E4A0C43}", 0);
	if (m_instWebContainer)
	{
		return;
	}

	m_queue.PostDelayTask(1000, &InvokeQueue::reConnWebContainer, this);
}

void InvokeQueue::reConnWebEngine(WebEnginePtr pWeb)
{
	int iState = -1;
	do
	{
		if (0 == m_initialized)
		{
			iState = 0;
			break;
		}

		if (!m_engineMgr.IsRequested(pWeb->iEngineType))
		{
			iState = 0;
			break;
		}

		if (nullptr == m_instWebContainer)
		{
			break;
		}		

		WebContainer_GetInstance_Req req = { 0 };
		req.iEngineType = pWeb->iEngineType;
		req.bPrivate = pWeb->bPrivate;
		strncpy(req.szRspAddr, m_strLocalAddr.c_str(), ADDR_MAX_LENTH);

		pWeb->inst = nullptr;
		m_engineMgr.AddWebEngine(pWeb->iEngineType, pWeb);

		if (-1 == SRPC_AsyncCall(m_instWebContainer, "WebContainer.GetInstance", &req, sizeof(req), 0, 0))
		{
			break;
		}
		iState = 0;

	} while (0);

	if (iState)
	{
		m_queue.PostDelayTask(1000, &InvokeQueue::reConnWebEngine, this, pWeb);
	}
}

void InvokeQueue::stopWebEngine(int iEngineType)
{
	int iError = -1;
	do
	{
		if (0 == m_initialized)
		{
			iError = 1;
			break;
		}

		if (!m_engineMgr.IsRequested(iEngineType))
		{
			iError = 2;
			break;
		}

		WebEnginePtr p = m_engineMgr.FindWebInst(iEngineType);
		if (p && p->inst)
		{
			SRPC_DestroyClient(p->inst);			
		}

		m_engineMgr.DelWebEngine(iEngineType);

		iError = 0;

	} while (0);	
}

void InvokeQueue::loadUrl(int iEngineType,ReqPtr pReq)
{
	int iState = -1;
	WebEnginePtr p;
	do
	{
		if (0 == m_initialized)
		{
			break;
		}

		if (!m_engineMgr.IsRequested(iEngineType))
		{
			break;
		}

		p = m_engineMgr.FindWebInst(iEngineType);
		if (!p || nullptr == p->inst)
		{
			break;
		}
		
		p->mapReqs[pReq->hWnd] = pReq;
		if ( SRPC_AsyncCall(p->inst, "WebContainer.Load", pReq.get(), sizeof(WebContainer_Load_Req), NULL, 0))
		{
			break;
		}	

	} while (0);
}

void InvokeQueue::callJs(int iEngineType, WebContainer_CallJs_Req* pReq, size_t size)
{
	int iState = -1;
	WebEnginePtr p;
	do
	{
		if (0 == m_initialized)
		{
			break;
		}

		if (!m_engineMgr.IsRequested(iEngineType))
		{
			break;
		}

		p = m_engineMgr.FindWebInst(iEngineType);
		if (!p || nullptr == p->inst)
		{
			break;
		}

		if (SRPC_AsyncCall(p->inst, "WebContainer.CallJs", pReq, size, NULL, 0))
		{
			break;
		}

	} while (0);
}

