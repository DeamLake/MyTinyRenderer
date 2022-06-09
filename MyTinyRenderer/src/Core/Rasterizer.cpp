#include "Rasterizer.h"

Rasterizer::Rasterizer(int w, int h):width(w), height(h)
{
	frame_buf.resize(w * h);
	depth_buf.resize(w * h);
}

void Rasterizer::clear(Buffers buff) 
{
	if ((buff & Buffers::Color) == Buffers::Color)
	{
		std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{ 0, 0, 0 });
	}
	if ((buff & Buffers::Depth) == Buffers::Depth)
	{
		std::fill(depth_buf.begin(), depth_buf.end(), -std::numeric_limits<float>::infinity());
	}
}

void Rasterizer::show()
{
	cv::Mat image(height, width, CV_32FC3, frame_buf.data());
	image.convertTo(image, CV_8UC3, 1.0f);
	cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
	cv::imshow("image", image);
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
	float derror = std::abs(dy / float(dx));//斜率
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

void Rasterizer::draw_wireframe(Model* model, const Eigen::Vector3f& color) 
{
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++) {
			Eigen::Vector3f v0 = model->vert(face[j]);
			Eigen::Vector3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x() + 1.0) * (width - 1) / 2;
			int y0 = (v0.y() + 1.0) * (height - 1) / 2;
			int x1 = (v1.x() + 1.0) * (width - 1) / 2;
			int y1 = (v1.y() + 1.0) * (height - 1) / 2;

			draw_line(Eigen::Vector2f(x0, y0), Eigen::Vector2f(x1, y1), color);
		}
	}
}

Eigen::Vector3f Rasterizer::baryCentric(std::vector<Eigen::Vector3f>& v, Eigen::Vector3f& p) const
{
	// AP 可以用 uAB + vAC来表示 AP = uAB + vAC => uAB + vAC + PA = 0
	// 即 (u, v, 1) 和 (ABx, ACx, PAx) 、 (ABy, ACy, PAy)都垂直 即求后两个向量叉积
	Eigen::Vector3f ab, ac, pa;
	ab = v[1] - v[0];
	ac = v[2] - v[0];
	pa = v[0] - p;
	
	Eigen::Vector3f s1(ab.x(), ac.x(), pa.x()), s2(ab.y(), ac.y(), pa.y());
	Eigen::Vector3f bcCood = s1.cross(s2);
	if (std::abs(bcCood.z() > 1e-2))
		return Eigen::Vector3f(1.0f - (bcCood.x() + bcCood.y()) / bcCood.z(), bcCood.y() / bcCood.z(), bcCood.x() / bcCood.z());
	return Eigen::Vector3f(-1, 1, 1);
}

bool Rasterizer::inside_triangle(std::vector<Eigen::Vector3f>& triangle, float x, float y, Eigen::Vector3f& bcCoord) const
{
	std::vector<Eigen::Vector3f> v(3);
	Eigen::Vector3f p(x, y, 1.0);
	for (int i = 0; i < 3; i++)
		v[i] = Eigen::Vector3f(triangle[i].x(), triangle[i].y(), 1.0);
	bcCoord = baryCentric(v, p);

	if (bcCoord.x() < 0 || bcCoord.y() < 0 || bcCoord.z() < 0)
		return false;
	return true;
}

void Rasterizer::draw_triangle(std::vector<Eigen::Vector3f>& v, const Eigen::Vector3f& color)
{
	float x_min = std::min({ v[0].x(),v[1].x(),v[2].x() });
	float x_max = std::max({ v[0].x(),v[1].x(),v[2].x() });
	float y_min = std::min({ v[0].y(),v[1].y(),v[2].y() });
	float y_max = std::max({ v[0].y(),v[1].y(),v[2].y() });

	for (int x = x_min; x <= x_max; x++) {
		for (int y = y_min; y < y_max; y++) {
			Eigen::Vector3f bcCoord;
			if (inside_triangle(v, x, y, bcCoord)) {
				float depth = v[0].z() * bcCoord.x() + v[1].z() * bcCoord.y() + v[2].z() * bcCoord.z();
				Eigen::Vector2f point(x, y);
				if (depth > get_depth(point)) {
					set_depth(point, depth);
					set_pixel(point, color);
				}
			}
		}
	}
}

void Rasterizer::draw_model(Model* model, const Eigen::Vector3f& color) 
{
	Eigen::Vector3f Light(0, 0, 1);
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		std::vector<Eigen::Vector3f> triangle(3);
		std::vector<Eigen::Vector3f> world(3);
		for (int j = 0; j < 3; j++) {
			world[j] = model->vert(face[j]);
			triangle[j] = Eigen::Vector3f((world[j].x() + 1.0) * (width - 1) / 2, (world[j].y() + 1.0) * (height - 1) / 2, world[j].z());
		}
		Eigen::Vector3f normal = (world[2] - world[0]).cross(world[1] - world[0]);
		normal.normalize();
		float intense = normal.dot(-Light);
		if (intense > 0)
			draw_triangle(triangle, intense * color);
	}
}

void Rasterizer::set_pixel(const Eigen::Vector2f& point, const Eigen::Vector3f& color) 
{
	int ind = (height - 1 - point.y()) * width + point.x();
	frame_buf[ind] = color;
}

void Rasterizer::set_depth(const Eigen::Vector2f& point, float depth) 
{
	int ind = (height - 1 - point.y()) * width + point.x();
	depth_buf[ind] = depth;
}

float Rasterizer::get_depth(const Eigen::Vector2f& point) const 
{
	int ind = (height - 1 - point.y()) * width + point.x();
	return depth_buf[ind];
}