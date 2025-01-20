#include "SceneManager.h"

#include <objects/mesh/Mesh.h>

#include "Camera.h"

void SceneManager::selectAllObjects(const std::vector<std::shared_ptr<Object>>& sceneObjects) {
	for (const auto& obj : sceneObjects) {
		selectedObjects.emplace_back(obj);
	}
}

void SceneManager::deselectAllObjects() {
	selectedObjects.clear();
}

void SceneManager::selectAllVertices(const std::shared_ptr<Mesh>& mesh) {
	for (const auto& v : mesh->vertices) {
		selectedVertices.emplace_back(*v);
	}
}

void SceneManager::deselectAllVertices() {
	selectedVertices.clear();
}

void SceneManager::selectObject(const std::shared_ptr<Object>& obj) {
	if (std::ranges::find(selectedObjects, obj) == selectedObjects.end()) {
		selectedObjects.emplace_back(obj);
	} else {
		deselectObject(obj);
	}
}

void SceneManager::deselectObject(const std::shared_ptr<Object>& obj) {
	if (const auto it = std::ranges::find(selectedObjects, obj); it != selectedObjects.end()) {
		selectedObjects.erase(it);
	}
}

void SceneManager::selectVertex(const Vertex& v) {
	if (std::ranges::find(selectedVertices, v) == selectedVertices.end()) {
		selectedVertices.emplace_back(v);
	} else {
		deselectVertex(v);
	}
}

void SceneManager::deselectVertex(const Vertex& v) {
	if (const auto it = std::ranges::find(selectedVertices, v);it != selectedVertices.end()) {
		selectedVertices.erase(it);
	}
}

void SceneManager::toggleSelectionMode() {
	if (selectionMode == OBJECT) {
		// Don't change mode if no Object is selected
		if (!selectedObjects.empty()) {
			selectionMode = EDIT;
		}
	} else if (selectionMode == EDIT) {
		selectionMode = OBJECT;
	}
}

void SceneManager::setTransformMode(const Mode& mode) {
	// Don't change mode if no Object is selected
	if (selectedObjects.empty()) return;
	transformMode = mode;

	// Reset transformation direction
	transformMode.subMode = SubMode::NONE;
}

void SceneManager::setTransformSubMode(const SubMode& subMode) {
	// Don't change mode if no Object is selected
	if (selectedObjects.empty() || transformMode.type != ModeType::TRANSFORM) return;

	// Set transformation direction
	transformMode.subMode = subMode;
}

void SceneManager::toggleShadingMode() const {
	if (const auto meshes = getSelectedMeshes(); !meshes.empty()) {
		for (const auto& mesh : meshes) {
			mesh->setShadingMode(mesh->shadingMode == ShadingMode::SMOOTH
				? ShadingMode::FLAT
				: ShadingMode::SMOOTH);
		}
	}
}

std::vector<std::shared_ptr<Mesh>> SceneManager::getSelectedMeshes() const {
	std::vector<std::shared_ptr<Mesh>> meshes;
	for (const auto& obj : selectedObjects) {
		// Check if the object is a Mesh by using dynamic_pointer_cast
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(obj)) {
			meshes.emplace_back(mesh);
		}
	}
	return meshes;
}

void SceneManager::transform(
	const double mouseX,
	const double mouseY,
	const Vector3 worldPos,
	const Vector3 camPos
) {
    if (getSelectedMeshes().empty()) return;	// TODO Implement transformation for Objects that aren't Meshes

	// Determine transformation direction
	const Vector3 direction = clampDirection(transformMode.subMode);
	if (direction == Vector3::ZERO) return;

	// Initialize lastTransform if zero
	if (lastTransform == Vector3::ZERO) lastTransform = worldPos;
	const auto dPos = worldPos - lastTransform;

	for (const auto& mesh : getSelectedMeshes()) {
		const auto camDist = mesh->position.distance(camPos); // Distance from Object to camera

		const auto mouseDist = static_cast<float>(	// Distance from Object to mouse
			 project(mesh->position, viewport, activeCamera->viewMatrix, activeCamera->projMatrix)
			.distance(Vector2(mouseX, mouseY))
		);

		switch (transformMode.mode) {
			case Mode::GRAB: {
				const Matrix4 transform = Matrix4::translate(
					direction * camDist	// Clamp direction
					* dPos				// Difference from last transform
				);
				mesh->applyTransformation(transformMode, transformMode, transform);
				break;
			}
			case Mode::SCALE: {
				const float scaleFactor = camDist * mouseDist * scalingSens;
				// Clamp direction
				const auto scaleVector = Vector3(
					direction.x != 0 ? scaleFactor : mesh->scale.x,
					direction.y != 0 ? scaleFactor : mesh->scale.y,
					direction.z != 0 ? scaleFactor : mesh->scale.z
				);
				const Matrix4 transform	= Matrix4::scale(
					  scaleVector
					/ mesh->scale		// Difference from last transform
				);
				mesh->applyTransformation(transformMode, transformMode, transform);
				break;
			}
			case Mode::ROTATE: {
				// Calculate rotation angle based on mouse drag distance
				const float angle = dPos.length() * rotationSens;

				// Create rotation matrix
				Matrix4 rotation =
					  Matrix4::rotateX(angle * direction.x)
					* Matrix4::rotateY(angle * direction.y)
					* Matrix4::rotateZ(angle * direction.z);

				// Apply rotation
				mesh->applyTransformation(transformMode, transformMode, rotation);
				break;
			}
			default: throw std::invalid_argument("Invalid transformation: Wrong Mode");
		}
	}

	// Update lastTransform for next frame
	lastTransform = worldPos;
}

void SceneManager::applyTransformation() {
	if (transformMode != NONE) {
		lastTransform			= Vector3::ZERO;	// Reset transformation data
		transformMode			= NONE;				// Go back to View Mode
		transformMode.subMode	= SubMode::NONE;	// Reset transformation direction
	}
}
