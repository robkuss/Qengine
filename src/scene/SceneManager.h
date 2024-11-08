#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "../objects/mesh/cube/Cube.cpp"
#include "../math/ray/Ray.h"

class SceneManager {
public:
	std::vector<std::shared_ptr<Object>> sceneObjects;		// Scene Objects as shared pointers to prevent object slicing
	std::shared_ptr<Object> selectedObject = nullptr;

	// Mode
	Mode viewportMode  = OBJECT;
	Mode transformMode = NONE;

	// Constructor & Destructor
	explicit SceneManager() {
		// Add Default Cube to scene
		addObject(
			std::make_shared<Cube>("Cube", Vector3(.5f, .5f, .5f), 1.0f)
		);
	}
	~SceneManager() = default;

	void render(Vector3 camPos) const;

	void addObject(const std::shared_ptr<Object>& obj)	  { sceneObjects.push_back(obj); }
	void removeObject(const std::shared_ptr<Object>& obj) { sceneObjects.erase(std::ranges::find(sceneObjects, obj)); }
	void selectObject(const std::shared_ptr<Object>& obj) { selectedObject = obj; }
	void select(const Ray &ray);

	void transform(double mouseX, double mouseY, int width, int height, Vector3 worldPos, Vector3 camPos);

	void toggleViewportMode();
	void changeTransformMode(Mode::ModeEnum mode);
	void changeTransformSubMode(SubMode subMode);

	// Variables for object transformation TODO could be made private if debugging was better
	Vector3 transformation	= Vector3::ZERO;
	Vector3 lastTransform	= transformation;

private:
	float scaleSens			= 1000.0f;
};

#endif // SCENE_MANAGER_H
