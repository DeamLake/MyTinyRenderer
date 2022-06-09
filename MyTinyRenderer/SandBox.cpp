#include "src/Core/Rasterizer.h"
#include "src/Resource/model.h"
#include "src/Platform/Utils.h"

using namespace Eigen;
Vector3f DefaultColor(255, 255, 255);

void main() {
	Rasterizer r(width, height);
	r.clear(Buffers::Color | Buffers::Depth);
	Model* model = new Model("Resource/obj/african_head.obj");
	r.draw_model(model, DefaultColor);
	int key = 0;
	while (key != 27) {
		r.show();
		key = cv::waitKey(10);
	}
}