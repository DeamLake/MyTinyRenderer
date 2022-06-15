/// <summary>
/// 沙盒程序
/// </summary>
#include "WinApp.h"
#include "Rasterizer.h"
#include "Config.h"
#include "model.h"
#include "BlingPhongShader.h"
#include <direct.h>

class SandBoxApp :public WinApp 
{
public:
	SandBoxApp(HINSTANCE hi, int w, int h, const char* wn) :WinApp(hi, w, h, wn), r(w, h) {}
	~SandBoxApp() {}

protected:
	virtual bool Initialize()override;
	bool InitializeRasterizer();
	virtual int Run()override;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
	void Clear();

protected:
	Rasterizer r;
	IShader* shader = nullptr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	WinApp* app = new SandBoxApp(hInstance, WindowWidth, WindowHeight, WinName);
	
	if (!app->Initialize())
		return 0;

	return app->Run();
}


bool SandBoxApp::Initialize() 
{
	if (!WinApp::Initialize())
		return false;

	if (!InitializeRasterizer())
		return false;

	return true;
}

// 初始化Rasterizer
bool SandBoxApp::InitializeRasterizer() 
{
	// 配置
	glm::vec3 view_point(0, 0, 4);
	const int eye_fov = 45;
	const float zNear = -0.1f;
	const float zFar = -500.0f;

	// 设置HDC
	r.SetHDC(gScreenHdc);

	//三个矩阵

	r.set_model(0, 1.0f);
	r.set_view(view_point);
	r.set_projection(zNear, zFar, eye_fov);
	Model* model = new Model("../Resources/african_head/african_head.obj");
	shader = new BlingPhongShader();
	shader->set_model_data(model);
	shader->World_mat = r.Model_mat;
	shader->ViewProj_mat = r.Projection_mat * r.View_mat;
	shader->LightPos = glm::vec3(0, 1, 3);
	shader->LightColor = glm::vec3(255, 255, 255);
	shader->ViewPos = view_point;

	return true;
}

int SandBoxApp::Run() 
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			Clear();
			r.draw_model(shader->model, shader);
			Show();
			ShowFPS();
			//mTimer.Tick();

			/*if (!mAppPaused)
			{
				CalculateFrameStats();
				Update(mTimer);
				Draw(mTimer);
			}
			else
			{
				Sleep(100);
			}*/
		}
	}

	return (int)msg.wParam;
}

LRESULT SandBoxApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void SandBoxApp::Clear()
{
	BitBlt(gScreenHdc, 0, 0, gClientWidth, gClientHeight, NULL, NULL, NULL, BLACKNESS);
	r.ClearDepth();
}