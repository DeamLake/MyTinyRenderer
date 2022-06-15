#pragma once
#include <Windows.h>
#include <assert.h>

#pragma comment( lib,"winmm.lib" )

class WinApp
{
protected:
	// winapp不能被构造
	WinApp(HINSTANCE hi, int w, int h, const char* wn);
	virtual ~WinApp() {}
	WinApp(const WinApp& rhs) = delete;
	WinApp&  operator=(const WinApp& rhs) = delete;

public:
	static WinApp* GetApp() { return gApp; }
	HINSTANCE AppInst()const { return ghAppInst; }
	HWND      MainWnd()const { return ghMainWnd; }
	float     AspectRatio()const { return static_cast<float>(gClientWidth) / gClientHeight; }

	virtual int Run();
	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	// 内部函数
	bool InitMainWindow();
	void Show();
	void ShowFPS();

protected:
	// 内部变量
	static WinApp* gApp;// app类实例

	HINSTANCE				ghAppInst			= nullptr;		// 应用程序handle
	HWND						ghMainWnd		= nullptr;		// 窗口handle
	HDC							gHdc					= nullptr;     // 主屏幕设备
	HDC							gScreenHdc			= nullptr;     // 副屏幕设备 着色完之后复制到主设备 完成渲染

	int gClientWidth, gClientHeight;
	const char* gWindowName;
};