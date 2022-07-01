#pragma once
#include <Windows.h>
#include <assert.h>
#include "glm/vec2.hpp"
#include "micros.h"
#pragma comment( lib,"winmm.lib" )

typedef struct mouse
{
	// for camera orbit
	glm::vec2 orbit_pos;
	glm::vec2 orbit_delta;
	// for first-person view (diabled now)
	glm::vec2 fv_pos;
	glm::vec2 fv_delta;
	// for mouse wheel
	float wheel_delta;
}mouse_t;

class WinApp
{
public:
	WinApp(int w, int h, const char* wn);
	~WinApp();
	WinApp(const WinApp& rhs) = delete;
	WinApp&  operator=(const WinApp& rhs) = delete;

public:
	bool isClose = false;
	char keys[512];
	char buttons[2];

	static WinApp* GetApp() { return gApp; }
	mouse_t* GetMouse() { return gMouse; }
	int GetWidth() { return gClientWidth; }
	int GetHeight() { return gClientHeight; }
	HWND MainWnd()const { return ghMainWnd; }
	HDC GetHDC() { return gScreenHdc; }

	void msg_dispatch();
	void Show(unsigned char* framebuffer);

protected:
	// �ڲ�����
	void Initialize();

protected:
	// �ڲ�����
	static WinApp* gApp;// app��ʵ��
	mouse_t* gMouse;
	unsigned char* window_fb;
	HWND		ghMainWnd  = nullptr;		// ����handle
	HDC			gScreenHdc	  = nullptr;     // ����Ļ�豸 ��ɫ��֮���Ƶ����豸 �����Ⱦ
	int gClientWidth, gClientHeight;
	const char* gWindowName;
};

glm::vec2 get_mouse_pos();