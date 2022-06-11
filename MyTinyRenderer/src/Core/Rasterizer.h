#pragma once
#include "../Platform/Utils.h"
#include "../Resource/model.h"
#include "Shader.h"

enum class Buffers
{
	Color = 1,
	Depth = 2
};

inline Buffers operator|(Buffers a, Buffers b)
{
	return Buffers((int)a | (int)b);
}

inline Buffers operator&(Buffers a, Buffers b)
{
	return Buffers((int)a & (int)b);
}

class Rasterizer {
public:
	Rasterizer(int w, int h, int eyefov, float zn, float zf, const Eigen::Vector3f& vp);

	void clear(Buffers buff);
	void show();

	void draw_wireframe(Model* model_data, const Eigen::Vector3f& color);
	void draw_model(Model* model_data, IShader* shader);

	void set_model(float angle, float scale);
	void set_view(const Eigen::Vector3f& view_point);
	void set_projection();
	
protected:

	// 基础绘制操作
	Eigen::Vector3f baryCentric(const std::vector<Eigen::Vector3f>& v, float x, float y) const;
	void draw_line(const Eigen::Vector2f& begin, const Eigen::Vector2f& end, const Eigen::Vector3f& color);
	bool inside_triangle(std::vector<Eigen::Vector3f>& v, float x, float y, Eigen::Vector3f& bcCoord) const;
	void draw_triangle(std::vector<Eigen::Vector3f>& v, IShader* shader);

	// 缓冲区操作
	void set_pixel(const Eigen::Vector2f& point, const Eigen::Vector3f& color);
	void set_depth(const Eigen::Vector2f& point, float depth);
	float get_depth(const Eigen::Vector2f& point) const;

private:	
	int width, height, eye_fov;
	float zNear, zFar, aspect_ratio;
	Eigen::Matrix4f Model_mat, View_mat, Projection_mat;
	Eigen::Vector3f ViewPoint;
	std::vector<Eigen::Vector3f> frame_buf;
	std::vector<float> depth_buf;
};