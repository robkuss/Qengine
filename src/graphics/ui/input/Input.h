#pragma once

#include <graphics/ui/UI.h>


const auto BUTTON_COLORS = std::vector{
	Colors::BUTTON_REG_FILL,
	Colors::BUTTON_REG_OUTLINE,
	Colors::BUTTON_HIGH_FILL,
	Colors::BUTTON_HIGH_OUTLINE,
	Colors::BUTTON_DEACT_FILL
};


class Input : public UIElement {
public:
	Input(
	 	const std::string& label,
		const int textSize,
		const TextMode textMode,
		const std::vector<Color>& colors,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy
	) :   UIElement(label, x, y, sx, sy),
		  rf(colors[0]), ro(colors[1]), hf(colors[2]), ho(colors[3]), df(colors[4]),
	      text(label), textSize(textSize), textMode(textMode), activated(true) {}

	~Input() override = default;

	[[nodiscard]] bool belowMouse() const {
		if (!activated) return false;
		// >= and < to avoid selecting two neighbors at the same time
		return *SceneManager::mouseX >= x
			&& *SceneManager::mouseX < x + sx.value
			&& *SceneManager::mouseY >= y
			&& *SceneManager::mouseY < y + sy.value;
	}

	void render() override;
	void setVertices() override;

	[[nodiscard]] std::string getText() const {
		return text;
	}

	void setActivated(const bool state) {
		activated = state;
	}

protected:
	Color rf, ro, hf, ho, df;		// Regular, highlight and deactivated Colors (f = fill, o = outline)
	std::string text;				// Label text for the input
	int textSize;					// Font size of the text
	TextMode textMode;				// Left, right, or center
	bool activated;					// Determines if the input is activated or not
};


inline void Input::render() {
	glPushAttrib(GL_LINE_BIT | GL_COLOR_BUFFER_BIT);	// Save line width and color state

	const auto isBelowMouse = belowMouse();

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
