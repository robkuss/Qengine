#pragma once

#include "Input.h"


enum class InputMode {
	IDLE,
	TYPING,
	SAVING
};

class InputField final : public Input {
public:
	explicit InputField(
		const Scene* scene = nullptr,
		const std::string& text = "",
		const int textSize = 12,
		const std::vector<Color>& colors = BUTTON_COLORS
	) : Input(scene, text, textSize, colors),
		  mode(InputMode::IDLE) {
	}

	void update() override;
	void render() const override;

	[[nodiscard]] std::string getInputRaw() const {
		return input;
	}

	[[nodiscard]] int getInput() const {
		return input.empty() ? 0 : std::stoi(input);
	}

	void resetInput() {
		input.clear();
	}

private:
	InputMode mode;
	std::string input;
};


inline void InputField::update() override {

}

inline void InputField::render() const override {
	Input::render();

	const float sxHalf = sx.value / 2.0f;
	const float syTenths = sy.value / 10.0f;

	glBegin(GL_QUADS);
	glVertex2f(x + 20 - sxHalf, y - syTenths);
	glVertex2f(x + sx.value / 4 - sxHalf, y - syTenths);
	glVertex2f(x + sx.value / 4 - sxHalf, y + syTenths);
	glVertex2f(x + 20 - sxHalf, y + syTenths);
	glEnd();

	Text::renderText(
		text + ":",
		TextMode::LEFT,
		x - sxHalf + 20,
		y - sy.value / 8,
		textSize,
		activated ? Colors::BLACK : Colors::BUTTON_DEACT_FILL
	);

	Text::renderText(
		input + (mode == InputMode::TYPING ? "|" : ""),
		TextMode::LEFT,
		x - sx.value / 2.75f + 30,
		y,
		static_cast<int>(textSize * 1.2),
		activated ? Colors::BLACK : Colors::BUTTON_DEACT_FILL
	);
}
