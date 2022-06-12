#include "Rasterizer.h"

Rasterizer::Rasterizer(int w, int h, int eyefov, float zn, float zf, const Eigen::Vector3f& vp)
	:width(w), height(h), eye_fov(eyefov), zNear(zn), zFar(zf), ViewPoint(vp)
{
	frame_buf.resize(w * h);
	depth_buf.resize(w * h);
	aspect_ratio = float(width) / height;
	set_model(0.0, 1);
	set_view(ViewPoint);
	set_projection();
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
	assert(0,"has not been design");
	// 点着色未修改
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
			//set_pixel(Eigen::Vector2f(y, x), color);
		}
		else {
			//set_pixel(Eigen::Vector2f(x, y), color);
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
			int x0 = (v0.x() + 1) * (width - 1) / 2;
			int y0 = (v0.y() + 1) * (height - 1) / 2;
			int x1 = (v1.x() + 1) * (width - 1) / 2;
			int y1 = (v1.y() + 1) * (height - 1) / 2;

			draw_line(Eigen::Vector2f(x0, y0), Eigen::Vector2f(x1, y1), color);
		}
	}
}

Eigen::Vector3f Rasterizer::baryCentric(const std::vector<Eigen::Vector3f>& v, float x, float y) const
{
	// AP 可以用 uAB + vAC来表示 AP = uAB + vAC => uAB + vAC + PA = 0
	// 即 (u, v, 1) 和 (ABx, ACx, PAx) 、 (ABy, ACy, PAy)都垂直 即求后两个向量叉积
	/*Eigen::Vector3f ab, ac, pa;
	ab = v[1] - v[0];
	ac = v[2] - v[0];
	pa = v[0] - p;*/
	Eigen::Vector3f p(x, y, 1);
	Eigen::Vector3f s1(v[1].x() - v[0].x(), v[2].x() - v[0].x(), v[0].x() - p.x());
	Eigen::Vector3f s2(v[1].y() - v[0].y(), v[2].y() - v[0].y(), v[0].y() - p.y());
	Eigen::Vector3f bcCood = s1.cross(s2);

	if (std::abs(bcCood.z() > 1e-2))
		return Eigen::Vector3f(1.0f - (bcCood.x() + bcCood.y()) / bcCood.z(), bcCood.x() / bcCood.z(), bcCood.y() / bcCood.z());
	return Eigen::Vector3f(-1, 1, 1);
}

bool Rasterizer::inside_triangle(std::vector<Eigen::Vector3f>& triangle, float x, float y, Eigen::Vector3f& bcCoord) const
{
	if (bcCoord.x() < 0 || bcCoord.y() < 0 || bcCoord.z() < 0)
		return false;
	return true;
}

void Rasterizer::draw_triangle(std::vector<Eigen::Vector3f>& v, IShader* shader)
{
	float x_min = std::max(0.0f, std::min({ v[0].x(),v[1].x(),v[2].x() }));
	float x_max = std::min(float(width), std::max({ v[0].x(),v[1].x(),v[2].x() }));
	float y_min = std::max(0.0f, std::min({ v[0].y(),v[1].y(),v[2].y() }));
	float y_max = std::min(float(height), std::max({ v[0].y(),v[1].y(),v[2].y() }));

	for (int x = x_min; x <= x_max; x++) {
		for (int y = y_min; y <= y_max; y++) {
			Eigen::Vector3f bcCoord = baryCentric(v, x, y);
			if (inside_triangle(v, x, y, bcCoord)) {
				float alpha = bcCoord.x(), beta = bcCoord.y(), gamma = bcCoord.z();
				float zp = 1.0f / (alpha / v[0].z() + beta / v[1].z() + gamma / v[2].z());
				int index = get_index(Eigen::Vector2i(x, y));

				if (zp > depth_buf[index]) {
					depth_buf[index] = zp;
					shader->fragment(bcCoord, frame_buf[index]);
				}
			}
		}
	}
}

void Rasterizer::draw_model(Model* model_data, IShader* shader)
{
	shader->set_model_data(model_data);
	shader->width = width;
	shader->height = height;
	shader->mvp = this->Projection_mat * this->View_mat * this->Model_mat;

	for (int i = 0; i < model_data->nfaces(); i++) {
		std::vector<Eigen::Vector3f> coords(3);
		for (int j = 0; j < 3; j++) 
			shader->vertex(i, j, coords[j]);

		{
			// 背面剔除
			Eigen::Vector3f  AB = coords[1] - coords[0];
			Eigen::Vector3f  AC = coords[2] - coords[0];
			if (AB.x() * AC.y() - AC.x() * AB.y() <= 0)
				continue;
		}

		draw_triangle(coords, shader);
	}
}

void Rasterizer::set_model(float angle, float scale)
{
	Eigen::Matrix4f rotation, scaletion, translate;
	// 绕Z轴转
	angle = angle * MY_PI / 180.0f;
	rotation << cos(angle), -sin(angle), 0, 0,
		sin(angle), cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	scaletion << scale, 0, 0, 0,
		0, scale, 0, 0,
		0, 0, scale, 0,
		0, 0, 0, 1;

	translate << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	this->Model_mat = translate * scaletion * rotation;
}

void Rasterizer::set_view(const Eigen::Vector3f& view_point)
{
	this->View_mat << 1, 0, 0, -view_point.x(),
		0, 1, 0, -view_point.y(),
		0, 0, 1, -view_point.z(),
		0, 0, 0, 1;
}

void Rasterizer::set_projection()
{
	float aspect_ratio = float(width) / height;

	Eigen::Matrix4f m, n, p;
	// 传统透视矩阵
	m << zNear, 0, 0, 0,
		0, zNear, 0, 0,
		0, 0, zNear + zFar, -zNear * zFar,
		0, 0, 1, 0;

	float half = eye_fov / 2 * MY_PI / 180.0f;// 半角
	float top = zNear * tan(half);
	float down = -top;
	float right = top * aspect_ratio;
	float left = -right;

	// 标准化 由于 zNear可能是负的  所以 标准化时可能会将值取反 导致上下颠倒
	n << 2 / abs(right - left), 0, 0, 0,
		0, 2 / abs(top - down), 0, 0,
		0, 0, 2 / abs(zFar - zNear), 0,
		0, 0, 0, 1;

	// 移到中间
	p << 1, 0, 0, -(left + right) / 2,
		0, 1, 0, -(down + top) / 2,
		0, 0, 1, -(zNear + zFar) / 2,
		0, 0, 0, 1;

	this->Projection_mat = n * p * m;
}

int Rasterizer::get_index(Eigen::Vector2i point){
	return (height - 1 - point.y()) * width + point.x();
}