#include "WebContainer.h"
#include "InnerProtocol.h"
#include "CResMgr.h"


namespace WebContainer
{
	int MODULE_API GetInstance(const void* data_in, unsigned int size_in, COMPLETIONPROC proc, void* usr)
	{
		int iRetVal = -1;
		
		do 
		{
			if (nullptr == data_in ||  size_in < sizeof(WebContainer_GetInstance_Req))
			{
				break;
			}

			WebContainer_GetInstance_Req* pReq = new(std::nothrow) WebContainer_GetInstance_Req;
			memcpy(pReq, data_in, sizeof(WebContainer_GetInstance_Req));

			CResMgr::GetInstance().PostWork_GetWebInstance(pReq);

			iRetVal = 0;
		} while (0);

		WebContainer_GetInstance_Rsp rsp = { iRetVal };
		if (proc)
		{
			proc(rsp.iError, &rsp, sizeof(rsp), usr);
		}
		return 0;
	}

	int MODULE_API ReleaseInstance(const void* data_in, unsigned int size_in, COMPLETIONPROC proc, void* usr)
	{
		int iRetVal = -1;

		do
		{
			if (nullptr == data_in || size_in < sizeof(WebContainer_ReleaseInstance_Req))
			{
				break;
			}

			WebContainer_ReleaseInstance_Req* pReq = new(std::nothrow) WebContainer_ReleaseInstance_Req;
			memcpy(pReq, data_in, size_in);

			CResMgr::GetInstance().PostWork_ReleaseWebInstance(pReq);

			iRetVal = 0;
		} while (0);

		WebContainer_ReleaseInstance_Rsp rsp = { iRetVal };
		if (proc)
		{
			proc(rsp.iError, &rsp, sizeof(rsp), usr);
		}
		return 0;
	}

	/** @brief 连接断开通知;
 *	@param instance  客户端实例;
 *	@param usr		 用户数据;
 *  @return 成功返回0，失败返回其他值;
 *	@note

	本回调发生时机;:
		1、远程服务端关闭连接或出现异常;
		2、本地主动断开连接（调用SRPC_DestroyClient）;

		*该事件发生时，用户需调用SRPC_DestroyClient关闭已经无效的instance;
		*若该事件是由于用户主动调用SRPC_DestroyClient而产生的,用户仍旧可以调用SRPC_DestroyClient关闭客户端;
		这时其很有可能返回失败，但并不会产生任何负面影响;

 */
	int SRPC_API OnSRPCError(void* instance, void* usr)
	{
		CAddrKey key("",instance);
		CResMgr::GetInstance().PostWork_ConnectErr(key);
		return 0;
	}

}