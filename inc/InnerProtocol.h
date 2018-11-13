#pragma once

#define METHOD_MAX_LENTH	64
#define ADDR_MAX_LENTH		260
#define URL_MAX_LENTH		1024

//ͨ�õĻظ�
struct Generally_Rsp
{
	int		iError;	//������,0����ɹ�
};

//////////////////////////////////////////////////////////////////////////
//��ȡ���������ʵ������
struct WebContainer_GetInstance_Req
{
	enum EngineType
	{
		Start,
		Chromium = 0,
		IE,
		End
	};

	int			iEngineType;					//��������
	bool		bPrivate;						//�Ƿ�˽��,��Ϊtrue,�򵥶�Ϊ����������һ��������Ҳ��Ṳ�������������
	char		szRspAddr[ADDR_MAX_LENTH];		//����ϱ���ַ����Ϊ�����ߵ�Ψһ��־ //��ʵ�֡�WebInstanceNotify���ӿ�
};

//��ȡ���������ʵ���ظ�
typedef Generally_Rsp WebContainer_GetInstance_Rsp;

//��ȡ���������ʵ��֪ͨ
struct WebContainer_GetInstance_Notify
{
	int		iEngineType;					//��������
	char	szEngineAddr[ADDR_MAX_LENTH];	//�����ʵ���ĵ�ַ;
};

//////////////////////////////////////////////////////////////////////////
//�ͷ������ʵ������
struct WebContainer_ReleaseInstance_Req
{	
	int		iEngineType;					//��������
	char	szReqAddr[ADDR_MAX_LENTH];	
};

//�ͷ������ʵ���ظ�
typedef Generally_Rsp WebContainer_ReleaseInstance_Rsp;




struct WebContainer_Load_Req
{
	char	szUrl[URL_MAX_LENTH];
	void*	hWnd;
	char	szRspAddr[ADDR_MAX_LENTH];
};

typedef Generally_Rsp WebContainer_Load_Rsp;

struct WebContainer_CallJs_Req
{
	void*	hWnd;
	char	szMethod[METHOD_MAX_LENTH];		//js�ķ�����
	char	szJsonArgs[1];					//json����,���ڽṹ����棬��0��β
};

typedef Generally_Rsp WebContainer_CallJs_Rsp;