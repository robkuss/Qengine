#pragma once

#include <functional>

#include "Input.h"


class Button final : public Input {
public:
	// The function that is called when the button is clicked
	std::function<void()> onClick;

	// Button with text label
	Button(
		const std::string& label,
		const int textSize,
		const TextMode textMode,
		const std::vector<Color>& colors,
		const Dim sx,
		const Dim sy
	) : Input(label, textSize, textMode, colors, sx, sy) {}

	// Button with no text label
	Button(
		const std::vector<Color>& colors,
		const Dim sx,
		const Dim sy
	) : Input(colors, sx, sy) {}

	// Default
	Button() : Input(BUTTON_COLORS) {}


	void render(float xpos, float ypos) override;

	void click() const {
		onClick();
	}

	bool setOnClickForButton(
		std::shared_ptr<UIOptionList> list,
		const std::string& label,
		const std::function<void()>& onClickAction
	);
};


inline void Button::render(const float xpos, const float ypos) {
	Input::render(xpos, ypos);

	float adjustedX;
	switch (textMode) {
		case TextMode::LEFT:
			adjustedX = xpos + static_cast<float>(textSize) / 2.0f;
		break;
		case TextMode::RIGHT:
			adjustedX = xpos + sx.value - static_cast<float>(textSize) / 2.0f;
		break;
		default: // CENTER
			adjustedX = xpos + sx.value / 2.0f;
	}

	Text::renderText(
		text,
		textMode,
		adjustedX,
		ypos + sy.value / 2.0f + static_cast<float>(textSize) / 3.0f,
		textSize,
		activated ? Colors::BUTTON_TEXT_COLOR : Colors::BUTTON_DEACT_FILL
	);
}
