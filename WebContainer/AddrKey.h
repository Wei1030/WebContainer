#pragma once
#include <string>

class CAddrKey
{
public:
	CAddrKey(const std::string& strAddr = "", void* hinst = 0)
	{
		m_strAddr = strAddr;
		m_hInstance = hinst;
	}
	~CAddrKey() {}

	CAddrKey& operator = (const std::string& strAddr) //赋值符重载;
	{
		m_strAddr = strAddr;
		return *this;
	}

	CAddrKey& operator = (void* hinst) //赋值符重载;
	{
		m_hInstance = hinst;
		return *this;
	}

	const std::string& GetAddr() const
	{
		return m_strAddr;
	}

	void* GetInstance() const
	{
		return m_hInstance;
	}

	bool IsValid() const
	{
		if (m_hInstance && m_strAddr.length())
		{
			return true;
		}

		return false;
	}

	bool  operator==(const CAddrKey& o) const
	{
		//有一个相等就认为相等;
		if (m_hInstance == o.m_hInstance
			|| m_strAddr == o.m_strAddr)
		{
			return true;
		}

		return false;
	}

	bool operator<(const CAddrKey& o) const
	{
		return m_hInstance < o.m_hInstance;
	}
private:
	std::string m_strAddr;
	void* m_hInstance;
};

typedef CAddrKey RequesterKey;
typedef CAddrKey WebResourceKey;