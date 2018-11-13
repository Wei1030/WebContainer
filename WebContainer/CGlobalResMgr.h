#pragma once
#include <mutex>
#include <map>
#include "AddrKey.h"

class CGlobalResMgr
{
public:
	CGlobalResMgr();
	~CGlobalResMgr();

	static CGlobalResMgr& GetInstance();	

	void AddAddr(int iType, const WebResourceKey& addr);

	void DelAddr(int iType);

	WebResourceKey GetAddr(int iType);

private:
	std::mutex m_mtAddr;
	std::map<int, WebResourceKey> m_mapAddrs;
};

