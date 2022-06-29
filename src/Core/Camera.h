#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "WinApp.h"
using namespace glm;

class Camera
{
public:
	Camera(vec3 e, vec3 t, vec3 up, float aspect);
	~Camera() {}

	//handle event
	void updata_camera_pos();
	void handle_events();
	void handle_mouse_events();
	void handle_key_events();

	vec3 eye, target, up;
	vec3 x, y, z;
	float aspect;
};
