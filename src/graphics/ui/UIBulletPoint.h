#pragma once

#include "UIButtonElement.h"


class UIBulletPoint final : public UIButtonElement {
public:
	explicit UIBulletPoint(
		const string& label
	) : UIButtonElement(label, INVIS_BUTTON_COLORS) {}

	void render(const float xpos, const float ypos) override {
		button->sx = Dim(static_cast<float>(*UI::width) - xpos - 2 * unit, DimType::Pixels);
		button->sy = UNIT;

		button->render(xpos + unit / 3.0f, ypos - unit / 2.0f);

		glPointSize(5.0f);
		color3f(Colors::TEXT_COLOR);
		glBegin(GL_POINTS);
		glVertex2f(xpos, ypos);
		glEnd();

		Text::renderText(
			label,
			TextMode::LEFT,
			xpos + unit / 2.0f,
			ypos + 6.5f,
			24,
			Colors::TEXT_COLOR
		);
	}

	void setOnClickAction(const function<void()>& onClickAction) const {
		button->onClick = onClickAction;
	}
};
