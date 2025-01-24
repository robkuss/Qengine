#pragma once

#include <memory>

#include "input/Button.h"
#include "UIElement.h"


class UIButtonElement : public UIElement {
public:
	std::shared_ptr<Button> button;	// Button field associated with this element

	UIButtonElement(
		const std::string& label,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy
	) :   UIElement(label, x, y, sx, sy) {

		// Make the associated Button
		button = std::make_shared<Button>(
			label,
			uiFontSize,
			TextMode::LEFT,
			BUTTON_COLORS,
			x, y, sx, sy
		);

		button->vertices.resize(vertexCount);
		button->setVertices();
		button->setActivated(true);
	}

	void render() override {
		button->render();
	}

	void setVertices() override {}

	void resizeY(const float value) {
		sy.value += value;
		setVertices();

		button->sy.value += value;
		button->setVertices();
	}

	virtual void checkButtonPressed() const {}
};
