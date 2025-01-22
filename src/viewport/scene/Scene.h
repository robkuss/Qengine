#pragma once

#include <memory>
#include <vector>

#include "SceneManager.h"


class Color;
class Ray;
class Object;
class Mesh;
struct Vertex;


class Scene {
public:
	// Constructor & Destructor
	Scene() = default;
	virtual ~Scene() = default;

	virtual void render() const;

	void addObject(const std::shared_ptr<Object>& obj);
	void removeObject(const std::shared_ptr<Object>& obj);

	static void setLight(const Color& diffuse, const Color& ambient, const Color& specular);

private:
	// Grant Debugger and SceneManager access to sceneObjects using the best keyword in C++
	friend class Debug;
	friend class SceneManager;

	// Objects
	std::vector<std::shared_ptr<Object>> sceneObjects;		// Scene Objects as shared pointers to prevent object slicing

	// Lighting
	float light1Pos[4] = {2, 3, 6, 0};
	float light2Pos[4] = {-2, -3, -6, 0};
};
