// BrowserSubProcess.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "include/cef_app.h"
#include "RendererApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	// Structure for passing command-line arguments.
	// The definition of this structure is platform-specific.
	// 传递命令行参数的结构体。
	// 这个结构体的定义与平台相关。
	CefMainArgs main_args(hInstance);

	// Optional implementation of the CefApp interface.
	// 可选择性地实现CefApp接口
	CefRefPtr<RendererApp> app(new RendererApp);

	// Execute the sub-process logic. This will block until the sub-process should exit.
	// 执行子进程逻辑，此时会堵塞直到子进程退出。
	return CefExecuteProcess(main_args, app,0);
}