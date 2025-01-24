#pragma once

#include <algorithm>
#include <string>

#include "UIButtonElement.h"


/** A UIOption represents an element in a UIOptionList */
class UIOption final : public UIButtonElement {
public:
	explicit UIOption(
		const std::string& label,
		const float x, const float y, const Dim sx, const Dim sy
	) :   UIButtonElement(label, x, y, sx, sy) {}

	void render() override {
		button->render();
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
	const std::vector<std::shared_ptr<UIOptionVariant>> options;
	mutable bool isOpen;
	bool isTabRoot;

	explicit UIOptionList(
		const std::string& label,
		const bool isTabRoot,
		const std::vector<std::shared_ptr<UIOptionVariant>>& options,
		const float x, const float y, const Dim sx, const Dim sy
	) :   UIButtonElement(label, x, y, sx, sy),
		  options(options),
		  isTabRoot(isTabRoot) {

		isOpen = false;
	}

	void render() override {	// NOLINT(*-no-recursion)
		button->render();

		if (!isTabRoot) {
			// Draw an arrow to the right to signify Option List
			glBegin(GL_TRIANGLES);
			color3f(Colors::UI_ARROW_COLOR);
			glVertex2f(x + sx.value - 25.0f, y + sy.value - 10.0f);
			glVertex2f(x + sx.value - 25.0f, y + 10.0f);
			glVertex2f(x + sx.value - 10.0f, y + sy.value / 2.0f);
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
				if (std::holds_alternative<UIOption>(*option))
					std::get<UIOption>(*option).render();

				if (std::holds_alternative<UIOptionList>(*option))
					std::get<UIOptionList>(*option).render();
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

	static bool isMouseInOptionList(const std::vector<std::shared_ptr<UIOptionVariant>>& optionList) {
		return std::ranges::any_of(optionList, [](const auto& option) {
			if (std::holds_alternative<UIOption>(*option))
				return isMouseHere(std::get<UIOption>(*option));

			if (std::holds_alternative<UIOptionList>(*option))
				return isMouseHere(std::get<UIOptionList>(*option));

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
			if (std::holds_alternative<UIOption>(*option))
				std::get<UIOption>(*option).checkButtonPressed();

			if (std::holds_alternative<UIOptionList>(*option))
				std::get<UIOptionList>(*option).checkButtonPressed();
		}
	}

private:
	int changeSize = 0;
};
