#pragma once
#include "IShader.h"

class GouraudShader : public IShader {
private:
	std::vector<Eigen::Vector3f> coords;
	std::vector<Eigen::Vector3f> colors;

public:
	GouraudShader()
	{
		coords = std::vector<Eigen::Vector3f>(3);
		colors = std::vector<Eigen::Vector3f>(3);
	}

	virtual bool vertex(int iface, int nthvert, Eigen::Vector3f& coord_ret) final
	{
		// 计算屏幕坐标
		Eigen::Vector3f Light(0, 0, 1);
		std::vector<int> face = model->face(iface);
		coords[nthvert] = model->vert(face[nthvert]);
		mvp_translate(coords[nthvert]);
		coord_ret = Eigen::Vector3f((coords[nthvert].x() + 1.0f) * (width - 1) / 2, (coords[nthvert].y() + 1.0f) * (height - 1) / 2, coords[nthvert].z());;
		coords[nthvert] = coord_ret;

		// 计算颜色
		Eigen::Vector3f norm = model->norm(iface, nthvert); //法线
		Eigen::Vector2i uv = model->uv(iface, nthvert); // 材质uv
		TGAColor mat_color = model->diffuse(uv); //材质颜色
		float intense = std::max(0.0f, norm.dot(Light)); // 光照强度
		colors[nthvert] = intense * Eigen::Vector3f(mat_color.r, mat_color.g, mat_color.b);
		return true;
	}

	virtual void fragment(const Eigen::Vector3f& bary, Eigen::Vector3f& color_ret) final
	{
		float alpha = bary.x(), beta = bary.y(), gamma = bary.z();
		float zp = 1.0f / (alpha / coords[0].z() + beta / coords[1].z() + gamma / coords[2].z());
		color_ret = (alpha * colors[0] / coords[0].z() + beta * colors[1] / coords[1].z() + gamma * colors[2] / coords[2].z()) * zp;
	}
};