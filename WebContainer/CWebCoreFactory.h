#pragma once
#include <map>
#include "CWebCore.h"



class CWebCoreFactory
{
public:
	CWebCoreFactory();
	~CWebCoreFactory();

	void AddWebCore(int iType, WebCorePtr web);

	size_t DelWebCore(int iType);

	WebCorePtr GetWebCore(int iType);

private:
	std::map<int, WebCorePtr> m_mapRes;
};

typedef std::shared_ptr<CWebCoreFactory> FactoryPtr;

