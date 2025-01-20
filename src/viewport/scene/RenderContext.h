#pragma once

#include <array>
#include <memory>
#include <vector>

#include <viewport/Mode.h>
#include "math/geometry/Vertex.h"

class Ray;
class Camera;
class Object;


struct RenderContext {
	Mode selectionMode = NONE;
	std::array<int, 4>* viewport = nullptr;
	Camera* activeCamera = nullptr;
	std::vector<std::shared_ptr<Object>> selectedObjects{};
	std::vector<Vertex> selectedVertices{};
	Ray* mouseRay = nullptr; // Use pointer to avoid issues with non-copyable reference
	bool depthIsolation = false;
	bool fixedPosition  = false;

	RenderContext() = default;
};
