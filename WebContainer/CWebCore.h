#pragma once
#include <set>
#include <memory>
#include "AddrKey.h"

class CWebCore
{
public:
	CWebCore(const WebResourceKey& key);
	~CWebCore();

	void AddRequester(const RequesterKey& key);
	size_t DelRequester(const RequesterKey& key);
	size_t RequesterCount() const;

	typedef std::set<RequesterKey>::const_iterator reqIt;

	reqIt begin() { return m_setReqs.begin(); }
	reqIt end() { return m_setReqs.end(); }

	void SetAddr(const WebResourceKey& key)
	{
		m_addrKey = key;
	}

	void SetPrivate(bool b)
	{
		m_bIsPrivate = b;
	}

	void SetType(int i)
	{
		m_iType = i;
	}

	const WebResourceKey& GetAddrKey() const
	{
		return m_addrKey;
	}

	bool IsPrivate() const
	{
		return m_bIsPrivate;
	}

	int GetType() const
	{
		return m_iType;
	}	

private:
	WebResourceKey	m_addrKey;
	bool	m_bIsPrivate;
	int		m_iType;
	
	std::set<RequesterKey> m_setReqs;

};

typedef std::shared_ptr<CWebCore> WebCorePtr;