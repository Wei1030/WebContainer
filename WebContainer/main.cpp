// WebContainer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <atomic>
#include <chrono>    // std::chrono::seconds
#include <thread> 
#include <string>
#include "WebContainer.h"
#include "SystemTraits.h"


static std::atomic<int>  g_ref(0);
static bool g_bExit = false;
static void* g_srvInstance = nullptr;
/*
1、有客户端连接;type = 0
2、远程客户端主动关闭或出现异常导致的连接断开;type = 1
*/
int SRPC_API OnClientError(void* instance, int type, void* usr)
{
	if (0 == type)
	{
		g_ref++;
	}
	else if (1 == type)
	{
		if (0 == --g_ref)
		{
			g_bExit = true;
		}
	}
	return 0;
}

void* SRPC_API RequesterChecker(void* usr)
{
	for (int i = 0; 0 == g_ref && i < 5; ++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (0 == g_ref)
	{
		g_bExit = true;
	}

	return 0;
}

int InitSRPC(const std::string& dir)
{
	SRPC_INIT_BEGIN()
		EXPORT_PROC("WebContainer.GetInstance", &WebContainer::GetInstance)
		EXPORT_PROC("WebContainer.ReleaseInstance", &WebContainer::ReleaseInstance)
	SRPC_INIT_END()

	if (SRPC_INIT_FAIL)
	{
		return -1;
	}

	SRPC_SetConnErrCb(NULL, &WebContainer::OnSRPCError, NULL);

	SRPC_SetAcptEventCb(NULL, &OnClientError, NULL);
	g_srvInstance = SRPC_CreateServer(FD_LOCAL, "WebContainer{60FEC64F-41F8-4EBD-A4DC-01A64E4A0C43}", 0);
	if (NULL == g_srvInstance)
	{
		return -1;
	}

	std::string strCfgPath = dir + "\\WebContainer.xml";
	if (-1 == SRPC_InitFrame(strCfgPath.c_str(), 0))
	{
		return -1;
	}
	
	//5秒内发起者未连接则退出
	return SRPC_ThreadPoolWork(NULL, &RequesterChecker, NULL);
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

	if (-1 == InitSRPC(SystemTraits::GetExeDir()))
	{
		return -1;
	}

	while (false == g_bExit)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (g_srvInstance)
	{
		SRPC_DestroyServer(g_srvInstance);
	}

	SRPC_Uninit();
    return 0;
}
