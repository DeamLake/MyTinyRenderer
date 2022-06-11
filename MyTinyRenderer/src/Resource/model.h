#pragma once
#include <vector>
#include <Eigen/Eigen>
#include "tgaimage.h"

class Model {
private:
    std::vector<Eigen::Vector3f> verts_;
    std::vector<std::vector<Eigen::Vector3i> > faces_; // attention, this Vec3i means vertex/uv/normal
    std::vector<Eigen::Vector3f> norms_;
    std::vector<Eigen::Vector2f> uv_;
    TGAImage diffusemap_;
    void load_texture(std::string filename, const char* suffix, TGAImage& img);
public:
    Model(const char* filename);
    ~Model();
    int nverts();
    int nfaces();
    Eigen::Vector3f norm(int iface, int nvert);
    Eigen::Vector3f vert(int i);
    Eigen::Vector2i uv(int iface, int nvert);
    TGAColor diffuse(Eigen::Vector2i uv);
    std::vector<int> face(int idx);
};