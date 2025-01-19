#pragma once

#include <vector>

#include "math/vector/Vector2.h"

class UIElement {
public:
	int vertexCount{};
	std::vector<Vector2> vertices{};

	virtual ~UIElement() = default;

	virtual void update() = 0;
	virtual void render() const = 0;
};