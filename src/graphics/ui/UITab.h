#pragma once

#include "UIElement.h"
#include "UIOption.h"
#include "input/Button.h"


class UITab final : public UIElement {
public:
	std::shared_ptr<Button> button;				// Tab button
	std::shared_ptr<UIOptionList> optionList;	// Root OptionList for the Tab

	explicit UITab(
		const std::shared_ptr<Button>& button,
		const std::shared_ptr<UIOptionList>& optionList,
		const float x = 0.0f,
		const float y = 0.0f,
		const Dim sx = NODIM,
		const Dim sy = NODIM
	) :   UIElement(x, y, sx, sy),
		  button(button),
		  optionList(optionList) {

		button->x  = x;
		button->y  = tabPadding;
		button->sx = sx;
		button->sy = Dim(sy.value - 2 * tabPadding, DimType::Pixels);

		setVertices();
		button->setVertices();
		button->setActivated(true);
	}

	void render() const override;
	void setVertices() override;
};


inline void UITab::render() const {

}

inline void UITab::setVertices() {

}
