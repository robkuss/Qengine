#pragma once

#include "Input.h"


class Button final : public Input {
public:
	explicit Button(
		const std::string& label = "",
		const int textSize = 48,
		const TextMode textMode = TextMode::LEFT,
		const std::vector<Color>& colors = BUTTON_COLORS
	) : Input(label, textSize, textMode, colors) {}

	void render() const override;
};


inline void Button::render() const {
	Input::render();

	float adjustedX;
	switch (textMode) {
		case TextMode::LEFT:
			adjustedX = x + static_cast<float>(textSize) / 2.0f;
		break;
		case TextMode::RIGHT:
			adjustedX = x + sx.value - static_cast<float>(textSize) / 2.0f;
		break;
		default: // CENTER
			adjustedX = x + sx.value / 2.0f;
	}

	Text::renderText(
		text,
		textMode,
		adjustedX,
		y + sy.value / 2.0f + static_cast<float>(textSize) / 3.0f,
		textSize,
		activated ? Colors::BUTTON_TEXT_COLOR : Colors::BUTTON_DEACT_FILL
	);
}
