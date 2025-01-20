#pragma once

#include <memory>
#include <vector>

#include <math/vector/Vector2.h>
#include <math/vector/Vector3.h>

#include "SceneManager.h"


class Color;
class Ray;
class Object;
class Mesh;
struct Vertex;


class Scene {
public:
	std::shared_ptr<SceneManager> context;

	// Constructor & Destructor
	explicit Scene(const std::shared_ptr<SceneManager>& context) : context(context) {}
	~Scene() = default;

	void render() const;

	void addObject(const std::shared_ptr<Object>& obj)	  { sceneObjects.emplace_back(obj); }
	void removeObject(const std::shared_ptr<Object>& obj) { sceneObjects.erase(std::ranges::find(sceneObjects, obj)); }

	void select(const Vector2& mousePos, bool preserve) const;

	static void setLight(const Color& diffuse, const Color& ambient, const Color& specular);

	// Helpers
	[[nodiscard]] Vector3 mouseWorld(double mouseX, double mouseY) const;

private:
	// Grant Debugger access to sceneObjects using the best keyword in C++
	friend class Debug;

	// Objects
	std::vector<std::shared_ptr<Object>> sceneObjects;		// Scene Objects as shared pointers to prevent object slicing

	// Lighting
	float light1Pos[4] = {2, 3, 6, 0};
	float light2Pos[4] = {-2, -3, -6, 0};
};
