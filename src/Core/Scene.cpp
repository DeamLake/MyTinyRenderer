#include "Scene.h"

TGAImage* texture_from_file(const char* file_name)
{
	TGAImage* texture = new TGAImage();
	texture->read_tga_file(file_name);
	texture->flip_vertically();
	return texture;
}

cubemap_t* cubemap_from_files(const char* positive_x, const char* negative_x,
	const char* positive_y, const char* negative_y,
	const char* positive_z, const char* negative_z)
{
	cubemap_t* cubemap = new cubemap_t();
	cubemap->faces[0] = texture_from_file(positive_x);
	cubemap->faces[1] = texture_from_file(negative_x);
	cubemap->faces[2] = texture_from_file(positive_y);
	cubemap->faces[3] = texture_from_file(negative_y);
	cubemap->faces[4] = texture_from_file(positive_z);
	cubemap->faces[5] = texture_from_file(negative_z);
	return cubemap;
}

void load_ibl_map(Rasterizer& r, const char* env_path)
{
	int i, j;
	iblmap_t* iblmap = new iblmap_t();
	const char* faces[6] = { "px", "nx", "py", "ny", "pz", "nz" };
	char paths[6][256];

	iblmap->mip_levels = 10;

	/* diffuse environment map */
	for (j = 0; j < 6; j++) {
		sprintf_s(paths[j], "%s/i_%s.tga", env_path, faces[j]);
	}
	iblmap->irradiance_map = cubemap_from_files(paths[0], paths[1], paths[2],
		paths[3], paths[4], paths[5]);

	/* specular environment maps */
	for (i = 0; i < iblmap->mip_levels; i++) {
		for (j = 0; j < 6; j++) {
			sprintf_s(paths[j], "%s/m%d_%s.tga", env_path, i, faces[j]);
		}
		iblmap->prefilter_maps[i] = cubemap_from_files(paths[0], paths[1],
			paths[2], paths[3], paths[4], paths[5]);
	}

	/* brdf lookup texture */
	iblmap->brdf_lut = texture_from_file("../obj/common/BRDF_LUT.tga");

	r.SetMap(iblmap);
}

void African_Head_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/african_head/african_head.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0, 0);
	modelData.scales = glm::vec3(1, 1, 1);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/african_head/african_head_eye_inner.obj",0);
	modelData.shader = new GouraudShader();
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/skybox4/box.obj", 1);
	modelData.shader = new SkyBoxShader();
	modelData.scales = glm::vec3(2, 2, 2);
	modelData.rotate = false;
	r.Add_Object(modelData);
	
	/*ModelData modelData;
	modelData.model = new Model("../Resources/skybox4/box.obj", 1);
	modelData.shader = new SkyBoxShader();
	modelData.translate = glm::vec3(0, 0, 0);
	modelData.scales = glm::vec3(2, 2, 2);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);*/
}

void Diablo_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/diablo/diablo3_pose.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0, 0);
	modelData.scales = glm::vec3(1, 1, 1);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);
}

void Helmat_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/helmet/helmet.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0, -1);
	modelData.scales = glm::vec3(1.0f, 1.0f, 1.0f);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/skybox4/box.obj", 1);
	modelData.shader = new SkyBoxShader();
	modelData.translate = glm::vec3(0, 0, 0);
	modelData.scales = glm::vec3(2, 2, 2);
	modelData.rotate = false;
	r.Add_Object(modelData);
}

void Fuhua_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/fuhua/fuhuabody.obj");
	modelData.shader = new GouraudShader();
	modelData.translate = glm::vec3(0, 0, 0);
	modelData.scales = glm::vec3(1, 1, 1);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/fuhua/fuhuacloak.obj");
	modelData.shader = new GouraudShader();
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/fuhua/fuhuaface.obj");
	modelData.shader = new GouraudShader();
	r.Add_Object(modelData);

	modelData.model = new Model("../Resources/fuhua/fuhuahair.obj");
	modelData.shader = new GouraudShader();
	r.Add_Object(modelData);
}

void Floor_Scene(Rasterizer& r)
{
	ModelData modelData;
	modelData.model = new Model("../Resources/floor.obj");
	modelData.shader = new BlingPhongShader();
	modelData.translate = glm::vec3(0, 0, -1);
	modelData.scales = glm::vec3(2.0f, 2.0f, 2.0f);
	modelData.yangle = 0.0f;
	r.Add_Object(modelData);
}