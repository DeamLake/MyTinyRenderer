#pragma once
#include "IShader.h"

class GouraudShader : public IShader
{
public:
	GouraudShader() : IShader() {}

	void vertex(int iface, int nthvert) final {
		// 获取坐标
		vec4 position(payload->model->vert(iface,nthvert), 1);
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

	bool fragment(const vec3& bary, vec3& color) final {
		vec2 uv = payload->VertUV * bary;
		vec3 position = payload->VertPosition * bary;
		vec3 normal = payload->VertNormal * bary;
		if (payload->model->normalmap) { normal = tbn_normal(payload, bary, uv); }//有法线贴图时启用
		vec3 lightDir = normalize(payload->pLightPos - position);
		vec3 viewDir = normalize(payload->pViewPos - position);

		vec3 midDir = normalize(lightDir + viewDir);
		float spec = pow(max(0.0f, dot(midDir, normal)), 200 );
		float intense = max(0.0f, dot(normal, lightDir));

		color = (intense+0.6f * spec) * payload->model->diffuse(uv);

		//测试不通过
		return false;
	}
};