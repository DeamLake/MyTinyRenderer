#pragma once
#include <Eigen/Eigen>
#include "../../Resource/model.h"

class IShader {
public:
	virtual ~IShader() {}
	void set_model_data(Model* model_data) { model = model_data; }
	void mvp_translate(Eigen::Vector3f& p);
	virtual bool vertex(int iface, int nthvert, Eigen::Vector3f& coord) = 0;
	virtual void fragment(const Eigen::Vector3f& bary, Eigen::Vector3f& color) = 0;
public:
	Model* model; 
	int width, height;
	Eigen::Matrix4f mvp;
};

