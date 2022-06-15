#pragma once
#include "IShader.h"

class BlingPhongShader : public IShader
{
public:
	BlingPhongShader(): IShader() {}

	vec3 vertex(int iface, int nthvert) final {
		// 获取坐标
		VertPosition[nthvert] = model->vert(model->face(iface)[nthvert]);
		// 转换到世界坐标
		VertPosition[nthvert] = mvp_translate(VertPosition[nthvert], World_mat);

		// 获取法向量
		VertNormal[nthvert] = model->norm(iface, nthvert);
		// 法向量转换
		VertNormal[nthvert] = mat3(transpose(inverse(World_mat))) * VertNormal[nthvert];

		// 获取颜色
		TGAColor color = model->diffuse(model->uv(iface, nthvert));
		VertColor[nthvert] = vec3(color.r, color.g, color.b);

		// 返回NDC坐标
		return mvp_translate(VertPosition[nthvert], ViewProj_mat);
	}

	vec3 fragment(const vec3& bary, vec3 zs) final {
		float alpha = bary.x, beta = bary.y, gamma = bary.z;
		float zp = 1.0f / (alpha / zs[0] + beta / zs[1] + gamma / zs[2]);
		vec3 position = (alpha * VertPosition[0] / zs[0] + beta * VertPosition[1] / zs[1] + gamma * VertPosition[2] / zs[2]) * zp;
		vec3 normal = (alpha * VertNormal[0] / zs[0] + beta * VertNormal[1] / zs[1] + gamma * VertNormal[2] / zs[2]) * zp;
		vec3 color = (alpha * VertColor[0] / zs[0] + beta * VertColor[1] / zs[1] + gamma * VertColor[2] / zs[2]) * zp;

		// ambient
		float ambientStrength = 0.05f;
		vec3 ambient = ambientStrength * LightColor;

		// diffuse 
		normal = normalize(normal);
		vec3 lightDir = normalize(LightPos - position);
		float diff = max(dot(normal, lightDir), 0.0f);
		vec3 diffuse = diff * LightColor;

		// specular
		float specularStrength = 0.1;
		vec3 viewDir = normalize(ViewPos - position);
		vec3 midDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, midDir), 0.0f), 32);
		vec3 specular = specularStrength * spec * LightColor;

		return ambient + diffuse + specular;
	}
};