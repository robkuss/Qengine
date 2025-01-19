#pragma once

#include "../UI.h"

#include <viewport/scene/Scene.h>


class Input : public UIElement {
public:
	Input(const Scene* scene, const float x, const float y, const float sx, const float sy, std::string text, const int textSize, const std::vector<Color>& colors)
		: scene(scene), x(x), y(y), sx(sx), sy(sy), c(colors[0]), h(colors[1]), d(colors[2]), text(std::move(text)), textSize(textSize), activated(true) {}

	~Input() override = default;

	virtual bool below(const double mouseX, const double mouseY) {
		if (!activated) return false;
		return mouseX >= x - sx / 2
			&& mouseX <= x + sx / 2
			&& mouseY >= y - sy / 2
			&& mouseY <= y + sy / 2;
	}

	virtual void drawBox() {
		const bool isBelow = below(scene->lastMousePos.x, scene->lastMousePos.y);
		glLineWidth(3.0f);
		color3f(getColor());
		glBegin(GL_QUADS);
		if (isBelow) {
			color3f(h);
		} else {
			color3f(c);
		}
		glVertex2f(x - sx / 2, y - sy / 2);
		glVertex2f(x + sx / 2, y - sy / 2);
		glVertex2f(x + sx / 2, y + sy / 2);
		glVertex2f(x - sx / 2, y + sy / 2);
		glEnd();
	}

	[[nodiscard]] std::string getText() const {
		return text;
	}

	void setActivated(const bool state) {
		activated = state;
	}

	[[nodiscard]] Color getColor() const {
		return activated ? Colors::BLACK : d;
	}

protected:
	const Scene* scene;

	float x, y;               // Coordinates
	float sx, sy;             // Dimensions
	Color c, h, d;            // Base, highlight, and deactivated color
	std::string text;         // Label text for the input
	int textSize;             // Font size of the text
	bool activated;           // Determines if the input is activated or not
};
