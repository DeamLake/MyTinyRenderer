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

void Helmat_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/helmet/helmet.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0, -1);
	modelData.scales = glm::vec3(1.0f, 1.0f, 1.0f);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);
}



void Fuhua_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/fuhua/fuhuabody.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0, 0);
	modelData.scales = glm::vec3(1, 1, 1);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/fuhua/fuhuacloak.obj");
	modelData.shader = new GouraudShader();
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/fuhua/fuhuaface.obj");
	modelData.shader = new GouraudShader();
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/fuhua/fuhuahair.obj");
	modelData.shader = new GouraudShader();
	r.Add_Object(modelData);
}

void Floor_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/floor.obj");
	modelData.shader = new BlingPhongShader();
	modelData.translate = glm::vec3(0, 0, -1);
	modelData.scales = glm::vec3(2.0f, 2.0f, 2.0f);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);
}