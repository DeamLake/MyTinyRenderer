#pragma once
#include "IShader.h"

class BlingPhongShader : public IShader
{
public:
	BlingPhongShader(): IShader() {}

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
		vec3 position = payload->VertPosition * bary;
		vec2 uv = payload->VertUV * bary;
		vec3 normal = payload->model->normal(uv);

		// ambient
		float ambientStrength = 0.05f;
		vec3 ambient = ambientStrength * payload->pLightColor;

		// diffuse 
		normal = normalize(normal);
		vec3 lightDir = normalize(payload->pLightPos - position);
		float diff = max(dot(normal, lightDir), 0.0f);
		vec3 diffuse = diff * payload->pLightColor;

		// specular
		float specularStrength = 0.1;
		vec3 viewDir = normalize(payload->pViewPos - position);
		vec3 midDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, midDir), 0.0f), 32);
		vec3 specular = specularStrength * spec * payload->pLightColor;

		color = ambient + diffuse + specular;

		// 测试不通过
		return false;
	}
};