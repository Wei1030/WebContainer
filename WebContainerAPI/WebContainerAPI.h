#pragma once

#if (defined(_WIN32) || defined(_WIN64))
#  if defined(WEBCONTAINERAPI_EXPORTS)
#    define WEBCONTAINERAPI_API __declspec(dllexport)
#  else
#    define WEBCONTAINERAPI_API __declspec(dllimport)
#  endif
#  define WEBCONTAINERAPI_CB __stdcall
#elif defined(__linux__)
#  define WEBCONTAINERAPI_API
#  define WEBCONTAINERAPI_CB
#else
#  define WEBCONTAINERAPI_API
#  define WEBCONTAINERAPI_CB
#endif

//////////////////////////////////////////////////////////////////////////
enum WebEngineType
{	
	Chromium = 0,
	IE	
};

//////////////////////////////////////////////////////////////////////////
struct WebContainerInfo
{
	unsigned int	uiSize;				//本结构大小
	WebEngineType	iEngineType;		//引擎类型
	bool			bPrivate;			//是否私有,若为true,则单独为请求者启动一个浏览器且不会共享给其他请求者
};

//////////////////////////////////////////////////////////////////////////
/** @brief js调用c++方法;
 *  @param const void* [in] 参数;
 *  @param unsigned int [in] 参数长度;
 *  @param void* 保留
 *  @return 成功返回0，失败返回其他值;
 *	@note 需要在WebContainerAPI_Init调用之前调用，Init之后调用该函数无效;
		限制js只能传递一个字符串，建议json形式传递
		该回调有可能会被*多线程调用*
 */
typedef int (WEBCONTAINERAPI_CB *Proc_FromJs)(const void* data_in, unsigned int size_in, void* reserve1,void* reserve2);

/** @brief 浏览器启动结果通知;
 *  @param unsigned int [in] 错误码;0表示启动成功，其他表示启动失败
 *  @param WebEngineType [in] 引擎类型;
 *  @param void* [in] 用户数据
 *  @return 无意义;
 *	@note 用户请求浏览器后，需要等待该回调被调用后，其他url请求接口才能被正确执行;
		该回调有可能会被*多线程调用*
 */
typedef int (WEBCONTAINERAPI_CB *WebStartCb)( unsigned int error, WebEngineType type,void* usr);

//////////////////////////////////////////////////////////////////////////
/** @brief 注册c++函数供js调用;只能注册以Proc_FromJs的形式，接收一个字符串参数
 *  @param const char* [in] 函数名;
 *  @param Proc_FromJs [in] 函数地址;
 *  @return 成功返回0，失败返回其他值;
 *	@note 需要在WebContainerAPI_Init调用之前调用，Init之后调用该函数无效;
		该接口*非线程安全*
 */
WEBCONTAINERAPI_API int WebContainerAPI_RegCbForJsCall(const char* method, Proc_FromJs fn);

/** @brief 初始化WebContainerAPI;
 *  @return 成功返回0，失败返回其他值;
 *	@note 注意：供js调用的回调函数需要在初始化前调用WebContainerAPI_RegCbForJsCall注册!
		该接口*非线程安全*
 */
WEBCONTAINERAPI_API int WebContainerAPI_Init(void);

/** @brief 设置浏览器启动结果回调;
 *  @param WebStartCb [in] 浏览器启动结果回调地址;
 *  @param void* [in] 用户数据;
 *  @return 成功返回0，失败返回其他值;
 *	@note 启动浏览器是异步操作，请在启动前设置该回调以接收启动结果
		该接口*非线程安全*
 */
WEBCONTAINERAPI_API int WebContainerAPI_SetWebStartCb(WebStartCb fn,void* usr);

/** @brief 启动浏览器;
 *  @param const WebContainerInfo* [in] 浏览器信息;
 *  @return 成功返回0，失败返回其他值;
 *	@note	一般情况下（WebContainerInfo::bPrivate==false），WebContainer的浏览器在本机上为所有进程共用，
			用户调用该接口请求某内核的浏览器后，若该内核的公用浏览器还未启动，WebContainer会启动之，并将
			其的引用计数递增；若该内核的公用浏览器已经启动，WebContainer则只是将其引用计数递增。

			每个用户（exe）同种内核的浏览器只能拥有一个引用，若用户已经引用了同种内核的浏览器，再次调用该
			接口则会失败（失败信息会在WebStartCb回调中返回）。该公用浏览器的引用计数也不会递增。

			用户也可以启动私有的浏览器（WebContainerInfo::bPrivate==true），私有浏览器不会供其他用户使用，
			同样的，若用户想启动某内核的私有浏览器，需要先释放已经引用的该内核的公有浏览器（参见第二条，每
			个用户同种内核的浏览器只能拥有一个引用）。
			
		该接口*线程安全*
 */
WEBCONTAINERAPI_API int WebContainerAPI_Start(const WebContainerInfo* pWebInfo);

/** @brief 停止浏览器;
 *  @param WebEngineType [in] 浏览器类型;
 *  @return 成功返回0，失败返回其他值;
 *	@note 该接口只是递减浏览器的引用计数，浏览器是否被停止取决于其引用计数是否为0
		该接口*线程安全*
 */
WEBCONTAINERAPI_API int WebContainerAPI_Stop(WebEngineType iEngineType);

/** @brief 载入url;
 *  @param WebEngineType [in] 浏览器类型;
 *  @param const char* [in] url;
 *  @param void* [in] 窗口;
 *  @return 成功返回0，失败返回其他值;
 *	@note 载入url
		该接口*线程安全*
 */
WEBCONTAINERAPI_API int WebContainerAPI_LoadUrl(WebEngineType type,const char* url, void* hWnd);

/** @brief 调用js方法;
 *  @param WebEngineType [in] 浏览器类型;
 *  @param void* [in] 窗口;
 *  @param const char* [in] 方法名;
 *  @param const char* [in] 数据;目前限定一个字符串参数，建议传json
 *  @param unsigned int [in] 数据长度;
 *  @return 成功返回0，失败返回其他值;
 *	@note 
		该接口*线程安全*
 */
WEBCONTAINERAPI_API int WebContainerAPI_CallJs(WebEngineType type, void* hWnd, const char* method,const char* data,unsigned int size);

/** @brief 反初始化WebContainerAPI;
 *  @return 成功返回0，失败返回其他值;
 *	@note 
		该接口*非线程安全*
 */
WEBCONTAINERAPI_API int WebContainerAPI_Uninit(void);