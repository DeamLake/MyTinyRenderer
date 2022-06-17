#pragma once
#include<string>
#include<glm/vec3.hpp>
#include "IShader.h"
#include "model.h"

struct EnvData
{
	glm::vec3 view_point, LightPos, LightColor;
	float zNear, zFar;
	int eye_fov;
};

struct ModelData
{
	glm::vec3 translate, scales;
	float yangle;
	IShader* shader;
	Model* model;
};