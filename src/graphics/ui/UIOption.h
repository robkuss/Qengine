#pragma once

#include <algorithm>
#include <string>

#include "UIButtonElement.h"


/** A UIOption represents an element in a UIOptionList */
class UIOption final : public UIButtonElement {
public:
	explicit UIOption(
		const string& label,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy
	) :   UIButtonElement(label, sx, sy) {

		this->x = x;
		this->y = y;
	}

	void render(const float xpos, const float ypos) override {
		button->render(xpos, ypos);
	}

	void checkButtonPressed() const override {
		if (button->belowMouse()) {
			if (button->onClick) button->onClick();
		}
	}
};


/** A UIOptionList is a list of UIOption variants */
class UIOptionList final : public UIButtonElement {
public:
	const vector<shared_ptr<UIOptionVariant>> options;
	mutable bool isOpen;
	bool isTabRoot;

	explicit UIOptionList(
		const string& label,
		const bool isTabRoot,
		const vector<shared_ptr<UIOptionVariant>>& options,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy
	) :   UIButtonElement(label, sx, sy),
		  options(options),
		  isTabRoot(isTabRoot) {

		this->x = x;
		this->y = y;

		isOpen = false;
	}

	void render(const float xpos, const float ypos) override {	// NOLINT(*-no-recursion)
		button->render(x + xpos, y + ypos);

		if (!isTabRoot) {
			// Draw an arrow to the right to signify Option List
			glBegin(GL_TRIANGLES);
			color3f(Colors::UI_ARROW_COLOR);
			glVertex2f(x + xpos + sx.value - 25.0f, y + ypos + sy.value - 10.0f);
			glVertex2f(x + xpos + sx.value - 25.0f, y + ypos + 10.0f);
			glVertex2f(x + xpos + sx.value - 10.0f, y + ypos + sy.value / 2.0f);
			glEnd();
		}

		if (isTabRoot) {
			if (button->belowMouse()) {
				// Open the Tab on mouse hovering
				if (!isOpen) changeSize = 1;	// remove gap between Tab and OptionList for the Option selection
				isOpen = true;
			} else if (!isMouseInOptionList(options)) {
				// If the mouse is neither on the Tab nor in the Option List, the Tab closes
				if (isOpen) changeSize = 2;		// put gap back
				isOpen = false;
			}
			switch (changeSize) {
				case 1: resizeY(tabAugment); break;
				case 2: resizeY(-tabAugment); break;
				default: ;	// Keep size
			}
		} else {
			isMouseAnywhere();
		}

		changeSize = 0;

		if (isOpen) {
			// Render each Option in the List recursively
			for (const auto& option : options) {
				if (holds_alternative<UIOption>(*option)) {
					auto isOption = get<UIOption>(*option);
					isOption.render(isOption.x + xpos, isOption.y + ypos);
				}

				if (holds_alternative<UIOptionList>(*option))
					get<UIOptionList>(*option).render(xpos, ypos);
			}
		}
	}

	static bool isMouseHere(const UIOption& singleOption) {
		return singleOption.button->belowMouse();
	}

	static bool isMouseHere(const UIOptionList& innerOptionList) {
		innerOptionList.isMouseAnywhere();
		return innerOptionList.isOpen;
	}

	static bool isMouseInOptionList(const vector<shared_ptr<UIOptionVariant>>& optionList) {
		return ranges::any_of(optionList, [](const auto& option) {
			if (holds_alternative<UIOption>(*option))
				return isMouseHere(get<UIOption>(*option));

			if (holds_alternative<UIOptionList>(*option))
				return isMouseHere(get<UIOptionList>(*option));

			return false;
		});
	}

	void isMouseAnywhere() const {
		if (button->belowMouse()) {
			isOpen = true;
		} else if (!isMouseInOptionList(options)) {
			isOpen = false;
		}
	}

	void checkButtonPressed() const override { // NOLINT(*-no-recursion)
		if (!isOpen) return;

		if (button->belowMouse()) {
			if (button->onClick) button->onClick();
		}

		for (const auto& option : options) {
			if (holds_alternative<UIOption>(*option))
				get<UIOption>(*option).checkButtonPressed();

			if (holds_alternative<UIOptionList>(*option))
				get<UIOptionList>(*option).checkButtonPressed();
		}
	}

private:
	int changeSize = 0;		// Increases sy slightly to allow for mouse transition between UIBar and UIOptionList
};
