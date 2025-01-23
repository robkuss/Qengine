#pragma once

#include "UI.h"
#include "UITab.h"

class UIBar final : public UIElement {
public:
	// Constructor
	explicit UIBar(
		std::vector<std::shared_ptr<UITab>> tabs,
		const float x = 0.0f,
		const float y = 0.0f,
		const Dim sx = NODIM,
		const Dim sy = NODIM
	) :   UIElement(x, y, sx, sy),
		  tabs(std::move(tabs)) {

		setVertices();
	}

	void render() const override;
	void setVertices() override;

private:
	std::vector<std::shared_ptr<UITab>> tabs;
};
