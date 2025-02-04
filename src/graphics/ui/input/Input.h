#pragma once

#include "viewport/scene/SceneManager.h"
#include "graphics/color/Colors.h"


const auto BUTTON_COLORS = vector{
	Colors::BUTTON_REG_FILL,
	Colors::BUTTON_REG_OUTLINE,
	Colors::BUTTON_HIGH_FILL,
	Colors::BUTTON_HIGH_OUTLINE,
	Colors::BUTTON_DEACT_FILL
};

const auto INVIS_BUTTON_COLORS = vector{
	Colors::UI_COLOR_2,
	Colors::UI_COLOR_2,
	Colors::BUTTON_REG_FILL,
	Colors::BUTTON_REG_FILL,
	Colors::BUTTON_DEACT_FILL
};


class Input : public UIElement {
public:
	// Input with text label
	Input(
	 	const string& label,
		const int textSize,
		const TextMode textMode,
		const vector<Color>& colors,
		const Dim sx,
		const Dim sy
	) :   UIElement(label, sx, sy),
		  rf(colors[0]), ro(colors[1]), hf(colors[2]), ho(colors[3]), df(colors[4]),
	      text(label), textSize(textSize), textMode(textMode),
		  activated(true) {}

	// Input with no text label
	Input(
		const vector<Color>& colors,
		const Dim sx,
		const Dim sy
	) :   UIElement("", sx, sy),
		  rf(colors[0]), ro(colors[1]), hf(colors[2]), ho(colors[3]), df(colors[4]),
		  activated(true) {}

	// Default
	explicit Input(
		const vector<Color>& colors
	) :   UIElement(""),
		  rf(colors[0]), ro(colors[1]), hf(colors[2]), ho(colors[3]), df(colors[4]),
		  activated(true) {}

	~Input() override = default;

	[[nodiscard]] bool belowMouse() const {
		if (!activated) return false;
		// >= and < to avoid selecting two neighbors at the same time
		return *SceneManager::mouseX >= x
			&& *SceneManager::mouseX <  x + sx.value
			&& *SceneManager::mouseY >= y
			&& *SceneManager::mouseY <  y + sy.value;
	}

	void render(float xpos, float ypos) override;

	[[nodiscard]] string getText() const {
		return text;
	}

	void setActivated(const bool state) {
		activated = state;
	}

protected:
	Color rf, ro, hf, ho, df;		// Regular, highlight and deactivated Colors (f = fill, o = outline)
	string text;				// Label text for the input
	int textSize{};					// Font size of the text
	TextMode textMode{};			// Left, right, or center
	bool activated{};				// Determines if the input is activated or not
};


inline void Input::render(const float xpos, const float ypos) {
	x = xpos;
	y = ypos;

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
