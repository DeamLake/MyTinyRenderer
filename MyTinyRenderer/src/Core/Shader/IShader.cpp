#include "IShader.h"

Eigen::Matrix4f Model_mat = Eigen::Matrix4f::Identity();
Eigen::Matrix4f View_mat = Eigen::Matrix4f::Identity();
Eigen::Matrix4f Projection_mat = Eigen::Matrix4f::Identity();

void IShader::mvp_translate(Eigen::Vector3f& p)
{
	Eigen::Vector4f v(p.x(), p.y(), p.z(), 1);
	v = mvp * v;
	v /= v.w();
	p.x() = v.x();
	p.y() = v.y();
	p.z() = v.z();
}