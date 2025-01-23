#pragma once

#include "Input.h"


class Button final : public Input {
public:
	Button(
		const std::string& label,
		const int textSize,
		const TextMode textMode,
		const std::vector<Color>& colors,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy
	) : Input(label, textSize, textMode, colors, x, y, sx, sy) {}

	void render() override;
};


inline void Button::render() {
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
