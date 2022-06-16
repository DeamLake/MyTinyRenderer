#pragma once
#include "IShader.h"

class GouraudShader : public IShader
{
public:
	GouraudShader() : IShader() {}

	vec4 vertex(int iface, int nthvert) final {
		// ��ȡ����
		vec4 position(model->vert(model->face(iface)[nthvert]), 1);
		// ת������������
		position = World_mat * position;
		VertPosition[nthvert] = position;

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
		vec2 uv = (alpha * VertUV[0] / zs[0] + beta * VertUV[1] / zs[1] + gamma * VertUV[2] / zs[2]) * zp;
		vec3 position = (alpha * VertPosition[0] / zs[0] + beta * VertPosition[1] / zs[1] + gamma * VertPosition[2] / zs[2]) * zp;
		vec3 lightPos = *pLightPos;
		vec3 lightDir = normalize(lightPos - position);

		std::vector<float> intensitys(3);
		for (int i = 0; i < 3; i++)
			intensitys[i] = max(0.0f, dot(VertNormal[i], lightDir));
		float intense = (alpha * intensitys[0] / zs[0] + beta * intensitys[1] / zs[1] + gamma * intensitys[2] / zs[2]) * zp;
		TGAColor color = model->diffuse(uv);

		return intense * vec3(color.r, color.g, color.b);
	}
};