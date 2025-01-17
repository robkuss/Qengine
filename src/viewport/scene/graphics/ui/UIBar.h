#pragma once

#include "UIElement.h"
#include "UITab.h"

#include <vector>
#include <math/vector/Vector2.h>


struct UIBar : UIElement {
	bool isHorizontal;
	std::vector<UITab> tabs;
	Vector2 startPos;
	float length;
	float width;
};
