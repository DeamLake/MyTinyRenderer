#pragma once
#include "../Platform/Utils.h"

class Rasterizer {
public:
	Rasterizer(int w, int h);

	void draw_line(const Eigen::Vector2f& begin, const Eigen::Vector2f& end, const Eigen::Vector3f& color);
	void set_pixel(const Eigen::Vector2f& point, const Eigen::Vector3f& color);
	void show();
private:	
	int width, height;
	Eigen::Matrix4f model, view, projection;
	std::vector<Eigen::Vector3f> frame_buf;
};