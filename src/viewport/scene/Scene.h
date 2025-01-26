#pragma once

#include <memory>
#include <utility>
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
	std::string name;

	// Constructor & Destructor
	explicit Scene(std::string name) : name(std::move(name)) {}
	virtual ~Scene() = default;

	virtual void render();

	void addObject(const std::shared_ptr<Object>& obj);
	void removeObject(const std::shared_ptr<Object>& obj);

	void addLight(const std::shared_ptr<Light> &light, const Color &diffuse, const Color &ambient, const Color &specular);

	void enableDepthIsolation();
	void enableFixedPosition();

private:
	// Grant UI and SceneManager access to sceneObjects using the best keyword in C++
	friend class UI;
	friend class Debug;
	friend class SceneManager;
	friend class UISceneManager;

	// Objects
	std::vector<std::shared_ptr<Object>> sceneObjects;	// Scene Objects (as shared pointers to prevent object slicing)
	std::vector<std::shared_ptr<Light>> lights;			// Scene Lights

	bool depthIsolation = false;
	bool fixedPosition  = false;
};
