#include "WinApp.h"
#include <limits>

glm::vec2 get_mouse_pos()
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(WinApp::GetApp()->MainWnd(), &point); // 从屏幕空间转到窗口空间
	return glm::vec2((float)point.x, (float)point.y);
}

static LRESULT CALLBACK msg_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		WinApp::GetApp()->isClose = true;
		break;
	case WM_KEYDOWN:
		WinApp::GetApp()->keys[wParam & 511] = 1;
		break;
	case WM_KEYUP:
		WinApp::GetApp()->keys[wParam & 511] = 0;
		break;
	case WM_LBUTTONDOWN:
		WinApp::GetApp()->GetMouse()->orbit_pos = get_mouse_pos();
		WinApp::GetApp()->buttons[0] = 1;
		break;
	case WM_LBUTTONUP:
		WinApp::GetApp()->buttons[0] = 0;
		break;
	case WM_RBUTTONDOWN:
		WinApp::GetApp()->GetMouse()->fv_pos = get_mouse_pos();
		WinApp::GetApp()->buttons[1] = 1;
		break;
	case WM_RBUTTONUP:
		WinApp::GetApp()->buttons[1] = 0;
		break;
	case WM_MOUSEWHEEL:
		WinApp::GetApp()->GetMouse()->wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		break;

	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

WinApp* WinApp::gApp = nullptr;

WinApp::WinApp(int w, int h, const char* wn)
	:gMouse(new mouse_t()), gClientWidth(w), gClientHeight(h), gWindowName(wn)
{
	assert(gApp == nullptr);
	gApp = this;
	Initialize();
}

WinApp::~WinApp()
{
	if (gScreenHdc) {
		DeleteDC(gScreenHdc);
		gScreenHdc = NULL;
	}
	if (ghMainWnd) {
		CloseWindow(ghMainWnd);
		ghMainWnd = NULL;
	}
}

void WinApp::Initialize()
{
	// 窗口类
	WNDCLASS wc;
	wc.style = CS_BYTEALIGNCLIENT;							//窗口风格
	wc.lpfnWndProc = (WNDPROC)msg_callback;					//回调函数
	wc.cbClsExtra = 0;										//紧跟在窗口类尾部的一块额外空间，不用则设为0
	wc.cbWndExtra = 0;										//紧跟在窗口实例尾部的一块额外空间，不用则设为0
	wc.hInstance = GetModuleHandle(NULL);					//当前实例句柄
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);				//任务栏图标
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				//光标样式
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//背景样式
	wc.lpszMenuName = NULL;									//菜单
	wc.lpszClassName = "SoftRenderer";					//该窗口类的名字

	// 注册窗口类
	if (!RegisterClass(&wc))
	{
		MessageBox(0, "RegisterClass Failed.", 0, 0);
		return;
	}

	// 创建窗口
	ghMainWnd = CreateWindow(wc.lpszClassName, gWindowName,
		WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, 0, 0, GetModuleHandle(NULL), 0);
	if (!ghMainWnd)
	{
		MessageBox(0, "CreateWindow Failed.", 0, 0);
		return;
	}

	RECT R = { 0, 0, gClientWidth, gClientHeight };
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	// 创建兼容性DC
	HDC hdc = GetDC((ghMainWnd));
	gScreenHdc = CreateCompatibleDC(hdc);
	ReleaseDC(ghMainWnd, hdc);

	// 创建图像位图
	BITMAPINFO bitmapInfo = { sizeof(BITMAPINFOHEADER),width, -height, 1, 32, BI_RGB, width * height * 4, 0, 0, 0, 0 };
	LPVOID ptr;
	HBITMAP bitmapHandler = CreateDIBSection(gScreenHdc, &bitmapInfo, DIB_RGB_COLORS, &ptr, 0, 0);
	if (!bitmapHandler)
	{
		MessageBox(0, "Createbitmap Failed.", 0, 0);
		return;
	}

	// 选择图像位图
	SelectObject(gScreenHdc, bitmapHandler);
	this->window_fb = (unsigned char*)ptr;

	// 设置窗口大小
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

	// 设置图像到屏幕中央
	int sx = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int sy = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(ghMainWnd, NULL, sx, sy, width, height, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));

	ShowWindow(ghMainWnd, SW_SHOW);
	UpdateWindow(ghMainWnd);

	//消息分派
	msg_dispatch();

	//初始化keys
	memset(window_fb, 0, width * height * 4);
	memset(keys, 0, sizeof(char) * 512);
}

void WinApp::msg_dispatch()
{
	MSG msg;
	while (1)
	{
		// Peek不阻塞，Get会阻塞，PM_NOREMOVE表示如果有消息不处理（留给接下来的Get处理）
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break; //没消息就溜，确定有消息再用Get
		if (!GetMessage(&msg, NULL, 0, 0)) break;

		TranslateMessage(&msg);	 //转换消息 虚拟按键->字符
		DispatchMessage(&msg); //传送消息给回调
	}
}

void WinApp::Show(unsigned char* framebuffer)
{
	HDC hDC = GetDC(ghMainWnd);
	for (int i = 0; i < gClientHeight; i++)
	{
		for (int j = 0; j < gClientWidth; j++) 
		{
			int index = (i * gClientWidth + j) * 4;
			window_fb[index] = framebuffer[index + 2];
			window_fb[index + 1] = framebuffer[index + 1];
			window_fb[index + 2] = framebuffer[index + 0];
		}
	}

	BitBlt(hDC, 0, 0, gClientWidth, gClientHeight, gScreenHdc, 0, 0, SRCCOPY);

	// show FPS
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
	TextOut(hDC, 0, 0, strBuffer, 6);

	ReleaseDC(ghMainWnd, hDC);
}