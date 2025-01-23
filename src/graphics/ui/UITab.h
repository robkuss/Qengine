
#pragma once

#include <utility>
#include <math/Util.h>

#include "UIButtonElement.h"
#include "UIOption.h"


class UITab final : public UIButtonElement {
public:
	std::vector<std::shared_ptr<UIOptionVariant>> optionList;	// Option List for the Tab
	bool isOpen;	// Tracks whether the user is currently selecting an Option from the Tab

	UITab(
		std::vector<std::shared_ptr<UIOptionVariant>> optionList,
		const std::string& label,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy
	) :   UIButtonElement(label, x, y, sx, sy),
		  optionList(std::move(optionList)) {

		isOpen = false;
	}

	void render() override {
		button->render();

		if (button->belowMouse()) {
			// Open the Tab on mouse hovering
			if (!isOpen) changeSize = 1;	// remove gap between Tab and OptionList for the Option selection
			isOpen = true;
		} else if (!UIOptionList::isMouseInOptionList(optionList)) {
			// If the mouse is neither on the Tab nor in the Option List, the Tab closes
			if (isOpen) changeSize = 2;		// put gap back
			isOpen = false;
		}
		switch (changeSize) {
			case 1: resizeY(tabAugment); break;
			case 2: resizeY(-tabAugment); break;
			default: ;	// Keep size
		}

		if (isOpen) {
			// Render the Option List only if the Tab is open
			for (const auto& option : optionList) {
				UI::variantToElement(option)->render();
			}
		}

		changeSize = 0;
	}

	void setVertices() override {}


private:
	int changeSize = 0;
};
