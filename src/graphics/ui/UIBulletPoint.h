#pragma once

#include "UIButtonElement.h"

class UIBulletPoint final : public UIButtonElement {
public:
	explicit UIBulletPoint(
		const std::string& label
	) : UIButtonElement(label) {}

	void render(const float xpos, const float ypos) override {
		button->render(xpos, ypos);

		glPointSize(5.0f);
		color3f(Colors::TEXT_COLOR);
		glBegin(GL_POINTS);
		glVertex2f(xpos, ypos);
		glEnd();

		Text::renderText(
			label,
			TextMode::LEFT,
			xpos,		// TODO text scalars
			ypos,		// TODO
			24,
			Colors::TEXT_COLOR
		);
	}

	void setOnClickAction(const std::function<void()>& onClickAction) const {
		button->onClick = onClickAction;
	}
};
