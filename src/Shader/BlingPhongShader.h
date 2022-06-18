#pragma once
#include "IShader.h"

class BlingPhongShader : public IShader
{
public:
	BlingPhongShader(): IShader() {}

	vec4 vertex(int iface, int nthvert) final {
		// ��ȡ����
		vec4 position(model->vert(iface, nthvert), 1);
		// ת������������
		position = position * World_mat;
		VertPosition[nthvert] = position;

		// ��ȡ����UV
		VertUV[nthvert] = model->uv(iface, nthvert);

		// ����NDC����
		return position * ViewProj_mat;
	}

	bool fragment(const vec3& bary, vec3& color) final {
		vec3 position = VertPosition * bary;
		vec2 uv = VertUV * bary;
		vec3 normal = model->normal(uv);

		// ambient
		float ambientStrength = 0.05f;
		vec3 ambient = ambientStrength * pLightColor;

		// diffuse 
		normal = normalize(normal);
		vec3 lightDir = normalize(pLightPos - position);
		float diff = max(dot(normal, lightDir), 0.0f);
		vec3 diffuse = diff * pLightColor;

		// specular
		float specularStrength = 0.1;
		vec3 viewDir = normalize(pViewPos - position);
		vec3 midDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, midDir), 0.0f), 32);
		vec3 specular = specularStrength * spec * pLightColor;

		color = ambient + diffuse + specular;

		// ���Բ�ͨ��
		return false;
	}
};