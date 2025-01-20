#pragma once

#include <iostream>

#include "../UI.h"

#include "viewport/scene/Scene.h"

#include <utility>

const auto BUTTON_COLORS = std::vector{
	Colors::BUTTON_REG_FILL,
	Colors::BUTTON_REG_OUTLINE,
	Colors::BUTTON_HIGH_FILL,
	Colors::BUTTON_HIGH_OUTLINE,
	Colors::BUTTON_DEACT_FILL
};


class Input : public UIElement {
public:
	explicit Input(
		const Scene* scene = nullptr,
		std::string text = "",
		const int textSize = 48,
		const std::vector<Color>& colors = BUTTON_COLORS
	) :   scene(scene),
		  rf(colors[0]), ro(colors[1]), hf(colors[2]), ho(colors[3]), df(colors[4]),
		  text(std::move(text)), textSize(textSize), activated(true) {
	}

	~Input() override = default;

	[[nodiscard]] bool below(const double mouseX, const double mouseY) const {
		if (!activated) return false;
		return mouseX >= x
			&& mouseX <= x + sx.value
			&& mouseY >= y
			&& mouseY <= y + sy.value;
	}

	void update() override;
	void render() const override;
	void setVertices() override;

	[[nodiscard]] std::string getText() const {
		return text;
	}

	void setActivated(const bool state) {
		activated = state;
	}

protected:
	const Scene* scene;

	Color rf, ro, hf, ho, df;   // Regular, highlight and deactivated Colors (f = fill, o = outline)
	std::string text;			// Label text for the input
	int textSize;				// Font size of the text
	bool activated;				// Determines if the input is activated or not
};


inline void Input::update() {

}

inline void Input::render() const {
	glPushAttrib(GL_LINE_BIT | GL_COLOR_BUFFER_BIT);	// Save line width and color state

	const auto isBelowMouse = below(*scene->mouseX, *scene->mouseY);

	// Draw the filled rectangle
	if (isBelowMouse) {
		color3f(hf);
	} else {
		color3f(rf);
	}
	glBegin(GL_QUADS);
	for (const auto vertex : vertices) {
		glVertex2d(vertex.x, vertex.y);
	}
	glEnd();

	// Draw the outline rectangle
	if (isBelowMouse) {
		color3f(ho);
	} else {
		color3f(ro);
	}
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	for (const auto vertex : vertices) {
		glVertex2d(vertex.x, vertex.y);
	}
	glLineWidth(1.0f);
	glEnd();

	glPopAttrib();	// Restore line width and color state
}

inline void Input::setVertices() {
	auto absDim = [](const Dim dim, const int dir) {
		return dim.type == DimType::Percent
			? static_cast<float>(dir == 0 ? (*vp)[2] : (*vp)[3]) * dim.value
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
