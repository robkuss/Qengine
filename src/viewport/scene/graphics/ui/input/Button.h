#pragma once

#include "Input.h"


class Button : public Input {
	TextMode textMode;

public:
	Button(const Scene* scene, const float x, const float y, const float sx, const float sy, const std::string& text, const int textSize, const TextMode textMode, const std::vector<Color>& colors)
		: Input(scene, x, y, sx, sy, text, textSize, colors), textMode(textMode) {}

	void drawButton() {
		drawBox();

		float adjustedX;
		switch (textMode) {
			case 37: // LEFT
				adjustedX = x - sx / 2.0f + static_cast<float>(textSize) / 2.0f;
			break;
			case 39: // RIGHT
				adjustedX = x + sx / 2.0f - static_cast<float>(textSize) / 2.0f;
			break;
			default: // CENTER
				adjustedX = x;
		}

		Text::renderText(
			text,
			textMode,
			adjustedX,
			y,
			textSize,
			getColor()
		);
	}
};