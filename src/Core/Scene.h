#pragma once
#include "Rasterizer.h"
#include "Shaders.h"

typedef struct
{
	const char* scene_name;
	void (*build_scene)(Rasterizer& r);
} scene_t;

void African_Head_Scene(Rasterizer& r);
void Diablo_Scene(Rasterizer& r);