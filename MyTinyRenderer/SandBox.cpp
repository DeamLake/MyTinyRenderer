#include "src/Core/Rasterizer.h"
#include "src/Resource/model.h"
using namespace Eigen;

const int width = 800, height = 800;
Vector3f Color(255, 255, 255);

void Draw_Model(Rasterizer& r) {
	Model* model = new Model("Resource/obj/african_head.obj");
	std::cout << model->nfaces() << std::endl;
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++) {
			Vector3f v0 = model->vert(face[j]);
			Vector3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x() + 1.0) * (width - 1) / 2;
			int y0 = (v0.y() + 1.0) * (height - 1) / 2;
			int x1 = (v1.x() + 1.0) * (width - 1) / 2;
			int y1 = (v1.y() + 1.0) * (height - 1) / 2;

			r.draw_line(Vector2f(x0, y0), Vector2f(x1, y1), Color);
		}
	}
}

void main() {
	Rasterizer r(width, height);
	Draw_Model(r);
	int key = 0;
	while (key != 27) {
		r.show();
		key = cv::waitKey(10);
	}
}