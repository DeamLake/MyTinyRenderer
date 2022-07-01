#pragma once
#include<string>
#include<glm/vec3.hpp>
#include "IShader.h"
#include "model.h"
#include "Camera.h"



struct EnvData
{
	Camera* camera;
	glm::vec3 LightPos, LightColor;
	float zNear, zFar;
	int eye_fov;
};

struct ModelData
{
	glm::vec3 translate, scales;
	float yangle;
	IShader* shader;
	Model* model;
	bool rotate = true;
};