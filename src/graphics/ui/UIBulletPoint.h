#pragma once

#include "UIButtonElement.h"

class UIBulletPoint final : UIButtonElement {
public:
	UIBulletPoint(
		const std::string& label,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy,
		const float textX,
		const float textY
	) : UIButtonElement(label, x, y, sx, sy),
		textX(textX),
		textY(textY) {}

	void render() override;

private:
	const float textX;
	const float textY;
};

inline void UIBulletPoint::render() {
	glPointSize(5.0f);
	color3f(Colors::TEXT_COLOR);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
	Text::renderText(label, TextMode::LEFT, textX, textY, 24, Colors::TEXT_COLOR);
}