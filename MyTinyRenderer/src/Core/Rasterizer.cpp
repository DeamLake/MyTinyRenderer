#include "Rasterizer.h"

Rasterizer::Rasterizer(int w, int h):width(w), height(h)
{
	frame_buf.resize(w * h);
}

void Rasterizer::draw_line(const Eigen::Vector2f& begin, const Eigen::Vector2f& end, const Eigen::Vector3f& color) 
{
	bool steep = false;
	int x0 = begin.x(), y0 = begin.y();
	int x1 = end.x(), y1 = end.y();
	if (std::abs(y1 - y0) > std::abs(x1 - x0)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	float derror = std::abs(dy / float(dx));//Ð±ÂÊ
	float error = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			set_pixel(Eigen::Vector2f(y, x), color);
		}else {
			set_pixel(Eigen::Vector2f(x, y), color);
		}
		error += derror;
		if (error > 0.5) {
			y += y1 > y0 ? 1 : -1;
			error -= 1;
		}
	}
}

void Rasterizer::set_pixel(const Eigen::Vector2f& point, const Eigen::Vector3f& color) 
{
	auto ind = (height - 1 - point.y()) * width + point.x();
	frame_buf[ind] = color;
}

void Rasterizer::show() 
{
	cv::Mat image(height, width, CV_32FC3, frame_buf.data());
	image.convertTo(image, CV_8UC3, 1.0f);
	cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
	cv::imshow("image", image);
}
