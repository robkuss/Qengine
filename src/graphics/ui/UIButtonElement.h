#pragma once

#include <memory>

#include "input/Button.h"
#include "UIElement.h"


class UIButtonElement : public UIElement {
public:
	shared_ptr<Button> button;	// Button field associated with this element

	// UIButtonElement with default button and dimensions
	UIButtonElement(
		const string& label,
		const Dim sx,
		const Dim sy
	) : UIElement(label, sx, sy) {

		// Make the associated Button
		button = make_shared<Button>(
			label,
			uiFontSize,
			TextMode::LEFT,
			BUTTON_COLORS,
			sx,
			sy
		);
	}

	// UIButtonElement with explicit button and dimensions
	UIButtonElement(
		const string& label,
		const Dim sx,
		const Dim sy,
		const shared_ptr<Button>& button
	) : UIElement(label, sx, sy),
		button(button) {}

	// UIButtonElement without explicit button or dimensions
	explicit UIButtonElement(
		const string& label,
		const vector<Color>& colors
	) : UIElement(label),
		button(make_shared<Button>(colors)) {}


	void render(const float xpos, const float ypos) override {
		button->render(xpos, ypos);
	}

	void resizeY(const float value) const {
		sy.value += value;
		button->sy.value += value;
	}

	virtual void checkButtonPressed() const {}
};
