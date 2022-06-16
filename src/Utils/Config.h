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
	glm::vec3 translate;
	float yangle, scale;
	IShader* shader;
	Model* model;
};