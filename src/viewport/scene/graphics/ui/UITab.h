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
		const std::shared_ptr<UIOptionList>& optionList
	) :   button(button),
	      optionList(optionList) {}

	void render() const override;
	void setVertices(float windowW, float windowH) override;
};


inline void UITab::render() const {

}

inline void UITab::setVertices(const float windowW, const float windowH) {

}
