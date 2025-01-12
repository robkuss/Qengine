#pragma once

#include <array>
#include <memory>
#include <vector>
#include <math/ray/Ray.h>

#include <math/vector/Vector3.h>

#include "Camera.h"
#include "../Mode.h"

class Object;
struct Vertex;

struct RenderContext {
	std::array<int, 4> viewport{};
	Camera* activeCamera = nullptr;
	Mode selectionMode;
	std::vector<std::shared_ptr<Object>> selectedObjects{};
	std::vector<Vertex> selectedVertices{};
	Ray* mouseRay = nullptr; // Use pointer to avoid issues with non-copyable reference
	bool depthIsolation = false;
	bool fixedPosition  = false;

	explicit RenderContext(const Mode selectionMode) : selectionMode(selectionMode) {}
};