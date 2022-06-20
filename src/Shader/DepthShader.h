#pragma once
#include "IShader.h"

class DepthShader : public IShader
{
public:
	DepthShader() : IShader() {}

	vec4 vertex(int iface, int nthvert) final {
		// 获取坐标
		vec4 position(model->vert(iface, nthvert), 1);
		// 转换到世界坐标
		position = position * World_mat;
		VertPosition[nthvert] = position;
		
		// 获取纹理UV
		VertUV[nthvert] = model->uv(iface, nthvert);

		// 返回NDC坐标
		return position * ViewProj_mat;
	}

	bool fragment(const vec3& bary, vec3& color) final {
		vec4 position = vec4(VertPosition * bary, 1.0f);
		float depth = (position * ViewProj_mat).w;
		float zNear = -0.1f, zFar = -10.0f;
		float zDepth = 1 - (depth - zNear) / (zFar - zNear);
		color = vec3(255, 255, 255) * zDepth;
		// 测试不通过
		return false;
	}
};