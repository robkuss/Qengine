#pragma once

#include <array>
#include <memory>
#include <vector>
#include <math/ray/Ray.h>

#include <math/vector/Vector3.h>
#include "Mode.h"

class Object;
struct Vertex;

struct RenderContext {
	Vector3 camPos{};
	std::array<int, 4> viewport{};
	std::array<float, 16> viewMatrix{};
	std::array<float, 16> projMatrix{};
	Mode selectionMode;
	std::vector<std::shared_ptr<Object>> selectedObjects{};
	std::vector<std::shared_ptr<Vertex>> selectedVertices{};
	Ray* mouseRay = nullptr; // Use pointer to avoid issues with non-copyable reference

	explicit RenderContext(const Mode selectionMode) : selectionMode(selectionMode) {}
};