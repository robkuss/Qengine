#pragma once

#include "UI.h"
#include "UITab.h"

class UIBar final : public UIElement {
public:
	// Constructor
	UIBar(
		std::vector<std::shared_ptr<UITab>> tabs,
		const std::string& label,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy
	) :   UIElement(label, x, y, sx, sy),
		  tabs(std::move(tabs)) {}

	void render() override;
	void setVertices() override;

private:
	std::vector<std::shared_ptr<UITab>> tabs;
};


inline void UIBar::render() {
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

inline void UIBar::setVertices() {
	auto absDim = [this](const Dim dim, const int dir) {
		return dim.type == DimType::Percent
			? static_cast<float>(dir == 0 ? *UI::width : *UI::height) * dim.value
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
