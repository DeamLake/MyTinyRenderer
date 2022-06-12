#include "src/Core/Rasterizer.h"
#include "src/Platform/Utils.h"
#include<ctime>

using namespace Eigen;

Eigen::Vector3f DefaultColor(255, 255, 255);
Eigen::Vector3f ViewPoint(0, 0, 3);

int main() {
	clock_t cur_time = clock();
	Rasterizer r(width, height,eye_fov,zNear,zFar,ViewPoint);
	Model* model = new Model("Resource/obj/african_head.obj");
	IShader* shader = new GouraudShader();
	int key = 0;
	
	while (key != 27) {
		r.clear(Buffers::Color | Buffers::Depth);
		r.draw_model(model, shader);
		r.show();
		std::cout << "Show time: " << clock() - cur_time << std::endl;
		cur_time = clock();
		key = cv::waitKey(10);
	}
}