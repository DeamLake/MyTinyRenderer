#pragma once
#include "IShader.h"

class SkyBoxShader : public IShader
{
public:
	SkyBoxShader() : IShader() {}

	void vertex(int iface, int nthvert) final 
	{
		// 获取坐标
		vec4 position(payload->model->vert(iface, nthvert), 1);
		// 转换到世界坐标
		position = position * payload->World_mat;
		payload->VertPosition[nthvert] = position;
		payload->VertNormal[nthvert] = vec4(payload->model->normal(iface, nthvert), 1) * transpose(inverse(payload->World_mat));
		payload->VertUV[nthvert] = payload->model->uv(iface, nthvert);
		position = position * payload->ViewProj_mat;

		payload->inClipPosition[nthvert] = position;
		payload->inVertPosition[nthvert] = payload->VertPosition[nthvert];
		payload->inVertNormal[nthvert] = payload->VertNormal[nthvert];
		payload->inVertUV[nthvert] = payload->VertUV[nthvert];
	}

	bool fragment(const vec3& bary, vec3& color) final 
	{
		vec3 position = payload->VertPosition * bary;
		color = cubemap_sampling(position, payload->model->environment_map);

		//测试不通过
		return false;
	}
};