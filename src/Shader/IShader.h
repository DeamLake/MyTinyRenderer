#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include "model.h"
#include "Camera.h"
using namespace glm;

struct payload_t
{
	Model* model = nullptr;
	iblmap_t* iblMap = nullptr;
	Camera* camera = nullptr;
	mat4x4 World_mat, ViewProj_mat;
	vec3 pLightPos, pLightColor, pViewPos;
	mat3x3 VertPosition, VertNormal;
	mat4x4 ClipPosition;
	mat3x2 VertUV;

	//for homogeneous clipping
	vec3 inVertPosition[10], inVertNormal[10];
	vec4 inClipPosition[10];
	vec2 inVertUV[10];
	vec3 outVertPosition[10], outVertNormal[10];
	vec4 outClipPosition[10];
	vec2 outVertUV[10];
};

class IShader {
public:
	IShader() {}
	virtual ~IShader() {}
	virtual void vertex(int iface, int nthvert) = 0;
	virtual bool fragment(const vec3& bary, vec3& color) = 0;
public:
	payload_t* payload;
};

vec3 cubemap_sampling(vec3 direction, cubemap_t* cubemap);