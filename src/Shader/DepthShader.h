#pragma once
#include "IShader.h"

class DepthShader : public IShader
{
public:
	DepthShader() : IShader() {}

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
		vec4 position = vec4(VertPosition * bary, 1.0f);
		float depth = (position * ViewProj_mat).w;
		float zNear = -0.1f, zFar = -10.0f;
		float zDepth = 1 - (depth - zNear) / (zFar - zNear);
		color = vec3(255, 255, 255) * zDepth;
		// ���Բ�ͨ��
		return false;
	}
};