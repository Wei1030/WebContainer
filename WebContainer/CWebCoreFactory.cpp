#include "CWebCoreFactory.h"



CWebCoreFactory::CWebCoreFactory()
{
}


CWebCoreFactory::~CWebCoreFactory()
{
}

void CWebCoreFactory::AddWebCore(int iType, WebCorePtr web)
{
	m_mapRes[iType] = web;
}

size_t CWebCoreFactory::DelWebCore(int iType)
{
	return m_mapRes.erase(iType);
}

WebCorePtr CWebCoreFactory::GetWebCore(int iType)
{
	auto it = m_mapRes.find(iType);
	if (it == m_mapRes.end())
	{
		return nullptr;
	}

	return it->second;
}
