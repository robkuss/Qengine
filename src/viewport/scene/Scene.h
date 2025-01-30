#pragma once

#include <vector>

#include "objects/light/Light.h"

using namespace std;

class Color;
class Ray;
class Object;
class Mesh;
struct Vertex;


class Scene final {
public:
	string name;

	// Constructor & Destructor
	explicit Scene(string name) : name(move(name)) {}
	~Scene() = default;

	void render() const;

	void addObject(const shared_ptr<Object>& obj);
	void removeObject(const shared_ptr<Object>& obj);

	void addLight(const shared_ptr<Light> &light, const Color &diffuse, const Color &ambient, const Color &specular);

	void enableDepthIsolation();
	void enableFixedPosition();

private:
	// Grant UI and SceneManager access to sceneObjects using the best keyword in C++
	friend class UI;
	friend class Debug;
	friend class SceneManager;
	friend class UISceneManager;

	// Objects
	vector<shared_ptr<Object>> sceneObjects;	// Scene Objects (as shared pointers to prevent object slicing)
	vector<shared_ptr<Light>> lights;			// Scene Lights

	bool depthIsolation = false;
	bool fixedPosition  = false;
};
