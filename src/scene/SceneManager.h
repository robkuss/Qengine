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

	void addObject(const std::shared_ptr<Object>& obj)	  { sceneObjects.push_back(obj); }
	void removeObject(const std::shared_ptr<Object>& obj) { sceneObjects.erase(std::ranges::find(sceneObjects, obj)); }

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

	Vector3 lastTransform   = Vector3::ZERO;
	Vector2 lastMousePos	= Vector2(0.0, 0.0);

	const float scalingSens		= 0.001;	// Scaling sensitivity
	const float rotationSens	= 10.0f;	// Rotation sensitivity

	void select(const Vector2 &mousePos, bool preserve);

	void selectAllObjects();
	void deselectAllObjects();
	void selectAllVertices(const std::shared_ptr<Mesh> &mesh);
	void deselectAllVertices();

	void selectObject(const std::shared_ptr<Object>& obj);
	void deselectObject(const std::shared_ptr<Object> &obj);
	void selectVertex(const Vertex &v);
	void deselectVertex(const Vertex &v);

	[[nodiscard]] std::vector<std::shared_ptr<Mesh>> getSelectedMeshes() const;

	void transform(double mouseX, double mouseY, Vector3 worldPos, Vector3 camPos);
	void doSingleTransformation(const std::shared_ptr<Mesh> &mesh, const Vector3 &position, const Vector3 &direction, const Vector3 &camPos, const
	                            Vector3 &worldPos, const Vector2 &mousePos) const;
	void applyTransformation();

	void toggleSelectionMode();
	void setTransformMode(const Mode& mode);
	void setTransformSubMode(const SubMode& subMode);

	void toggleShadingMode() const;
};
