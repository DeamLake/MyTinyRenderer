#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "model.h"
using namespace glm;

class IShader {
public:
	IShader() :VertPosition(3), VertNormal(3), VertColor(3) {}
	virtual ~IShader() {}
	void set_model_data(Model* model_data) { model = model_data; }
	vec3 mvp_translate(vec3& p, mat4x4& mat);
	virtual vec3 vertex(int iface, int nthvert) = 0;
	virtual vec3 fragment(const vec3& bary, vec3 zs) = 0;
public:
	Model* model;
	mat4x4 World_mat, ViewProj_mat;
	vec3 LightPos, LightColor, ViewPos;

protected:
	std::vector<vec3> VertPosition;
	std::vector<vec3> VertNormal;
	std::vector<vec3> VertColor;
	
};