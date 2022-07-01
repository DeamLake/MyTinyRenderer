#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "IShader.h"

int homo_clipping(payload_t* payload);
void transform_attri(payload_t* payload, int index0, int index1, int index2);
bool is_back_facing(std::vector<vec4>& ndc_pos);