#pragma once

class Ray;
class Object;
class Mesh;

#include "graphics/Viewport.h"

class SceneManager {
public:
	// Constructor & Destructor
	explicit SceneManager();
	~SceneManager() = default;

	void render(const Vector3 &camPos) const;

private:
	// Grant a few Viewport functions access to private members using the best keyword in C++
	friend void Viewport::drawOnScreenText() const;
	friend void Viewport::setCallbacks(GLFWwindow* window);
	friend void Viewport::onKeyboardInput(GLFWwindow *cbWindow, int key, int scancode, int action, int mods);

	// Mode
	Mode viewportMode  = OBJECT;
	Mode transformMode = NONE;

	std::vector<std::shared_ptr<Object>> sceneObjects;		// Scene Objects as shared pointers to prevent object slicing
	std::vector<std::shared_ptr<Object>> selectedObjects;

	float scalingSens		= 1000.0f;
	Vector3 lastTransform   = Vector3::ZERO;

	void addObject(const std::shared_ptr<Object>& obj)	  { sceneObjects.push_back(obj); }
	void removeObject(const std::shared_ptr<Object>& obj) { sceneObjects.erase(std::ranges::find(sceneObjects, obj)); }

	void select(const Ray& ray, const Mode& mode);
	void selectObject(const std::shared_ptr<Object>& obj);
	void deselectObjeect(const std::shared_ptr<Object> &obj);
	void selectFace(const Triangle* triangle) const;

	[[nodiscard]] std::vector<std::shared_ptr<Mesh>> getSelectedMeshes() const;

	void transform(double mouseX, double mouseY, int width, int height, Vector3 worldPos, Vector3 camPos);
	void applyTransformation();

	void toggleViewportMode();
	void setTransformMode(const Mode& mode);
	void setTransformSubMode(const SubMode& subMode);

	void toggleShadingMode() const;
};
