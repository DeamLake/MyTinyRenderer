#pragma once
#include <windows.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp> // make
#include<memory>
#include "Buffer.h"
#include "IShader.h"
#include "Config.h"

#define MY_PI 3.1415926

class Rasterizer
{
public:
	Rasterizer(int w, int h);
	~Rasterizer() {}

public:
	void ClearDepth();
	void SetHDC(HDC hdc) { gScreenHdc = hdc; }
	glm::mat4 calculate_model(float angle, float scale, const glm::vec3& trans);
	void update_view();
	void update_projection(float zNear, float zFar, float eye_fov);

	void SetUpEnvironment(EnvData* data);
	void Add_Object(ModelData data);
	void draw_model(Model* model_data, IShader* shader);

	ModelData getNthObject(int idx) const { return (*gObjects)[idx]; }
	int getSizeOfObject() const { return gObjectSize; }

private:
	// 内部函数
	glm::vec3 baryCentric(const std::vector<glm::vec3>& v, float x, float y) const;
	bool inside_triangle(glm::vec3& bcCoord) const;
	void draw_triangle(std::vector<glm::vec3>& v, IShader* shader);
	void DrawPixel(int x, int y, glm::vec3& color);
	void SetDepth(int x, int y, float depth);

private:
	// 内部变量
	int width, height;
	HDC gScreenHdc;
	DepthBuffer gDepthBuffer;
	
	glm::mat4 ViewMat, ProjectionMat;
	std::shared_ptr<glm::vec3> pViewPoint, pLightPos, pLightColor;
	std::shared_ptr <std::vector<ModelData>> gObjects = std::make_shared<std::vector<ModelData>>();
	int gObjectSize = 0;
};