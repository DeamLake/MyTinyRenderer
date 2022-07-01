#pragma once
#include "IShader.h"

class SkyBoxShader : public IShader
{
public:
	SkyBoxShader() : IShader() {}

	vec4 vertex(int iface, int nthvert) final 
	{
		vec4 position(model->vert(iface, nthvert), 1);
		VertPosition[nthvert] = position;
		vec4 ret =  position * ViewProj_mat;
		return ret;
	}

	bool fragment(const vec3& bary, vec3& color) final 
	{
		if (this->iblMap == nullptr) {
			color = vec3(255.0f);
			return false;
		}

		vec3 position = VertPosition * bary;
		color = cubemap_sampling(position, iblMap->irradiance_map);

		//²âÊÔ²»Í¨¹ı
		return false;
	}
};