#pragma once

#include <memory>

#include "input/Button.h"
#include "UIElement.h"


class UIButtonElement : public UIElement {
public:
	std::shared_ptr<Button> button;	// Button field associated with this element

	UIButtonElement(
		const std::string& label,
		const Dim sx,
		const Dim sy
	) : UIElement(label, sx, sy) {

		// Make the associated Button
		button = std::make_shared<Button>(
			label,
			uiFontSize,
			TextMode::LEFT,
			BUTTON_COLORS,
			sx,
			sy
		);

		button->setActivated(true);
	}

	UIButtonElement(
		const std::string& label,
		const Dim sx,
		const Dim sy,
		const std::shared_ptr<Button>& button
	) : UIElement(label, sx, sy),
		button(button) {

		button->setActivated(true);
	}


	void render(const float xpos, const float ypos) override {
		button->render(xpos, ypos);
	}

	void resizeY(const float value) const {
		sy.value += value;
		button->sy.value += value;
	}

	virtual void checkButtonPressed() const {}
};
