#pragma once
#include <opencv.hpp>
#include <Eigen/Eigen>
#include <vector>
#include <algorithm>

#define MY_PI 3.1415926

const int width = 800, height = 800;
const int eye_fov = 45;
const float zNear = -0.1f;
const float zFar = -500.0;