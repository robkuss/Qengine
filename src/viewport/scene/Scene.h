#pragma once

#include <memory>
#include <vector>

#include <math/vector/Vector2.h>
#include <math/vector/Vector3.h>
#include <viewport/Mode.h>

#include "RenderContext.h"


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
	explicit Scene(RenderContext* context) : context(context) {}
	~Scene() = default;

	void render() const;

	// Objects
	void addObject(const std::shared_ptr<Object>& obj)	  { sceneObjects.emplace_back(obj); }
	void removeObject(const std::shared_ptr<Object>& obj) { sceneObjects.erase(std::ranges::find(sceneObjects, obj)); }

	void select(const Vector2& mousePos, bool preserve);
	void transform(double mouseX, double mouseY, Vector3 worldPos, Vector3 camPos);

	static void setLight(const Color& diffuse, const Color& ambient, const Color& specular);

	// Mode
	Mode selectionMode = OBJECT;
	Mode transformMode = NONE;

	void toggleSelectionMode();
	void setTransformMode(const Mode& mode);
	void setTransformSubMode(const SubMode& subMode);
	void toggleShadingMode() const;

	// Helpers
	[[nodiscard]] Vector3 mouseWorld(double mouseX, double mouseY) const;

private:
	// Grant Debugger access to sceneObjects using the best keyword in C++
	friend class Debug;

	// Constants
	const float scalingSens		= 0.001;	// Scaling sensitivity
	const float rotationSens	= 10.0f;	// Rotation sensitivity

	// Objects
	std::vector<std::shared_ptr<Object>> sceneObjects;		// Scene Objects as shared pointers to prevent object slicing
	std::vector<std::shared_ptr<Object>> selectedObjects;
	std::vector<Vertex> selectedVertices;
	Vector3 lastTransform   = Vector3::ZERO;

	// Lighting
	float light1Pos[4] = {2, 3, 6, 0};
	float light2Pos[4] = {-2, -3, -6, 0};


	// Functions
	void selectAllObjects();
	void deselectAllObjects();
	void selectAllVertices(const std::shared_ptr<Mesh>& mesh);
	void deselectAllVertices();

	void selectObject(const std::shared_ptr<Object>& obj);
	void deselectObject(const std::shared_ptr<Object> &obj);
	void selectVertex(const Vertex &v);
	void deselectVertex(const Vertex &v);

	[[nodiscard]] std::vector<std::shared_ptr<Mesh>> getSelectedMeshes() const;

	void applyTransformation();
};
