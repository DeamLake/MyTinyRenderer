#pragma once
#include <Eigen/Eigen>
#include "../../Resource/model.h"

class IShader {
public:
	virtual ~IShader() {}
	void set_model_data(Model* model_data) { model = model_data; }
	void mvp_translate(Eigen::Vector3f& p);
	virtual Eigen::Vector3f vertex(int iface, int nthvert) = 0;
	virtual Eigen::Vector3f fragment(const Eigen::Vector3f& bary) = 0;
public:
	Model* model; 
	int width, height;
	Eigen::Matrix4f mvp;
};

