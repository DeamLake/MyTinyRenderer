#include "WinApp.h"
#include "Rasterizer.h"
#include "model.h"
#include "Shaders.h"
#include "Camera.h"
#include "Scene.h"

const scene_t Scenes[]
{
	{"african_head",African_Head_Scene},
	{"fuhua",Fuhua_Scene},
	{"helmet",Helmat_Scene},
	{"gun",Gun_Scene}
};

const vec3 Eye(0, 2, 4);
const vec3 Up(0, 1, 0);
const vec3 Target(0, 0, 0);

int main()
{
	new WinApp(1600, 600, "SoftRenderer");
	int width = WinApp::GetApp()->GetWidth(), height = WinApp::GetApp()->GetHeight();
	Camera* camera = new Camera(Eye, Target, Up, (float)(width) / height);
	Rasterizer r(width, height);
	r.SetHDC(WinApp::GetApp()->GetHDC());

	// »·¾³ÅäÖÃ
	EnvData* envData = new EnvData();
	envData->camera = camera;
	envData->LightColor = glm::vec3(255, 255, 255);
	envData->LightPos = glm::vec3(1, 1, 1);
	envData->zNear = -0.5f;
	envData->zFar = -10.0f;
	envData->eye_fov = 45;
	r.SetUpEnvironment(envData);

	Scenes[3].build_scene(r);

	float angle = 120.0f;
	while (!WinApp::GetApp()->isClose)
	{
		// Clear
		r.ClearAll();

		// camera
		camera->handle_events();

		angle += 1.0f;

		for (int i = 0; i < r.getSizeOfObject(); i++) {
			ModelData data = r.getNthObject(i);
			if (data.rotate) { r.rotate_object(angle, data); }
			r.draw_model(data.model, data.shader, camera);
		}

		// reset mouse information
		WinApp::GetApp()->GetMouse()->wheel_delta = 0;
		WinApp::GetApp()->GetMouse()->orbit_delta = vec2(0, 0);
		WinApp::GetApp()->GetMouse()->fv_delta = vec2(0, 0);

		WinApp::GetApp()->Show(r.GetBuffer());
		WinApp::GetApp()->msg_dispatch();
	}

	free(WinApp::GetApp());
	system("pause");
}