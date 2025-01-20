#pragma once

#include <array>
#include <memory>
#include <vector>

#include <viewport/Mode.h>
#include "math/geometry/Vertex.h"

class Mesh;
class Ray;
class Camera;
class Object;


class SceneManager {
public:
	Mode selectionMode		= OBJECT;
	Mode transformMode		= NONE;

	std::array<int, 4>* viewport	= nullptr;
	Camera* activeCamera			= nullptr;
	Ray* mouseRay					= nullptr;

	bool depthIsolation				= false;
	bool fixedPosition				= false;

	std::vector<std::shared_ptr<Object>> selectedObjects{};
	std::vector<Vertex> selectedVertices{};

	SceneManager() = default;

	void selectAllObjects(const std::vector<std::shared_ptr<Object>> &sceneObjects);
	void deselectAllObjects();
	void selectAllVertices(const std::shared_ptr<Mesh>& mesh);
	void deselectAllVertices();

	void selectObject(const std::shared_ptr<Object>& obj);
	void deselectObject(const std::shared_ptr<Object> &obj);
	void selectVertex(const Vertex &v);
	void deselectVertex(const Vertex &v);

	void toggleSelectionMode();
	void setTransformMode(const Mode& mode);
	void setTransformSubMode(const SubMode& subMode);
	void toggleShadingMode() const;

	void transform(double mouseX, double mouseY, Vector3 worldPos, Vector3 camPos);

private:
	friend class Scene;

	Vector3 lastTransform   = Vector3::ZERO;

	// Constants
	const float scalingSens		= 0.001;	// Scaling sensitivity
	const float rotationSens	= 10.0f;	// Rotation sensitivity

	void applyTransformation();

	[[nodiscard]] std::vector<std::shared_ptr<Mesh>> getSelectedMeshes() const;
};
