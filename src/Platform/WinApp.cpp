#include "WinApp.h"
#include <limits>

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return WinApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

WinApp* WinApp::gApp = nullptr;

WinApp::WinApp(HINSTANCE hi, int w, int h, const char* wn)
	:ghAppInst(hi), gClientWidth(w), gClientHeight(h), gWindowName(wn)
{
	assert(gApp == nullptr);
	gApp = this;
}

int WinApp::Run()
{
	assert(0, "Cant Run raw app");
	return 0;
}


bool WinApp::Initialize()
{
	if (!InitMainWindow())
		return false;

	return true;
}

LRESULT WinApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool WinApp::InitMainWindow()
{
	// 窗口类
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = ghAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "MainWnd";

	// 注册窗口类
	if (!RegisterClass(&wc))
	{
		MessageBox(0, "RegisterClass Failed.", 0, 0);
		return false;
	}

	// 设置窗口大小
	RECT R = { 0, 0, gClientWidth, gClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	// 创建窗口
	ghMainWnd = CreateWindow("MainWnd", gWindowName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, ghAppInst, 0);
	if (!ghMainWnd)
	{
		MessageBox(0, "CreateWindow Failed.", 0, 0);
		return false;
	}

	// 创建设备
	gHdc = GetDC((ghMainWnd));
	gScreenHdc = CreateCompatibleDC(gHdc);
	//ReleaseDC(ghMainWnd, gHdc);

	// 创建图像位图
	BITMAPINFO bitmapInfo = { { sizeof(BITMAPINFOHEADER),width, height, 1, 32, BI_RGB, width * height * 4, 0, 0, 0, 0 } };
	LPVOID ptr;
	HBITMAP bitmapHandler = CreateDIBSection(gScreenHdc, &bitmapInfo, DIB_RGB_COLORS, &ptr, 0, 0);
	if (!bitmapHandler)
	{
		MessageBox(0, "Createbitmap Failed.", 0, 0);
		return false;
	}

	// 选择图像位图
	SelectObject(gScreenHdc, bitmapHandler);

	// 设置图像到屏幕中央
	int sx = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int sy = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(ghMainWnd, NULL, sx, sy, width, height, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));

	ShowWindow(ghMainWnd, SW_SHOW);
	UpdateWindow(ghMainWnd);

	return true;
}

void WinApp::Show() 
{
	BitBlt(gHdc, 0, 0, gClientWidth, gClientHeight, gScreenHdc, 0, 0, SRCCOPY);
}

void WinApp::ShowFPS()
{
	static float  fps = 0;
	static int    frameCount = 0;
	static float  currentTime = 0.0f;
	static float  lastTime = 0.0f;

	frameCount++;
	currentTime = timeGetTime() * 0.001f;

	if (currentTime - lastTime > 1.0f)
	{
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}

	char strBuffer[20];
	sprintf_s(strBuffer, 20, "%0.4f", fps);
	TextOut(gHdc, 0, 0, strBuffer, 6);
}