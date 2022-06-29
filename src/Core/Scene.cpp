#include "Scene.h"

void African_Head_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/african_head/african_head.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0, 0);
	modelData.scales = glm::vec3(1, 1, 1);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/african_head/african_head_eye_inner.obj");
	modelData.shader = new GouraudShader();
	r.Add_Object(modelData);
}

void Diablo_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/diablo/diablo3_pose.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0, 0);
	modelData.scales = glm::vec3(1, 1, 1);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);
}