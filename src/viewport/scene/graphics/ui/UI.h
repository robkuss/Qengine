#pragma once

#include <glfw/glfw3.h>

#include "UIElement.h"

#include <vector>

class UI {
public:
	std::vector<UIElement> elements;

	static void setup();
};

