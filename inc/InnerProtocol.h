#pragma once

#define METHOD_MAX_LENTH	64
#define ADDR_MAX_LENTH		260
#define URL_MAX_LENTH		1024

//通用的回复
struct Generally_Rsp
{
	int		iError;	//错误码,0代表成功
};

//////////////////////////////////////////////////////////////////////////
//获取浏览器引擎实例请求
struct WebContainer_GetInstance_Req
{
	enum EngineType
	{
		Start,
		Chromium = 0,
		IE,
		End
	};

	int			iEngineType;					//引擎类型
	bool		bPrivate;						//是否私有,若为true,则单独为请求者启动一个浏览器且不会共享给其他请求者
	char		szRspAddr[ADDR_MAX_LENTH];		//结果上报地址且作为请求者的唯一标志 //需实现“WebInstanceNotify”接口
};

//获取浏览器引擎实例回复
typedef Generally_Rsp WebContainer_GetInstance_Rsp;

//获取浏览器引擎实例通知
struct WebContainer_GetInstance_Notify
{
	int		iEngineType;					//引擎类型
	char	szEngineAddr[ADDR_MAX_LENTH];	//浏览器实例的地址;
};

//////////////////////////////////////////////////////////////////////////
//释放浏览器实例请求
struct WebContainer_ReleaseInstance_Req
{	
	int		iEngineType;					//引擎类型
	char	szReqAddr[ADDR_MAX_LENTH];	
};

//释放浏览器实例回复
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
	char	szMethod[METHOD_MAX_LENTH];		//js的方法名
	char	szJsonArgs[1];					//json参数,跟在结构体后面，以0结尾
};

typedef Generally_Rsp WebContainer_CallJs_Rsp;