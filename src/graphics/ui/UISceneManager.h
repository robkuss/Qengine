#pragma once

#include "UIElement.h"


class UISceneManager final : public UIElement {
public:
	UISceneManager(
		const std::string& label,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy
	) : UIElement(label, x, y, sx, sy) {}

	void render() override;
	void setVertices() override {}
};
