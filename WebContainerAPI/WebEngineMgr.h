#pragma once
#include <map>
#include <memory>
#include "InnerProtocol.h"

typedef std::shared_ptr<WebContainer_Load_Req> ReqPtr;

struct WebEngine
{
	void* inst;
	int iEngineType;
	bool bPrivate;

	std::map<void*, ReqPtr> mapReqs;
};

typedef std::shared_ptr<WebEngine> WebEnginePtr;

class WebEngineMgr
{
public:
	WebEngineMgr();
	~WebEngineMgr();	

	void AddWebEngine(int iType, WebEnginePtr pInst);
	void DelWebEngine(int iType);
	bool IsRequested(int iType);
	WebEnginePtr FindWebInst(int iType);

	WebEnginePtr IsEngineExist(void* pInst);
private:
	std::map<int, WebEnginePtr> m_mapRequested;
};

