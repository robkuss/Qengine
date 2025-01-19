#include "UIBar.h"

#include <viewport/scene/graphics/color/Colors.h>


void UIBar::setVertices(const Vector2 startPos, const Dim length, const Dim width) {
	auto absDim = [](const Dim dim, const int dir) {
		return dim.type == DimType::Percent
			? static_cast<float>(dir == 0 ? (*vp)[2] : (*vp)[3]) * dim.value
			: dim.value;
	};

	vertices[0].x = startPos.x;
	vertices[0].y = startPos.y;

	vertices[1].x = startPos.x + absDim(length, 0);
	vertices[1].y = startPos.y;

	vertices[2].x = startPos.x + absDim(length, 0);
	vertices[2].y = startPos.y + absDim(width, 0);

	vertices[3].x = startPos.x;
	vertices[3].y = startPos.y + absDim(width, 1);
}

void UIBar::update() {
	setVertices(startPos, length, width);
}

void UIBar::render() const {
	// Draw the filled rectangle
	color3f(Colors::UI_COLOR);
	glBegin(GL_QUADS);
	for (const auto vertex : vertices) {
		glVertex2d(vertex.x, vertex.y);
	}
	glEnd();

	// Draw the outline rectangle
	color3f(Colors::BLACK);
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);
	for (const auto vertex : vertices) {
		glVertex2d(vertex.x, vertex.y);
	}
	glEnd();

	// Render the tabs
	for (const auto& tab : tabs) {
		tab->render();
	}
}