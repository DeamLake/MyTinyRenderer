#include "src/Core/Rasterizer.h"
#include "src/Resource/model.h"
#include "src/Platform/Utils.h"

using namespace Eigen;
Vector3f DefaultColor(255, 255, 255);

void main() {
	Rasterizer r(width, height);
	Model* model = new Model("Resource/obj/african_head.obj");
	r.clear(Buffers::Color | Buffers::Depth);
	r.set_model(0.0, 1);
	r.set_view(Vector3f(1, 1, 5));
	r.set_projection(45, width / height, -0.1, -500);
	r.draw_model(model, DefaultColor);
	int key = 0;
	while (key != 27) {
		r.show();
		key = cv::waitKey(10);
	}
}