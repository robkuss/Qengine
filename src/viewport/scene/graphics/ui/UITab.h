#pragma once

#include "UIElement.h"
#include "UIOption.h"

#include <utility>


class UITab final : public UIElement {
public:
	std::string name;                       // Tab name
	std::shared_ptr<UIOptionList> options;  // Root OptionList for the Tab

	UITab(std::string name, const std::shared_ptr<UIOptionList>& options)
		: name(std::move(name)), options(options) {}

	void update() override;
	void render() const override;
};

inline void UITab::update() {

}

inline void UITab::render() const {

}
