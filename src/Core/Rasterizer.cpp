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
			gDepthBuffer.depthBuffer[i][j] = -151551515;
		}
	}
}

glm::mat4 Rasterizer::calculate_model(float angle, float scale, const glm::vec3& trans)
{
	glm::mat4 rotation, scaletion, translate;
	angle = angle * MY_PI / 180.0f;

	float rotation_list[16] = { cos(angle), 0,-sin(angle), 0,
											0, 1, 0, 0,
											sin(angle), 0, cos(angle), 0,
											0, 0, 0, 1 };

	rotation = transpose(glm::make_mat4(rotation_list));

	float scaletion_list[16] = { scale, 0, 0, 0,
											0, scale, 0, 0,
											0, 0, scale, 0,
											0, 0, 0, 1 };

	scaletion = transpose(glm::make_mat4(scaletion_list));

	float translate_list[16] = { 1, 0, 0, trans.x,
											0, 1, 0, trans.y,
											0, 0, 1, trans.z,
											0, 0, 0, 1 };

	translate = transpose(glm::make_mat4(translate_list));
	return translate * scaletion * rotation;
}

void Rasterizer::update_view()
{
	float view_list[16] = { 1,0,0,-pViewPoint->x,
									0,1,0,-pViewPoint->y,
									0,0,1,-pViewPoint->z,
									0,0,0,1 };

	this->ViewMat = transpose(glm::make_mat4(view_list));
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

	m = transpose(glm::make_mat4(m_list));
	n = transpose(glm::make_mat4(n_list));
	p = transpose(glm::make_mat4(p_list));
	this->ProjectionMat = n * p * m;
}

void Rasterizer::SetUpEnvironment(EnvData* data)
{
	this->pViewPoint = std::make_shared<glm::vec3>(data->view_point);
	this->pLightPos = std::make_shared<glm::vec3>(data->LightPos);
	this->pLightColor = std::make_shared<glm::vec3>(data->LightColor);
	update_view();
	update_projection(data->zNear, data->zFar, data->eye_fov);
}

void Rasterizer::Add_Object(ModelData data)
{
	IShader* shader = data.shader;
	shader->set_model_data(data.model);
	shader->World_mat = calculate_model(data.yangle, data.scale, data.translate);
	shader->ViewProj_mat = ProjectionMat * ViewMat;
	shader->pLightPos = pLightPos;
	shader->pLightColor = pLightColor;
	shader->pViewPos = pViewPoint;
	gObjects->push_back(data);
	gObjectSize++;
}

void Rasterizer::draw_model(Model* model_data, IShader* shader) 
{
	for (int i = 0; i < model_data->nfaces(); i++) {
		int ClipStateCode = 63;
		std::vector<glm::vec3> coords(3);
		for (int j = 0; j < 3; j++) {
			coords[j] = shader->vertex(i, j);
			int CodeTmp = 0;
			CodeTmp |= (coords[j][0] < -1) << 0;
			CodeTmp |= (coords[j][0] > 1) << 1;
			CodeTmp |= (coords[j][1] < -1) << 2;
			CodeTmp |= (coords[j][1] > 1) << 3;
			CodeTmp |= (coords[j][2] < -1) << 4;
			CodeTmp |= (coords[j][2] > 1) << 5;
			ClipStateCode &= CodeTmp;
		}

		// NDC �ü�
		if (ClipStateCode > 0) { continue; }

		{
			// ����ü�
			glm::vec3  AB = coords[1] - coords[0];
			glm::vec3  AC = coords[2] - coords[0];
			if (AB[0] * AC[1] - AC[0] * AB[1] < 0)
				continue;
		}

		for (int j = 0; j < 3; j++) 
		{
			// ��Ļ����
			coords[j] = glm::vec3((coords[j].x + 1.0f) * (width - 1) / 2, (coords[j].y + 1.0f) * (height - 1) / 2, coords[j].z);
		}

		draw_triangle(coords, shader);
	}
}

glm::vec3 Rasterizer::baryCentric(const std::vector<glm::vec3>& v, float x, float y) const 
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

void Rasterizer::draw_triangle(std::vector<glm::vec3>& v, IShader* shader) 
{
	float x_min = max(0.01f, min(v[0].x,min(v[1].x,v[2].x)));
	float x_max = min(width - 0.01f, max(v[0].x,max(v[1].x,v[2].x)));
	float y_min = max(0.01f, min(v[0].y,min(v[1].y,v[2].y)));
	float y_max = min(height - 0.01f, max(v[0].y, max(v[1].y,v[2].y)));

	for (int x = x_min; x <= x_max; x++) {
		for (int y = y_min; y <= y_max; y++) {
			glm::vec3 bcCoord = baryCentric(v, x, y);
			if (inside_triangle(bcCoord)) {
				float alpha = bcCoord.x, beta = bcCoord.y, gamma = bcCoord.z;
				float zp = 1.0f / (alpha / v[0].z + beta / v[1].z + gamma / v[2].z);

				if (zp > gDepthBuffer.Sample(x, y)) {
					SetDepth(x, y, zp);
					glm::vec3 color = shader->fragment(bcCoord, glm::vec3(v[0].z,v[1].z,v[2].z));
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

