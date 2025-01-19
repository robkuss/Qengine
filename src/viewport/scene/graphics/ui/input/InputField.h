#pragma once

#include "Input.h"


enum class InputMode {
	IDLE,
	TYPING,
	SAVING
};

class InputField : public Input {
	InputMode mode;
	std::string input;

public:
	InputField(const Scene* scene, const float x, const float y, float const sx, float const sy, const std::string& text, const int textSize, const std::vector<Color>& colors)
		: Input(scene, x, y, sx, sy, text, textSize, colors), mode(InputMode::IDLE) {}

	void drawInputField() {
		drawBox();
		glBegin(GL_QUADS);
		glVertex2f(x + 20 - sx / 2, y - sy / 10);
		glVertex2f(x + sx / 4 - sx / 2, y - sy / 10);
		glVertex2f(x + sx / 4 - sx / 2, y + sy / 10);
		glVertex2f(x + 20 - sx / 2, y + sy / 10);
		glEnd();

		Text::renderText(
			text + ":",
			TextMode::LEFT,
			x - sx / 2 + 20,
			y - sy / 8,
			textSize,
			getColor()
		);
		Text::renderText(
			input + (mode == InputMode::TYPING ? "|" : ""),
			TextMode::LEFT,
			x - sx / 2.75f + 30,
			y,
			static_cast<int>(textSize * 1.2),
			getColor()
		);
	}

	[[nodiscard]] std::string getInputRaw() const {
		return input;
	}

	[[nodiscard]] int getInput() const {
		return input.empty() ? 0 : std::stoi(input);
	}

	void resetInput() {
		input.clear();
	}
};
