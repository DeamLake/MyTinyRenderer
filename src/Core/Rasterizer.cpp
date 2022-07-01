#include "Rasterizer.h"
#include "Triangle_Clip.h"

void Rasterizer::ClearAll()
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			gDepthBuffer.depthBuffer[i][j] = -256;
			int index = (i * width + j) * 4;
			framebuffer[index + 2] = 80;
			framebuffer[index + 1] = 56;
			framebuffer[index] = 56;
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

void Rasterizer::update_lookat(glm::vec3& view_point, glm::vec3& center, glm::vec3& up, bool isSkyBox)
{
	glm::vec3 z = normalize(view_point - center);
	glm::vec3 x = normalize(cross(up, z));
	glm::vec3 y = normalize(cross(z, x));
	glm::mat4x4 Minv(1), Translate(1);
	for (int i : {0, 1, 2}) {
		Minv[0][i] = x[i];
		Minv[1][i] = y[i];
		Minv[2][i] = z[i];
		if (!isSkyBox) { Translate[i][3] = -view_point[i]; }
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
	payload_t* payload = new payload_t();
	data.shader->payload = payload;
	payload->model = data.model;
	payload->World_mat = calculate_model(data.yangle, data.scales, data.translate);
	payload->pLightPos = this->pLightPos;
	payload->pLightColor = this->pLightColor;
	payload->iblMap = this->iblMap;
	gObjects->push_back(data);
	gObjectSize++;
}

void Rasterizer::draw_model(Model* model_data, IShader* shader, Camera* camera)
{
	payload_t* payload = shader->payload;
	bool isSkyBox = model_data->is_skybox;
	update_lookat(camera->eye, camera->target, camera->up, isSkyBox);
	update_projection(this->zNear, this->zFar, this->eye_fov);
	payload->camera = camera;
	payload->ViewProj_mat = ModelViewMat * ProjectionMat;
	payload->pViewPos = camera->eye;

	for (int i = 0; i < model_data->nfaces(); i++) {
		for (int j = 0; j < 3; j++)
			shader->vertex(i, j);

		// homogeneous clipping
		int num_vertex = homo_clipping(shader->payload);

		// triangle assembly and reaterize
		for (int i = 0; i < num_vertex - 2; i++) {
			int index0 = 0;
			int index1 = i + 1;
			int index2 = i + 2;
			// transform data to real vertex attri
			transform_attri(shader->payload, index0, index1, index2);
			draw_triangle(shader->payload, shader);
		}
	}
}

void Rasterizer::rotate_object(float angle, ModelData& data)
{
	data.shader->payload->World_mat = calculate_model(angle, data.scales, data.translate);
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

void Rasterizer::draw_triangle(payload_t* payload, IShader* shader) 
{
	std::vector<vec4> v(3);
	bool isSkyBox = payload->model->is_skybox;

	// 透视除法 ndc坐标
	for (int i = 0; i < 3; i++) 
	{
		v[i].x = payload->ClipPosition[i].x / payload->ClipPosition[i].w;
		v[i].y = payload->ClipPosition[i].y / payload->ClipPosition[i].w;
		v[i].z = payload->ClipPosition[i].z / payload->ClipPosition[i].w;
		v[i].w = payload->ClipPosition[i].w;
	}

	// 屏幕坐标
	for (int i = 0; i < 3; i++)
	{
		v[i].x = 0.5 * (width - 1) * (v[i].x + 1.0);
		v[i].y = 0.5 * (height - 1) * (v[i].y + 1.0);
	}

	if (!isSkyBox)
	{
		if (is_back_facing(v))
			return;
	}

	float x_min = min(v[0].x,min(v[1].x,v[2].x));
	float x_max = max(v[0].x,max(v[1].x,v[2].x));
	float y_min = min(v[0].y,min(v[1].y,v[2].y));
	float y_max = max(v[0].y, max(v[1].y,v[2].y));

	for (int x = x_min; x <= x_max; x++) {
		for (int y = y_min; y <= y_max; y++) {
			glm::vec3 bcCoord = baryCentric(v, x, y);
			if (inside_triangle(bcCoord)) {
				// 便利性计算
				bcCoord.x /= v[0].w; bcCoord.y /= v[1].w; bcCoord.z /= v[2].w;
				float zp = 1.0f / (bcCoord[0] + bcCoord[1] + bcCoord[2]);
				bcCoord *= zp;
				if (shader->payload->model->is_skybox) { zp = -255; }

				if (zp > gDepthBuffer.Sample(x, y)) {
					SetDepth(x, y, zp);
					glm::vec3 color(0.0f);
					shader->fragment(bcCoord, color);
					DrawPixel(x, y, color);
				}
			}
		}
	}
}

void Rasterizer::DrawPixel(int x, int y, glm::vec3& color)
{
	int index = ((height - y - 1) * width + x) * 4;
	for (int i = 0; i < 3; i++)
		framebuffer[index + i] = min(255.0f,color[i]*255.0f);
}

void Rasterizer::SetDepth(int x, int y, float depth)
{
	//y = height - y-1;
	gDepthBuffer.depthBuffer[y][x] = depth;
}

