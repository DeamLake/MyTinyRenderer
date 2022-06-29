#include "Rasterizer.h"

Rasterizer::Rasterizer(int w, int h) 
	:width(w), height(h), gScreenHdc(NULL), gDepthBuffer(w, h)
{

}

void Rasterizer::ClearDepth() 
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			gDepthBuffer.depthBuffer[i][j] = -256;
		}
	}
}

glm::mat4 Rasterizer::calculate_model(float angle, const glm::vec3& scales, const glm::vec3& trans)
{
	glm::mat4 rotation, scaletion, translate;
	angle = angle * MY_PI / 180.0f;

	float rotation_list[16] = { cos(angle), 0,-sin(angle), 0,
											0, 1, 0, 0,
											sin(angle), 0, cos(angle), 0,
											0, 0, 0, 1 };

	rotation = glm::make_mat4(rotation_list);

	float scaletion_list[16] = { scales.x, 0, 0, 0,
											0, scales.y, 0, 0,
											0, 0, scales.z, 0,
											0, 0, 0, 1 };

	scaletion = glm::make_mat4(scaletion_list);

	float translate_list[16] = { 1, 0, 0, trans.x,
											0, 1, 0, trans.y,
											0, 0, 1, trans.z,
											0, 0, 0, 1 };

	translate = glm::make_mat4(translate_list);
	return rotation * scaletion * translate;
}

void Rasterizer::update_lookat(glm::vec3& view_point, glm::vec3& center, glm::vec3& up)
{
	glm::vec3 z = normalize(view_point - center);
	glm::vec3 x = normalize(cross(up, z));
	glm::vec3 y = normalize(cross(z, x));
	glm::mat4x4 Minv(1), Translate(1);
	for (int i : {0, 1, 2}) {
		Minv[0][i] = x[i];
		Minv[1][i] = y[i];
		Minv[2][i] = z[i];
		Translate[i][3] = -view_point[i];
	}
	ModelViewMat = Translate * Minv;
}

void Rasterizer::update_projection(float zNear, float zFar, float eye_fov)
{
	float aspect_ratio = static_cast<float>(width) / height;
	glm::mat4 m, n, p;

	float m_list[16] = { zNear,0,0,0,
									0,zNear,0,0,
									0,0,zNear+zFar,-zNear*zFar,
									0,0,1,0 };

	float half = eye_fov / 2 * MY_PI / 180.0f;
	float top = zNear * tan(half);
	float down = -top;
	float right = top * aspect_ratio;
	float left = -right;

	float n_list[16] = { 2/abs(right-left),0,0,0,
									0,2/abs(top-down),0,0,
									0,0,2/abs(zFar-zNear),0,
									0,0,0,1};

	float p_list[16] = { 1,0,0,-(left+right)/2,
									0,1,0,-(down+top)/2,
									0,0,1,-(zNear+zFar)/2,
									0,0,0,1 };

	m = glm::make_mat4(m_list);
	n = glm::make_mat4(n_list);
	p = glm::make_mat4(p_list);
	this->ProjectionMat = m * p * n;
}

void Rasterizer::SetUpEnvironment(EnvData* data)
{
	this->camera = data->camera;
	this->pLightPos = data->LightPos;
	this->pLightColor =data->LightColor;
	this->zNear = data->zNear;
	this->zFar = data->zFar;
	this->eye_fov = data->eye_fov;
}

void Rasterizer::Add_Object(ModelData data)
{
	IShader* shader = data.shader;
	shader->set_model_data(data.model);
	shader->World_mat = calculate_model(data.yangle, data.scales, data.translate);
	shader->pLightPos = pLightPos;
	shader->pLightColor = pLightColor;
	gObjects->push_back(data);
	gObjectSize++;
}

