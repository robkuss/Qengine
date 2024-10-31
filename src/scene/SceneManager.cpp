#ifndef SCENEMANAGER_C
#define SCENEMANAGER_C

#include "../objects/mesh/cube/Cube.cpp"
#include "../scene/graphics/MeshRenderer.cpp"

#include <optional>
#include <algorithm>


class SceneManager {
public:
	std::vector<Object> sceneObjects;
	std::optional<Object> selectedObject = std::nullopt;

	// Constructor & Destructor
	explicit SceneManager(const MeshRenderer meshRenderer) : meshRenderer(meshRenderer) {
		// Add Default Cube to scene
		addObject(
			Cube("Cube", Vector3(.5f, .5f, .5f), 1.0f)
		);
	}
	~SceneManager() = default;

	void render(Mode mode, Vector3 camPos);

	void addObject(const Object& obj) { sceneObjects.push_back(obj); }
	void removeObject(const Object& obj) { sceneObjects.erase(std::ranges::find(sceneObjects, obj)); }
	void selectObject(const std::optional<Object>& obj) { selectedObject = obj; }

private:
	MeshRenderer meshRenderer;
};

inline void SceneManager::render(const Mode mode, const Vector3 camPos) {
	// Loop through the sceneObjects and render Mesh instances
	for (Object& obj : sceneObjects) {
		// Check if selectedObject is not null and if it matches the current obj
		const bool isSelected = selectedObject.has_value() && *selectedObject == obj;

		// Attempt to cast Object to Mesh using dynamic_cast
		if (const auto mesh = dynamic_cast<const Mesh*>(&obj)) {
			MeshRenderer::render(*mesh, camPos, isSelected, mode.mode == Mode::EDIT);
		}
	}
}

#endif // SCENEMANAGER_C
