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

	/** @brief ���ӶϿ�֪ͨ;
 *	@param instance  �ͻ���ʵ��;
 *	@param usr		 �û�����;
 *  @return �ɹ�����0��ʧ�ܷ�������ֵ;
 *	@note

	���ص�����ʱ��;:
		1��Զ�̷���˹ر����ӻ�����쳣;
		2�����������Ͽ����ӣ�����SRPC_DestroyClient��;

		*���¼�����ʱ���û������SRPC_DestroyClient�ر��Ѿ���Ч��instance;
		*�����¼��������û���������SRPC_DestroyClient��������,�û��Ծɿ��Ե���SRPC_DestroyClient�رտͻ���;
		��ʱ����п��ܷ���ʧ�ܣ�������������κθ���Ӱ��;

 */
	int SRPC_API OnSRPCError(void* instance, void* usr)
	{
		CAddrKey key("",instance);
		CResMgr::GetInstance().PostWork_ConnectErr(key);
		return 0;
	}

}