void Rasterizer::draw_model(Model* model_data, IShader* shader, Camera* camera)
{
	update_lookat(camera->eye, camera->target, camera->up);
	update_projection(this->zNear, this->zFar, this->eye_fov);
	shader->ViewProj_mat = ModelViewMat * ProjectionMat;
	shader->pViewPos = camera->eye;

	for (int i = 0; i < model_data->nfaces(); i++) {
		int ClipStateCode = 63;
		std::vector<glm::vec4> coords(3);
		for (int j = 0; j < 3; j++) {
			coords[j] = shader->vertex(i, j);
			// w需要被复用  所以不能直接除
			coords[j].x /= coords[j].w;
			coords[j].y /= coords[j].w;
			coords[j].z /= coords[j].w;
			int CodeTmp = 0;
			CodeTmp |= (coords[j][0] < -1) << 0;
			CodeTmp |= (coords[j][0] > 1) << 1;
			CodeTmp |= (coords[j][1] < -1) << 2;
			CodeTmp |= (coords[j][1] > 1) << 3;
			CodeTmp |= (coords[j][2] < -1) << 4;
			CodeTmp |= (coords[j][2] > 1) << 5;
			ClipStateCode &= CodeTmp;
		}

		// NDC 裁剪
		if (ClipStateCode > 0) { continue; }

		{
			// 背面裁剪
			glm::vec3  AB = coords[1] - coords[0];
			glm::vec3  AC = coords[2] - coords[0];
			if (AB[0] * AC[1] - AC[0] * AB[1] < 0)
				continue;
		}

		for (int j = 0; j < 3; j++) 
		{
			// 视口转换
			coords[j].x = (coords[j].x + 1.0f)* (width - 1) / 2;
			coords[j].y = (coords[j].y + 1.0f) * (height - 1) / 2;
			coords[j].z = (coords[j].z + 1.0f) * 255.0f/2 ;
		}

		draw_triangle(coords, shader);
	}
}

void Rasterizer::rotate_object(float angle, ModelData& data)
{
	data.shader->World_mat = calculate_model(angle, data.scales, data.translate);
}

glm::vec3 Rasterizer::baryCentric(const std::vector<glm::vec4>& v, float x, float y) const 
{
	glm::vec3 s1(v[1].x - v[0].x, v[2].x - v[0].x, v[0].x - x);
	glm::vec3 s2(v[1].y - v[0].y, v[2].y - v[0].y, v[0].y - y);
	glm::vec3 bcCood = glm::cross(s1, s2);
	glm::vec3 bary(-1, 1, 1);

	if (abs(bcCood.z) > 1e-2) {
		bary.x = 1.0f - (bcCood.x + bcCood.y) / bcCood.z;
		bary.y = bcCood.x / bcCood.z;
		bary.z = bcCood.y / bcCood.z;
	}
	return bary;
}

bool Rasterizer::inside_triangle(glm::vec3& bcCoord) const 
{
	if (bcCoord.x < 0 || bcCoord.y < 0 || bcCoord.z < 0)
		return false;
	return true;
}

void Rasterizer::draw_triangle(std::vector<glm::vec4>& v, IShader* shader) 
{
	float x_min = max(0.01f, min(v[0].x,min(v[1].x,v[2].x)));
	float x_max = min(width - 0.01f, max(v[0].x,max(v[1].x,v[2].x)));
	float y_min = max(0.01f, min(v[0].y,min(v[1].y,v[2].y)));
	float y_max = min(height - 0.01f, max(v[0].y, max(v[1].y,v[2].y)));

	for (int x = x_min; x <= x_max; x++) {
		for (int y = y_min; y <= y_max; y++) {
			glm::vec3 bcCoord = baryCentric(v, x, y);
			if (inside_triangle(bcCoord)) {
				// 便利性计算
				bcCoord.x /= v[0].w; bcCoord.y /= v[1].w; bcCoord.z /= v[2].w;
				float zp = 1.0f / (bcCoord[0] + bcCoord[1] + bcCoord[2]);
				bcCoord *= zp;

				if (zp > gDepthBuffer.Sample(x, y)) {
					SetDepth(x, y, zp);
					glm::vec3 color;
					shader->fragment(bcCoord, color);
					DrawPixel(x, y, color);
				}
			}
		}
	}

}

void Rasterizer::DrawPixel(int x, int y, glm::vec3& color)
{
	y = height - y-1;
	SetPixel(gScreenHdc, x, y, RGB(min(255.0f,color.r), min(255.0f,color.g), min(255.0f, color.b)));
}
void Rasterizer::SetDepth(int x, int y, float depth)
{
	//y = height - y-1;
	gDepthBuffer.depthBuffer[y][x] = depth;
}

