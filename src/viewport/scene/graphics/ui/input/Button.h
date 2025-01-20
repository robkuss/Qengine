#pragma once

#include "Input.h"


class Button final : public Input {
public:
	explicit Button(
		const Scene* scene = nullptr,
		const std::string& text = "",
		const int textSize = 12,
		const TextMode textMode = TextMode::LEFT,
		const std::vector<Color>& colors = BUTTON_COLORS
	) : Input(scene, text, textSize, colors),
		  textMode(textMode) {
	}

	void update() override;
	void render() const override;

private:
	TextMode textMode;
};


inline void Button::update() {

}

inline void Button::render() const {
	Input::render();

	const float sxHalf = sx.value / 2.0f;

	float adjustedX;
	switch (textMode) {
		case TextMode::LEFT:
			adjustedX = x + static_cast<float>(textSize);
		break;
		case TextMode::RIGHT:
			adjustedX = 2 * x - static_cast<float>(textSize);
		break;
		default: // CENTER
			adjustedX = x + sxHalf;
	}

	Text::renderText(
		text,
		textMode,
		adjustedX,
		y + sy.value / 2.0f + static_cast<float>(textSize) / 2.0f + 2.0f,
		Text::fontSize,
		activated ? Colors::BUTTON_TEXT_COLOR : Colors::BUTTON_DEACT_FILL
	);
}
