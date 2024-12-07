#pragma once

class Ray;
class Object;
class Mesh;

#include "RenderContext.h"
#include "graphics/Viewport.h"

class SceneManager {
public:
	RenderContext* context{};

	// Constructor & Destructor
	explicit SceneManager();
	~SceneManager() = default;

	void render() const;

private:
	// Grant a few Viewport functions access to private members using the best keyword in C++
	friend void Viewport::drawOnScreenText() const;
	friend void Viewport::setCallbacks(GLFWwindow* window);
	friend void Viewport::onKeyboardInput(GLFWwindow* cbWindow, int key, int scancode, int action, int mods);

	// Mode
	Mode selectionMode = OBJECT;
	Mode transformMode = NONE;

	std::vector<std::shared_ptr<Object>> sceneObjects;		// Scene Objects as shared pointers to prevent object slicing
	std::vector<std::shared_ptr<Object>> selectedObjects;
	std::vector<Vertex> selectedVertices;

	float scalingSens		= 1000.0f;
	Vector3 lastTransform   = Vector3::ZERO;

	void addObject(const std::shared_ptr<Object>& obj)	  { sceneObjects.push_back(obj); }
	void removeObject(const std::shared_ptr<Object>& obj) { sceneObjects.erase(std::ranges::find(sceneObjects, obj)); }

	void select(const Vector2 &mousePos, bool preserve);
	void selectObject(const std::shared_ptr<Object>& obj);
	void deselectObject(const std::shared_ptr<Object> &obj);
	void selectVertex(const Vertex &v);
	void deselectVertex(const Vertex &v);

	[[nodiscard]] std::vector<std::shared_ptr<Mesh>> getSelectedMeshes() const;

	void transform(double mouseX, double mouseY, int width, int height, Vector3 worldPos, Vector3 camPos);
	void applyTransformation();

	void toggleSelectionMode();
	void setTransformMode(const Mode& mode);
	void setTransformSubMode(const SubMode& subMode);

	void toggleShadingMode() const;
};
