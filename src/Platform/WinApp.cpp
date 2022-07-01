#include "WinApp.h"
#include <limits>

glm::vec2 get_mouse_pos()
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(WinApp::GetApp()->MainWnd(), &point); // ����Ļ�ռ�ת�����ڿռ�
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
	// ������
	WNDCLASS wc;
	wc.style = CS_BYTEALIGNCLIENT;							//���ڷ��
	wc.lpfnWndProc = (WNDPROC)msg_callback;					//�ص�����
	wc.cbClsExtra = 0;										//�����ڴ�����β����һ�����ռ䣬��������Ϊ0
	wc.cbWndExtra = 0;										//�����ڴ���ʵ��β����һ�����ռ䣬��������Ϊ0
	wc.hInstance = GetModuleHandle(NULL);					//��ǰʵ�����
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);				//������ͼ��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				//�����ʽ
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//������ʽ
	wc.lpszMenuName = NULL;									//�˵�
	wc.lpszClassName = "SoftRenderer";					//�ô����������

	// ע�ᴰ����
	if (!RegisterClass(&wc))
	{
		MessageBox(0, "RegisterClass Failed.", 0, 0);
		return;
	}

	// ��������
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

	// ����������DC
	HDC hdc = GetDC((ghMainWnd));
	gScreenHdc = CreateCompatibleDC(hdc);
	ReleaseDC(ghMainWnd, hdc);

	// ����ͼ��λͼ
	BITMAPINFO bitmapInfo = { sizeof(BITMAPINFOHEADER),width, -height, 1, 32, BI_RGB, width * height * 4, 0, 0, 0, 0 };
	LPVOID ptr;
	HBITMAP bitmapHandler = CreateDIBSection(gScreenHdc, &bitmapInfo, DIB_RGB_COLORS, &ptr, 0, 0);
	if (!bitmapHandler)
	{
		MessageBox(0, "Createbitmap Failed.", 0, 0);
		return;
	}

	// ѡ��ͼ��λͼ
	SelectObject(gScreenHdc, bitmapHandler);
	this->window_fb = (unsigned char*)ptr;

	// ���ô��ڴ�С
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

	// ����ͼ����Ļ����
	int sx = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int sy = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(ghMainWnd, NULL, sx, sy, width, height, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));

	ShowWindow(ghMainWnd, SW_SHOW);
	UpdateWindow(ghMainWnd);

	//��Ϣ����
	msg_dispatch();

	//��ʼ��keys
	memset(window_fb, 0, width * height * 4);
	memset(keys, 0, sizeof(char) * 512);
}

void WinApp::msg_dispatch()
{
	MSG msg;
	while (1)
	{
		// Peek��������Get��������PM_NOREMOVE��ʾ�������Ϣ������������������Get����
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break; //û��Ϣ���ȷ������Ϣ����Get
		if (!GetMessage(&msg, NULL, 0, 0)) break;

		TranslateMessage(&msg);	 //ת����Ϣ ���ⰴ��->�ַ�
		DispatchMessage(&msg); //������Ϣ���ص�
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