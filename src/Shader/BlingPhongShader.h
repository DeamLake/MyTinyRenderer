#pragma once
#include "IShader.h"

class BlingPhongShader : public IShader
{
public:
	BlingPhongShader(): IShader() {}

	vec4 vertex(int iface, int nthvert) final {
		// ��ȡ����
		vec4 position(model->vert(model->face(iface)[nthvert]), 1);
		// ת������������
		position = World_mat * position;
		VertPosition[nthvert] = position / position.w;

		// ��ȡ������
		VertNormal[nthvert] = model->norm(iface, nthvert);
		// ������ת��
		VertNormal[nthvert] = mat3(transpose(inverse(World_mat))) * VertNormal[nthvert];

		// ��ȡ����UV
		VertUV[nthvert] = model->uv(iface, nthvert);

		// ����NDC����
		return ViewProj_mat * position;
	}

	vec3 fragment(const vec3& bary, vec3 zs) final {
		float alpha = bary.x, beta = bary.y, gamma = bary.z;
		float zp = 1.0f / (alpha / zs[0] + beta / zs[1] + gamma / zs[2]);
		vec3 position = (alpha * VertPosition[0] / zs[0] + beta * VertPosition[1] / zs[1] + gamma * VertPosition[2] / zs[2]) * zp;
		vec3 normal = (alpha * VertNormal[0] / zs[0] + beta * VertNormal[1] / zs[1] + gamma * VertNormal[2] / zs[2]) * zp;

		vec3 lightColor = *pLightColor;
		vec3 lightPos = *pLightPos;
		vec3 viewPos = *pViewPos;

		// ambient
		float ambientStrength = 0.05f;
		vec3 ambient = ambientStrength * lightColor;

		// diffuse 
		normal = normalize(normal);
		vec3 lightDir = normalize(lightPos - position);
		float diff = max(dot(normal, lightDir), 0.0f);
		vec3 diffuse = diff * lightColor;

		// specular
		float specularStrength = 0.1;
		vec3 viewDir = normalize(viewPos - position);
		vec3 midDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, midDir), 0.0f), 32);
		vec3 specular = specularStrength * spec * lightColor;

		return ambient + diffuse + specular;
	}
};