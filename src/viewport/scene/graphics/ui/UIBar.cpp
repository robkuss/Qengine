#include "UIBar.h"


void UIBar::render() const {
	color3f(Colors::UI_COLOR);
	glBegin(GL_QUADS);
	for (const auto vertex : vertices) {
		glVertex2d(vertex.x, vertex.y);
	}
	glEnd();

	color3f(Colors::BLACK);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	for (const auto vertex : vertices) {
		glVertex2d(vertex.x, vertex.y);
	}
	glLineWidth(1.0f);
	glEnd();
}

void UIBar::setVertices(const float windowW, const float windowH) {
	auto absDim = [windowW, windowH](const Dim dim, const int dir) {
		return dim.type == DimType::Percent
			? static_cast<float>(dir == 0 ? windowW : windowH) * dim.value
			: dim.value;
	};

	vertices[0].x = x;
	vertices[0].y = y;

	vertices[1].x = x + absDim(sx, 0);
	vertices[1].y = y;

	vertices[2].x = x + absDim(sx, 0);
	vertices[2].y = y + absDim(sy, 1);

	vertices[3].x = x;
	vertices[3].y = y + absDim(sy, 1);
}
