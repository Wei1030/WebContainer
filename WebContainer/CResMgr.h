#pragma once
#include <map>

#include "AnyTaskQueue.hpp"
#include "CWebCoreFactory.h"

struct WebContainer_GetInstance_Req;
struct WebContainer_ReleaseInstance_Req;

class CResMgr
{
public:
	
	static CResMgr& GetInstance();

	void PostWork_GetWebInstance(WebContainer_GetInstance_Req* pReq);

	void PostWork_ReleaseWebInstance(WebContainer_ReleaseInstance_Req* pReq);

	void PostWork_ConnectErr(const CAddrKey& addr);

private:
	CResMgr();
	~CResMgr();
	CResMgr(const CResMgr &) = delete;
	void operator=(const CResMgr&) = delete;

	void procGetWebInstance(WebContainer_GetInstance_Req* pReq);
	void procReleaseWebInstance(WebContainer_ReleaseInstance_Req* pReq);
	void procConnectErr(const CAddrKey& addr);

	WebResourceKey getGlobalWeb(int iType);

	WebCorePtr createOneCore(int iType);

	void notifyWebAddr(const RequesterKey& req, const WebResourceKey& res,int type);

	bool doWebRelease(const RequesterKey& reqKey,int iType);
	bool doWebRelease(WebCorePtr pWeb);

	std::string type2str( int type);

private:
	AnyTaskQueue	m_queue;


	std::map<WebResourceKey, WebCorePtr> m_mapRes;
	std::map<RequesterKey, FactoryPtr> m_mapReqs;
};

