#define _CRT_SECURE_NO_WARNINGS
#include "CResMgr.h"
#include "CGlobalResMgr.h"
#include "InnerProtocol.h"
#include "sRPC/sRPC.h"

CResMgr& CResMgr::GetInstance()
{
	static CResMgr instance;
	return instance;
}

CResMgr::CResMgr()
{
	m_queue.Start();
}

CResMgr::~CResMgr()
{
	m_queue.Stop();
}

void CResMgr::PostWork_GetWebInstance(WebContainer_GetInstance_Req* pReq)
{
	if (!pReq)
	{
		return;
	}
	m_queue.PostTask(&CResMgr::procGetWebInstance, this, pReq);
}

void CResMgr::PostWork_ReleaseWebInstance(WebContainer_ReleaseInstance_Req* pReq)
{
	if (!pReq)
	{
		return;
	}
	m_queue.PostTask(&CResMgr::procReleaseWebInstance, this, pReq);
}

void CResMgr::PostWork_ConnectErr(const CAddrKey& addr)
{
	m_queue.PostTask(&CResMgr::procConnectErr, this, addr);
}

void CResMgr::procGetWebInstance(WebContainer_GetInstance_Req* pReq)
{
	do 
	{
		RequesterKey key(pReq->szRspAddr);
		FactoryPtr pFac;
		std::map<RequesterKey, FactoryPtr>::iterator it = m_mapReqs.find(key);
		if (it == m_mapReqs.end())
		{
			pFac = FactoryPtr(new CWebCoreFactory);
		}
		else
		{
			pFac = it->second;
		}

		if (!pFac)
		{
			break;
		}

		WebCorePtr pWeb = pFac->GetWebCore(pReq->iEngineType);
		if (pWeb)
		{//1、已经申请过，直接再次回复地址
			notifyWebAddr(key, pWeb->GetAddrKey(),pWeb->GetType());
			break;
		}

		//2、请求者没有记录的web
		//是否已连接上报地址
		if (!key.IsValid())
		{
			//先连接结果上报地址
			key = SRPC_CreateClient(FD_LOCAL, pReq->szRspAddr, 0);
			if (!key.IsValid())
			{
				break;
			}
		}

		//公私判断
		if (pReq->bPrivate)
		{
			pWeb = createOneCore(pReq->iEngineType);
		}
		else
		{
			pWeb = m_mapRes[getGlobalWeb(pReq->iEngineType)];
		}

		if (!pWeb)
		{
			break;
		}
		pWeb->SetType(pReq->iEngineType);
		pWeb->SetPrivate(pReq->bPrivate);
		pWeb->AddRequester(key);
		pFac->AddWebCore(pReq->iEngineType, pWeb);
		m_mapReqs[key] = pFac;
		notifyWebAddr(key, pWeb->GetAddrKey(),pWeb->GetType());

	} while (0);

	delete pReq;	
}

void CResMgr::procReleaseWebInstance(WebContainer_ReleaseInstance_Req* pReq)
{
	doWebRelease(RequesterKey(pReq->szReqAddr), pReq->iEngineType);
	delete pReq;
}

void CResMgr::procConnectErr(const CAddrKey& addr)
{
	auto webIt = m_mapRes.find(addr);
	if (m_mapRes.end() != webIt)
	{		
		doWebRelease(webIt->second);
		return;
	}

	for (int i = WebContainer_GetInstance_Req::Start; i < WebContainer_GetInstance_Req::End; i++)
	{
		doWebRelease(addr, i);
	}
}

WebResourceKey CResMgr::getGlobalWeb(int iType)
{
	WebResourceKey globalAddr = CGlobalResMgr::GetInstance().GetAddr(iType);
	if (globalAddr.IsValid())
	{
		return globalAddr;
	}

	WebCorePtr p = createOneCore(iType);
	if (!p)
	{
		return WebResourceKey();
	}
	p->SetPrivate(false);
	CGlobalResMgr::GetInstance().AddAddr(iType, p->GetAddrKey());

	return p->GetAddrKey();
}

WebCorePtr CResMgr::createOneCore(int iType)
{
	char addrName[ADDR_MAX_LENTH] = { 0 };
	void* inst = SRPC_CreateProvider(type2str(iType).c_str(), addrName, ADDR_MAX_LENTH);

	WebCorePtr p(new(std::nothrow) CWebCore(WebResourceKey(addrName, inst)));
	if (p)
	{
		m_mapRes[p->GetAddrKey()] = p;
	}
	return p;
}

void CResMgr::notifyWebAddr(const RequesterKey& req, const WebResourceKey& res, int type)
{
	if (!req.IsValid() || !res.IsValid())
	{
		return;
	}
	if (res.GetAddr().length() >= ADDR_MAX_LENTH)
	{
		return;
	}
	WebContainer_GetInstance_Notify nf = { 0 };
	nf.iEngineType = type;
	strncpy(nf.szEngineAddr, res.GetAddr().c_str(), ADDR_MAX_LENTH);

	SRPC_AsyncCall(req.GetInstance(), "WebInstanceNotify", &nf, sizeof(WebContainer_GetInstance_Notify), 0, 0);
}

bool CResMgr::doWebRelease(const RequesterKey& reqKey, int iType)
{
	auto it = m_mapReqs.find(reqKey);
	if (it == m_mapReqs.end())
	{
		return false;
	}
	FactoryPtr pFac = it->second;
	WebCorePtr pWeb = pFac->GetWebCore(iType);
	if (!pWeb)
	{
		return false;
	}
	if (0 == pFac->DelWebCore(iType))
	{
		SRPC_DestroyClient(it->first.GetInstance());
		m_mapReqs.erase(it);
	}	

	if (pWeb->DelRequester(reqKey))
	{		
		return true;
	}
	
	m_mapRes.erase(pWeb->GetAddrKey());
	if (false == pWeb->IsPrivate())
	{
		CGlobalResMgr::GetInstance().DelAddr(iType);
	}
	SRPC_DestroyClient(pWeb->GetAddrKey().GetInstance());
	return true;
}

bool CResMgr::doWebRelease(WebCorePtr pWeb)
{
	if (false == pWeb->IsPrivate())
	{
		CGlobalResMgr::GetInstance().DelAddr(pWeb->GetType());
	}
	SRPC_DestroyClient(pWeb->GetAddrKey().GetInstance());
	m_mapRes.erase(pWeb->GetAddrKey());

	FactoryPtr pFac;
	for (auto it = pWeb->begin(); it!=pWeb->end(); it++)
	{
		auto itFac = m_mapReqs.find(*it);
		if (m_mapReqs.end() == itFac)
		{
			continue;
		}
		pFac = itFac->second;
		if (!pFac)
		{
			continue;
		}
		if (0 == pFac->DelWebCore(pWeb->GetType()))
		{
			SRPC_DestroyClient(itFac->first.GetInstance());
			m_mapReqs.erase(itFac);
		}		
	}
	return true;
}

std::string CResMgr::type2str(int type)
{
	std::string strKey;
	if (WebContainer_GetInstance_Req::Chromium == type)
	{
		strKey = "Chromium";
	}
	else if (WebContainer_GetInstance_Req::IE == type)
	{
		strKey = "IE";
	}

	return strKey;
}

