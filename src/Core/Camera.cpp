#include "Camera.h"

Camera::Camera(vec3 e, vec3 t, vec3 up, float aspect) :
	eye(e), target(t), up(up), aspect(aspect)
{}

void Camera::updata_camera_pos()
{
	mouse_t* mouse = WinApp::GetApp()->GetMouse();
	int width = WinApp::GetApp()->GetWidth();
	int height = WinApp::GetApp()->GetHeight();
	vec3 from_target = eye - target;			// vector point from target to camera's position
	float radius = sqrt(from_target.x * from_target.x + from_target.y * from_target.y + from_target.z * from_target.z);

	float phi = (float)atan2(from_target[0], from_target[2]); // azimuth angle(·½Î»½Ç), angle between from_target and z-axis£¬[-pi, pi]
	float theta = (float)acos(from_target[1] / radius);		  // zenith angle(Ìì¶¥½Ç), angle between from_target and y-axis, [0, pi]
	float x_delta = mouse->orbit_delta[0] / width;
	float y_delta = mouse->orbit_delta[1] / height;

	// for mouse wheel
	radius *= (float)pow(0.95, mouse->wheel_delta);

	float factor = 1.5 * PI;
	// for mouse left button
	phi += x_delta * factor;
	theta += y_delta * factor;
	if (theta > PI) theta = PI - EPSILON * 100;
	if (theta < 0)  theta = EPSILON * 100;

	eye[0] = target[0] + radius * sin(phi) * sin(theta);
	eye[1] = target[1] + radius * cos(theta);
	eye[2] = target[2] + radius * sin(theta) * cos(phi);

	// for mouse right button
	factor = radius * (float)tan(60.0 / 360 * PI) * 2.2;
	x_delta = mouse->fv_delta[0] / width;
	y_delta = mouse->fv_delta[1] / height;
	vec3 left = x_delta * factor * x;
	vec3 up = y_delta * factor * y;

	eye += (left - up);
	target += (left - up);
}

void Camera::handle_mouse_events()
{
	mouse_t* mouse = WinApp::GetApp()->GetMouse();
	if (WinApp::GetApp()->buttons[0])
	{
		vec2 cur_pos = get_mouse_pos();
		mouse->orbit_delta = mouse->orbit_pos - cur_pos;
		mouse->orbit_pos = cur_pos;
	}

	if (WinApp::GetApp()->buttons[1])
	{
		vec2 cur_pos = get_mouse_pos();
		mouse->fv_delta = mouse->fv_pos - cur_pos;
		mouse->fv_pos = cur_pos;
	}

	updata_camera_pos();
}

void Camera::handle_key_events()
{
	vec3 tmp = target - eye;
	float distance = sqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
	char* keys = WinApp::GetApp()->keys;

	if (keys['W'])
	{
		eye += float(-10.0 / WinApp::GetApp()->GetWidth() * distance) * z;
	}
	if (keys['S'])
	{
		eye += 0.05f * z;
	}
	if (keys[VK_UP] || keys['Q'])
	{
		eye += 0.05f * y;
		target += 0.05f * y;
	}
	if (keys[VK_DOWN] || keys['E'])
	{
		eye += -0.05f * y;
		target += -0.05f * y;
	}
	if (keys[VK_LEFT] || keys['A'])
	{
		eye += -0.05f * x;
		target += -0.05f * x;
	}
	if (keys[VK_RIGHT] || keys['D'])
	{
		eye += 0.05f * x;
		target += 0.05f * x;
	}
	if (keys[VK_ESCAPE])
	{
		WinApp::GetApp()->isClose = 1;
	}
}

void Camera::handle_events()
{
	//calculate camera axis
	z = normalize(eye - target);
	x = normalize(cross(up, z));
	y = normalize(cross(z, x));

	//mouse and keyboard events
	handle_mouse_events();
	handle_key_events();
}