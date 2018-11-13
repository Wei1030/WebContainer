#include "CGlobalResMgr.h"



CGlobalResMgr::CGlobalResMgr()
{
}


CGlobalResMgr::~CGlobalResMgr()
{
}

CGlobalResMgr& CGlobalResMgr::GetInstance()
{
	static CGlobalResMgr instance;
	return instance;
}

void CGlobalResMgr::AddAddr(int iType, const WebResourceKey& addr)
{
	std::lock_guard<std::mutex> guard(m_mtAddr);
	m_mapAddrs[iType] = addr;
}

void CGlobalResMgr::DelAddr(int iType)
{
	std::lock_guard<std::mutex> guard(m_mtAddr);
	m_mapAddrs.erase(iType);
}

WebResourceKey CGlobalResMgr::GetAddr(int iType)
{
	std::lock_guard<std::mutex> guard(m_mtAddr);
	auto it = m_mapAddrs.find(iType);
	if (it == m_mapAddrs.end())
	{
		return WebResourceKey();
	}

	return it->second;
}
