#include "WebEngineMgr.h"


WebEngineMgr::WebEngineMgr()
{
}


WebEngineMgr::~WebEngineMgr()
{
}

void WebEngineMgr::AddWebEngine(int iType, WebEnginePtr pInst)
{
	m_mapRequested[iType] = pInst;
}

void WebEngineMgr::DelWebEngine(int iType)
{
	m_mapRequested.erase(iType);
}

bool WebEngineMgr::IsRequested(int iType)
{
	auto it = m_mapRequested.find(iType);
	return it != m_mapRequested.end();
}

WebEnginePtr WebEngineMgr::FindWebInst(int iType)
{
	auto it = m_mapRequested.find(iType);
	if (it == m_mapRequested.end())
	{
		return nullptr;
	}
	return it->second;
}

WebEnginePtr WebEngineMgr::IsEngineExist(void* pInst)
{
	for (auto it = m_mapRequested.begin();
		it != m_mapRequested.end();
		it++)
	{
		if (it->second->inst == pInst)
		{
			return it->second;
		}
	}
	return nullptr;
}
