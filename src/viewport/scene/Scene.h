#pragma once

#include <memory>
#include <vector>

#include "SceneManager.h"
#include "objects/light/Light.h"


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

	virtual void render();

	void addObject(const std::shared_ptr<Object>& obj);
	void removeObject(const std::shared_ptr<Object>& obj);

	void addLight(const std::shared_ptr<Light> &light, const Color &diffuse, const Color &ambient, const Color &specular);

private:
	// Grant UI and SceneManager access to sceneObjects using the best keyword in C++
	friend class UI;
	friend class Debug;
	friend class SceneManager;

	// Objects
	std::vector<std::shared_ptr<Object>> sceneObjects;	// Scene Objects (as shared pointers to prevent object slicing)
	std::vector<std::shared_ptr<Light>> lights;			// Scene Lights
};
