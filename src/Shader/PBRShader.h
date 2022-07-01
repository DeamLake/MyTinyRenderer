#pragma once
#include "IShader.h"

static vec3 FresnelSchlick_Roughness(float h_dot_v, vec3& f0, float roughness)
{
	// 粗糙度影响反射率
	float r1 = 1.0f - roughness;
	if (r1 < f0[0]) { r1 = f0[0]; }
	return f0 + (vec3(r1, r1, r1) - f0) * pow(1 - h_dot_v, 5.0f);
}

static float float_aces(float value)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	value = (value * (a * value + b)) / (value * (c * value + d) + e);
	return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

// Tone mapping
static void Reinhard_mapping(vec3& color)
{
	for (int i = 0; i < 3; i++)
	{
		color[i] = float_aces(color[i]);
		color[i] = pow(color[i], 1.0f / 2.2f);
	}
}

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
		vec3 normal = tbn_normal(payload, bary, uv);
		vec3 viewDir = normalize(payload->pViewPos - position);
		float n_dot_v = max(dot(normal, viewDir), 0.0f);

		if (n_dot_v > 0)
		{
			float roughness = payload->model->roughness(uv); // 粗糙度
			float metalness = payload->model->metalness(uv); // 金属度
			float occlusion = payload->model->occlusion(uv); // 几何遮挡
			vec3 emission = payload->model->emission(uv); // 自发光项

			vec3 albedo = payload->model->diffuse(uv); // 颜色
			vec3 tmp = vec3(0.04);
			vec3 f0 = tmp + (albedo - tmp) * metalness; // 光线反射率 用金属度插值得到

			// 计算漫反射项所占比率
			vec3 F = FresnelSchlick_Roughness(n_dot_v, f0, roughness);
			vec3 kD = (vec3(1.0f) - F) * (1 - metalness); 
			
			// 漫反射项
			vec3 irradiance = cubemap_sampling(normal, payload->iblMap->irradiance_map);
			for (int i = 0; i < 3; i++)
				irradiance[i] = irradiance[i] * irradiance[i];
			vec3 diffuse = irradiance * kD * albedo;

			// 镜面反射项
			vec3 r = normalize(2.0f * dot(viewDir, normal) * normal - viewDir); // 反射光
			vec2 lut_uv = vec2(n_dot_v, roughness);
			vec3 lut_sample = texture_sample(lut_uv, payload->iblMap->brdf_lut); // 获取预计算值
			float specular_scale = lut_sample.x;
			float specular_bias = lut_sample.y;
			vec3 specular = f0 * specular_scale + vec3(specular_bias);
			float max_mip_level = static_cast<float>(payload->iblMap->mip_levels - 1);
			int specular_miplevel = static_cast<int>(roughness * max_mip_level + 0.5f);
			vec3 prefilter_color = cubemap_sampling(r, payload->iblMap->prefilter_maps[specular_miplevel]);
			for (int i = 0; i < 3; i++)
				prefilter_color[i] = prefilter_color[i] * prefilter_color[i];
			specular = vec3(prefilter_color[0] * specular[0], prefilter_color[1] * specular[1], prefilter_color[2] * specular[2]);
			color = diffuse + specular + emission;
		}

		Reinhard_mapping(color);
		//测试不通过
		return false;
	}
};