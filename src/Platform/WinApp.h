#pragma once
#include <Windows.h>
#include <assert.h>

#pragma comment( lib,"winmm.lib" )

class WinApp
{
protected:
	// winapp���ܱ�����
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
	// �ڲ�����
	bool InitMainWindow();
	void Show();
	void ShowFPS();

protected:
	// �ڲ�����
	static WinApp* gApp;// app��ʵ��

	HINSTANCE				ghAppInst			= nullptr;		// Ӧ�ó���handle
	HWND						ghMainWnd		= nullptr;		// ����handle
	HDC							gHdc					= nullptr;     // ����Ļ�豸
	HDC							gScreenHdc			= nullptr;     // ����Ļ�豸 ��ɫ��֮���Ƶ����豸 �����Ⱦ

	int gClientWidth, gClientHeight;
	const char* gWindowName;
};