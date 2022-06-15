#pragma once
#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "tgaimage.h"
typedef glm::vec3 Vec3f;
typedef glm::vec2 Vec2f;
typedef glm::ivec3 Vec3i;
typedef glm::ivec2 Vec2i;

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<Vec3i> > faces_; // attention, this Vec3i means vertex/uv/normal
    std::vector<Vec3f> norms_;
    std::vector<Vec2f> uv_;
    TGAImage diffusemap_;
    void load_texture(std::string filename, const char* suffix, TGAImage& img);
public:
    Model(const char* filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f norm(int iface, int nvert);
    Vec3f vert(int i);
    Vec2i uv(int iface, int nvert);
    TGAColor diffuse(Vec2i uv);
    std::vector<int> face(int idx);
};


