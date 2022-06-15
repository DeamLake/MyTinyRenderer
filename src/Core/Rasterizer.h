#pragma once
#include <windows.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp> // make
#include "Buffer.h"
#include "IShader.h"

#define MY_PI 3.1415926

class Rasterizer
{
public:
	Rasterizer(int w, int h);
	~Rasterizer() {}

public:
	glm::mat4 Model_mat, View_mat, Projection_mat;
public:
	void ClearDepth();
	void SetHDC(HDC hdc) { gScreenHdc = hdc; }
	void set_model(float angle, float scale);
	void set_view(const glm::vec3& view_point);
	void set_projection(float zNear, float zFar, float eye_fov);

	void draw_model(Model* model_data, IShader* shader);
	glm::vec3 baryCentric(const std::vector<glm::vec3>& v, float x, float y) const;
	bool inside_triangle(glm::vec3& bcCoord) const;
	void draw_triangle(std::vector<glm::vec3>& v, IShader* shader);

private:
	// 内部函数
	void DrawPixel(int x, int y, glm::vec3& color);
	void SetDepth(int x, int y, float depth);

private:
	// 内部变量
	int width, height;
	HDC gScreenHdc;
	DepthBuffer gDepthBuffer;
	
};