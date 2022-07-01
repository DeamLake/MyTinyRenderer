#pragma once
#include "IShader.h"

class PBRShader : public IShader
{
public:
	PBRShader() : IShader() {}

	void vertex(int iface, int nthvert) final {
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

	bool fragment(const vec3& bary, vec3& color) final {
		vec2 uv = payload->VertUV * bary;
		vec3 position = payload->VertPosition * bary;
		vec3 normal = normalize(payload->VertNormal * bary);
		vec3 lightDir = normalize(payload->pLightPos - position);
		vec3 viewDir = normalize(payload->pViewPos - position);

		// tbn 转换 normal mapping
		mat3x3 AI = inverse(mat3x3{ payload->VertPosition[1] - payload->VertPosition[0], payload->VertPosition[2] - payload->VertPosition[0], normal });
		vec3 i = vec3(payload->VertUV[1][0] - payload->VertUV[0][0], payload->VertUV[2][0] - payload->VertUV[0][0], 0) * AI;
		vec3 j = vec3(payload->VertUV[1][1] - payload->VertUV[0][1], payload->VertUV[2][1] - payload->VertUV[0][1], 0) * AI;
		mat3x3 TBN = transpose(mat3x3{ normalize(i), normalize(j), normal });
		normal = normalize(payload->model->normal(uv) * TBN);

		vec3 midDir = normalize(lightDir + viewDir);
		float spec = pow(max(0.0f, dot(midDir, normal)), 200);
		float intense = max(0.0f, dot(normal, lightDir));

		color = (intense + 0.6f * spec) * payload->model->diffuse(uv);

		//测试不通过
		return false;
	}
};