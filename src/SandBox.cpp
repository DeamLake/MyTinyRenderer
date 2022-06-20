/// <summary>
/// 沙盒程序
/// </summary>
/// #include <direct.h>
#include "WinApp.h"
#include "Rasterizer.h"
#include "model.h"
#include "Shaders.h"


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
	void OnKeyboardInput();
	void Clear();

protected:
	Rasterizer r;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	WinApp* app = new SandBoxApp(hInstance, 1600, 800, "Peach Renderer");
	
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
	// 设置HDC
	r.SetHDC(gScreenHdc);

	// 环境配置
	EnvData* envData = new EnvData();
	envData->view_point = glm::vec3(0, 2, 4);
	envData->center = glm::vec3(0, 0, 0);
	envData->up = glm::vec3(0, 1, 0);
	envData->LightColor = glm::vec3(255, 255, 255);
	envData->LightPos = glm::vec3(1, 1, 3);
	envData->zNear = -0.1f;
	envData->zFar = -10.0f;
	envData->eye_fov = 45;
	r.SetUpEnvironment(envData);


	// shader 测试场景
	ModelData modelData;
	modelData.model = new Model("../Resources/african_head/african_head.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(-1.5f, 0, 0);
	modelData.scales = glm::vec3(1, 1, 1);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/african_head/african_head_eye_inner.obj");
	modelData.shader = new GouraudShader();
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/diablo/diablo3_pose.obj");
	modelData.shader = new DepthShader();
	modelData.translate = glm::vec3(1.5f, 0, 0);
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/floor.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0.18f, -1.2f);
	modelData.scales = glm::vec3(4, 1, 2);
	modelData.rotate = false;
	r.Add_Object(modelData);

	return true;
}

int SandBoxApp::Run() 
{
	MSG msg = { 0 };

	float angle = 0;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Clear();
			
			angle +=1.0f;

			for (int i = 0; i < r.getSizeOfObject(); i++) {
				ModelData data = r.getNthObject(i);
				if(data.rotate){ r.rotate_object(angle, data); }
				r.draw_model(data.model, data.shader);
			}

			Show();
			ShowFPS();
		}
	}

	return (int)msg.wParam;
}

LRESULT SandBoxApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE){
			PostQuitMessage(0);
		}
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void SandBoxApp::Clear()
{
	BitBlt(gScreenHdc, 0, 0, gClientWidth, gClientHeight, NULL, NULL, NULL, BLACKNESS);
	r.ClearDepth();
}