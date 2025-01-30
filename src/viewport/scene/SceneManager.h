#pragma once

#include <array>
#include <memory>
#include <vector>

#include <viewport/scene/Mode.h>
#include "math/geometry/Vertex.h"

using namespace std;

class Scene;
class UI;
class Object;
class Camera;
class Mesh;
class Ray;


class SceneManager {
public:
	// Pointers from Viewport
	static shared_ptr<array<int, 4>> viewport;
	static shared_ptr<Camera> activeCamera;
	static shared_ptr<Ray> mouseRay;
	static double* mouseX;
	static double* mouseY;

	// Store scenes as weak pointers to avoid circular dependencies
	static vector<shared_ptr<Scene>> scenes;

	static Mode selectionMode;
	static Mode transformMode;

	static vector<shared_ptr<Object>> selectedObjects;

private:
	friend class Viewport;
	friend class Scene;
	friend class UI;
	friend class UISceneManager;
	friend class Debug;

	// General
	static void addScene(const shared_ptr<Scene> &scene);
	static void deleteScene(const shared_ptr<Scene> &scene);
	static void cleanupScenes();
	static void renderScenes();

	// Selection
	static void select(const Vector2 &mousePos, bool preserve);

	static void selectAllObjects(const vector<shared_ptr<Object>> &sceneObjects);
	static void deselectAllObjects();
	static void selectAllVertices(const shared_ptr<Mesh>& mesh);
	static void deselectAllVertices();

	static void selectObject(const shared_ptr<Object>& obj);
	static void selectObject(const string &label);
	static void deselectObject(const shared_ptr<Object> &obj);
	static void selectVertex(const shared_ptr<Vertex> &v);

	static void toggleSelectionMode();

	[[nodiscard]] static bool isMeshSelected(const shared_ptr<Mesh> &mesh);
	[[nodiscard]] static vector<shared_ptr<Mesh>> getSelectedMeshes();

	// Transformation
	static void setTransformMode(const Mode& mode);
	static void setTransformSubMode(const SubMode& subMode);

	static void transform(double mouseX, double mouseY, Vector3 worldPos, Vector3 camPos);
	static void applyTransformation();

	// Other
	[[nodiscard]] static Vector3 mouseWorld();

	static void toggleShadingMode();
};
