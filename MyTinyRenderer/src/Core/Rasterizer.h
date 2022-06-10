#pragma once
#include "../Platform/Utils.h"
#include "../Resource/model.h"

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
	Rasterizer(int w, int h);

	void clear(Buffers buff);
	void show();

	
	void draw_wireframe(Model* model_data, const Eigen::Vector3f& color);
	void draw_model(Model* model_data, const Eigen::Vector3f& color);

	void set_model(float angle, float scale);
	void set_view(const Eigen::Vector3f& view_point);
	void set_projection(float eye_fov, float aspect_ratio, float zNear, float zFar);
	void mvp_translate(Eigen::Vector3f& p, Eigen::Matrix4f& mvp);
	
protected:

	// 基础绘制操作
	Eigen::Vector3f baryCentric(std::vector<Eigen::Vector3f>& v, Eigen::Vector3f& p) const;
	void draw_line(const Eigen::Vector2f& begin, const Eigen::Vector2f& end, const Eigen::Vector3f& color);
	bool inside_triangle(std::vector<Eigen::Vector3f>& v, float x, float y, Eigen::Vector3f& bcCoord) const;
	void draw_triangle(std::vector<Eigen::Vector3f>& v, const Eigen::Vector3f& color);

	// 缓冲区操作
	void set_pixel(const Eigen::Vector2f& point, const Eigen::Vector3f& color);
	void set_depth(const Eigen::Vector2f& point, float depth);
	float get_depth(const Eigen::Vector2f& point) const;


private:	
	int width, height;
	Eigen::Matrix4f model, view, projection;
	std::vector<Eigen::Vector3f> frame_buf;
	std::vector<float> depth_buf;
};