#pragma once

#include "UI.h"
#include "UIElement.h"
#include "graphics/color/Colors.h"


class UIWindow final : public UIElement {
public:
	UIWindow(
		const string& label,
		const Dim sx,
		const Dim sy
	) :	  UIElement(label, sx, sy) {}


	void render(const float xpos, const float ypos) override {
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

		glPushAttrib(GL_LINE_BIT | GL_COLOR_BUFFER_BIT);	// Save line width and color state

		// Draw the filled rectangle
		color3f(Colors::UI_COLOR_2);
		glBegin(GL_QUADS);
		for (const auto vertex : vertices) {
			glVertex2d(vertex.x, vertex.y);
		}
		glEnd();

		// Draw the outline rectangle
		color3f(Colors::UI_OUTLINE_COLOR);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		for (const auto vertex : vertices) {
			glVertex2d(vertex.x, vertex.y);
		}
		glLineWidth(1.0f);
		glEnd();

		glPopAttrib();	// Restore line width and color state
	}
};