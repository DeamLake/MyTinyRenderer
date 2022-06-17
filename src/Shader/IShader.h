#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include "model.h"
using namespace glm;

class IShader {
public:
	IShader() :VertPosition(3), VertNormal(3), VertUV(3), model(nullptr), World_mat(), ViewProj_mat() {}
	virtual ~IShader() {}
	void set_model_data(Model* model_data) { model = model_data; }	
	vec3 calculateT(int idx0, int idx1, int idx2);
	virtual vec4 vertex(int iface, int nthvert) = 0;
	virtual vec3 fragment(const vec3& bary, vec3 zs) = 0;
public:
	Model* model = nullptr;
	mat4x4 World_mat, ViewProj_mat;
	vec3 pLightPos, pLightColor, pViewPos;

protected:
	std::vector<vec3> VertPosition, VertNormal;
	std::vector<vec2> VertUV;
	
};
