#include "WinApp.h"
#include "Rasterizer.h"
#include "model.h"
#include "Shaders.h"

void main()
{
	new WinApp(1600, 600, "SoftRenderer");
	int width = WinApp::GetApp()->GetWidth(), height = WinApp::GetApp()->GetHeight();
	Rasterizer r(width, height);
	r.SetHDC(WinApp::GetApp()->GetHDC());

	// »·¾³ÅäÖÃ
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

	// shader ²âÊÔ³¡¾°
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

	/*modelData.model = new Model("../Resources/floor.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0.18f, -1.2f);
	modelData.scales = glm::vec3(4, 1, 2);
	modelData.rotate = false;
	r.Add_Object(modelData);*/

	float angle = 0;
	while (!WinApp::GetApp()->isClose)
	{
		// Clear
		BitBlt(WinApp::GetApp()->GetHDC(), 0, 0, width, height, NULL, NULL, NULL, BLACKNESS);
		r.ClearDepth();

		angle += 1.0f;

		for (int i = 0; i < r.getSizeOfObject(); i++) {
			ModelData data = r.getNthObject(i);
			if (data.rotate) { r.rotate_object(angle, data); }
			r.draw_model(data.model, data.shader);
		}

		// reset mouse information
		WinApp::GetApp()->GetMouse()->wheel_delta = 0;
		WinApp::GetApp()->GetMouse()->orbit_delta = vec2(0, 0);
		WinApp::GetApp()->GetMouse()->fv_delta = vec2(0, 0);

		WinApp::GetApp()->Show();
		WinApp::GetApp()->msg_dispatch();
	}

	free(WinApp::GetApp());
	system("pause");
}