#pragma once

#include <memory>
#include <vector>

#include <math/vector/Vector2.h>
#include <math/vector/Vector3.h>
#include <viewport/Mode.h>

#include <viewport/Viewport.h>

class Color;
class Ray;
class Object;
class Mesh;

struct RenderContext;
struct Vertex;


class Scene {
public:
	RenderContext* context{};

	// Constructor & Destructor
	explicit Scene();
	~Scene() = default;

	void render() const;

	void addObject(const std::shared_ptr<Object>& obj)	  { sceneObjects.emplace_back(obj); }
	void removeObject(const std::shared_ptr<Object>& obj) { sceneObjects.erase(std::ranges::find(sceneObjects, obj)); }

	static void setLight(const Color& diffuse, const Color& ambient, const Color& specular);

private:
	// Grant the Debugger and a few Viewport functions access to private members using the best keyword in C++
	friend class Debug;
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

	void select(const Vector2& mousePos, bool preserve);

	void selectAllObjects();
	void deselectAllObjects();
	void selectAllVertices(const std::shared_ptr<Mesh>& mesh);
	void deselectAllVertices();

	void selectObject(const std::shared_ptr<Object>& obj);
	void deselectObject(const std::shared_ptr<Object> &obj);
	void selectVertex(const Vertex &v);
	void deselectVertex(const Vertex &v);

	[[nodiscard]] std::vector<std::shared_ptr<Mesh>> getSelectedMeshes() const;

	void transform(double mouseX, double mouseY, Vector3 worldPos, Vector3 camPos);
	void applyTransformation();

	void toggleSelectionMode();
	void setTransformMode(const Mode& mode);
	void setTransformSubMode(const SubMode& subMode);

	void toggleShadingMode() const;
};
