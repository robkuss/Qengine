#pragma once

#include <array>
#include <memory>
#include <vector>

#include <viewport/Mode.h>

class Ray;
class Camera;
class Object;
struct Vertex;

struct RenderContext {
	std::array<int, 4>* viewport = nullptr;
	Camera* activeCamera = nullptr;
	Mode selectionMode;
	std::vector<std::shared_ptr<Object>> selectedObjects{};
	std::vector<Vertex> selectedVertices{};
	Ray* mouseRay = nullptr; // Use pointer to avoid issues with non-copyable reference
	bool depthIsolation = false;
	bool fixedPosition  = false;

	explicit RenderContext(const Mode selectionMode) : selectionMode(selectionMode) {}
};
