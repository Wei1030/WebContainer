#include "CWebCore.h"



CWebCore::CWebCore(const WebResourceKey& key)
	: m_addrKey(key)
	, m_bIsPrivate(false)
	, m_iType(0)
{
}


CWebCore::~CWebCore()
{
}

void CWebCore::AddRequester(const RequesterKey& key)
{
	m_setReqs.insert(key);
}

size_t CWebCore::DelRequester(const RequesterKey& key)
{
	return m_setReqs.erase(key);
}

size_t CWebCore::RequesterCount() const
{
	return m_setReqs.size();
}
