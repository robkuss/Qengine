#pragma once

#include "UIElement.h"
#include "UIOption.h"

struct UITab : UIElement {
	std::vector<UIOptionList> options;
};
