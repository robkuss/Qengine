#include "../objects/mesh/cube/Cube.cpp"
#include "../scene/graphics/MeshRenderer.cpp"

#include <algorithm>


class SceneManager {
public:
	std::vector<std::shared_ptr<Object>> sceneObjects;		// Scene Objects as shared pointers to prevent object slicing
	std::shared_ptr<Object> selectedObject = nullptr;

	// Constructor & Destructor
	explicit SceneManager() {
		// Add Default Cube to scene
		addObject(
			std::make_shared<Cube>("Cube", Vector3(.5f, .5f, .5f), 1.0f)
		);
	}
	~SceneManager() = default;

	void render(Mode mode, Vector3 camPos) const;

	void addObject(const std::shared_ptr<Object>& obj)	  { sceneObjects.push_back(obj); }
	void removeObject(const std::shared_ptr<Object>& obj) { sceneObjects.erase(std::ranges::find(sceneObjects, obj)); }
	void selectObject(const std::shared_ptr<Object>& obj) { selectedObject = obj; }
};

inline void SceneManager::render(const Mode mode, const Vector3 camPos) const {
	// Loop through the sceneObjects and render Mesh instances
	for (const auto& objPtr : sceneObjects) {
		// Check if the current Object is selected
		const bool isSelected = selectedObject && selectedObject == objPtr;

		// Attempt to cast Object to Mesh using dynamic_cast
		if (const auto mesh = dynamic_cast<const Mesh*>(objPtr.get())) {
			MeshRenderer::render(*mesh, camPos, isSelected, mode.mode == Mode::EDIT);
		}
	}
}
