#pragma once

#include "UI.h"
#include "graphics/color/Colors.h"


class UIBar final : public UIElement {
public:
	// Constructor
	UIBar(
		vector<shared_ptr<UIOptionList>> tabs,
		const string& label,
		const Dim sx,
		const Dim sy
	) :   UIElement(label, sx, sy),
		  tabs(move(tabs)) {}

	void render(float xpos, float ypos) override;

private:
	vector<shared_ptr<UIOptionList>> tabs;
};


inline void UIBar::render(const float xpos, const float ypos) {
	auto absDim = [this](const Dim dim, const int dir) {
		return dim.type == DimType::Percent
			? static_cast<float>(dir == 0 ? *UI::width : *UI::height) * dim.value
			: dim.value;
	};

	vertices[0].x = xpos;
	vertices[0].y = ypos;

	vertices[1].x = xpos + absDim(sx, 0);
	vertices[1].y = ypos;

	vertices[2].x = xpos + absDim(sx, 0);
	vertices[2].y = ypos + absDim(sy, 1);

	vertices[3].x = xpos;
	vertices[3].y = ypos + absDim(sy, 1);

	color3f(Colors::UI_COLOR);
	glBegin(GL_QUADS);
	for (const auto vertex : vertices) {
		glVertex2d(vertex.x, vertex.y);
	}
	glEnd();

	color3f(Colors::UI_OUTLINE_COLOR);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	for (const auto vertex : vertices) {
		glVertex2d(vertex.x, vertex.y);
	}
	glLineWidth(1.0f);
	glEnd();
}
