#pragma once

#include <array>
#include <memory>
#include <vector>

#include <viewport/scene/Mode.h>
#include "math/geometry/Vertex.h"

class Scene;
class UI;
class Object;
class Camera;
class Mesh;
class Ray;


class SceneManager {
public:
	// Pointers from Viewport
	static std::shared_ptr<std::array<int, 4>> viewport;
	static std::shared_ptr<Camera> activeCamera;
	static std::shared_ptr<Ray> mouseRay;
	static double* mouseX;
	static double* mouseY;

	// Store scenes as weak pointers to avoid circular dependencies
	static std::vector<std::shared_ptr<Scene>> scenes;

	static Mode selectionMode;
	static Mode transformMode;

	static std::vector<std::shared_ptr<Object>> selectedObjects;
	static std::vector<Vertex> selectedVertices;

private:
	friend class Viewport;
	friend class Scene;
	friend class UI;
	friend class UISceneManager;
	friend class Debug;

	// General
	static void addScene(const std::shared_ptr<Scene> &scene);
	static void deleteScene(const std::shared_ptr<Scene> &scene);
	static void cleanupScenes();
	static void renderScenes();

	// Selection
	static void select(const Vector2 &mousePos, bool preserve);

	static void selectAllObjects(const std::vector<std::shared_ptr<Object>> &sceneObjects);
	static void deselectAllObjects();
	static void selectAllVertices(const std::shared_ptr<Mesh>& mesh);
	static void deselectAllVertices();

	static void selectObject(const std::shared_ptr<Object>& obj);
	static void selectObject(const std::string &label);
	static void deselectObject(const std::shared_ptr<Object> &obj);
	static void selectVertex(const Vertex &v);
	static void deselectVertex(const Vertex &v);

	static void toggleSelectionMode();

	[[nodiscard]] static bool isMeshSelected(const std::shared_ptr<Mesh> &mesh);
	[[nodiscard]] static std::vector<std::shared_ptr<Mesh>> getSelectedMeshes();

	// Transformation
	static void setTransformMode(const Mode& mode);
	static void setTransformSubMode(const SubMode& subMode);

	static void transform(double mouseX, double mouseY, Vector3 worldPos, Vector3 camPos);
	static void applyTransformation();

	// Other
	[[nodiscard]] static Vector3 mouseWorld();

	static void toggleShadingMode();
};
