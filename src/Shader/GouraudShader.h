#pragma once
#include "IShader.h"

class GouraudShader : public IShader
{
public:
	GouraudShader() : IShader() {}

	vec4 vertex(int iface, int nthvert) final {
		// ��ȡ����
		vec4 position(model->vert(iface,nthvert), 1);
		// ת������������
		position = position * World_mat;
		VertPosition[nthvert] = position;

		// ���㷨��
		VertNormal[nthvert] = vec4(model->normal(iface, nthvert), 1) * transpose(inverse(World_mat));

		// ��ȡ����UV
		VertUV[nthvert] = model->uv(iface, nthvert);

		// ����NDC����
		return position * ViewProj_mat;
	}

	bool fragment(const vec3& bary, vec3& color) final {
		vec2 uv = VertUV * bary;
		vec3 position = VertPosition * bary;
		vec3 normal = normalize(VertNormal * bary);
		vec3 lightDir = normalize(pLightPos - position);
		vec3 viewDir = normalize(pViewPos - position);
		
		// tbn ת�� normal mapping
		mat3x3 AI = inverse(mat3x3{ VertPosition[1] - VertPosition[0], VertPosition[2] - VertPosition[0], normal });
		vec3 i = vec3(VertUV[1][0] - VertUV[0][0], VertUV[2][0] - VertUV[0][0], 0) * AI;
		vec3 j = vec3(VertUV[1][1] - VertUV[0][1], VertUV[2][1] - VertUV[0][1], 0) * AI;
		mat3x3 TBN = transpose(mat3x3{ normalize(i), normalize(j), normal });
		normal = normalize(model->normal(uv) * TBN);

		vec3 midDir = normalize(lightDir + viewDir);
		float spec = pow(max(0.0f, dot(midDir, normal)), 200 );
		float intense = max(0.0f, dot(normal, lightDir));

		color = (intense+0.6f * spec) * model->diffuse(uv);

		//���Բ�ͨ��
		return false;
	}
};