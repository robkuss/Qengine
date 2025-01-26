#pragma once

#include "UIButtonElement.h"

class UIBulletPoint final : public UIButtonElement {
public:
	UIBulletPoint(
		const std::string& label,
		const Dim sx,
		const Dim sy,
		const float textXScalar,
		const float textYScalar
	) : UIButtonElement(label, sx, sy/*,
		std::make_shared<Button>(
			BUTTON_COLORS,
			x + 15.0f,
			y - unit/2.0f,
			sx - 30.0f,
			sy
		)*/),
		textXScalar(textXScalar),
		textYScalar(textYScalar) {}

	void render(float xpos, float ypos) override;

private:
	const float textXScalar;
	const float textYScalar;
};

inline void UIBulletPoint::render(const float xpos, const float ypos) {
	button->render(xpos, ypos);

	glPointSize(5.0f);
	color3f(Colors::TEXT_COLOR);
	glBegin(GL_POINTS);
	glVertex2f(xpos, ypos);
	glEnd();

	Text::renderText(
		label,
		TextMode::LEFT,
		xpos + textXScalar * unit,
		ypos + textYScalar,
		24,
		Colors::TEXT_COLOR
	);
}