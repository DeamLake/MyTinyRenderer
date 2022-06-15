#include "IShader.h"

vec3 IShader::mvp_translate(vec3& p, mat4x4& mat) 
{
	vec4 v(p, 1);
	v = mat * v;
	v /= v.w;
	return vec3(v.x,v.y,v.z);
}