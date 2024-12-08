#include "SceneManager.h"

#include <memory>

#include <math/vector/Vector2.h>
#include <scene/graphics/MeshRenderer.h>
#include <objects/mesh/cube/Cube.cpp>

#include "RenderContext.h"


SceneManager::SceneManager() {
	// Add Default Cube to scene
	const auto cube = std::make_shared<Cube>("Cube", 1.0f);
	cube->setPosition(Vector3(0.5f, 0.5f, 0.5f));
	cube->setScale(Vector3::ONE);
	cube->setRotation(Vector3::ZERO);
	addObject(cube);
	context = new RenderContext(selectionMode);
}

void SceneManager::render() const {
	// Loop through the sceneObjects and render Mesh instances
	for (const auto& obj : sceneObjects) {
		if (const auto mesh = dynamic_cast<Mesh*>(obj.get())) {
			MeshRenderer::render(*mesh, *context);
		}
	}
}

/**
 * Handles the selection of [Object]s in the scene based on the current mouse position.
 *
 * This function retrieves the mouse Ray using the current mouse coordinates and checks for
 * intersections with any [Mesh] in the scene. If a Mesh is intersected, it is selected
 * by the [SceneManager]. Additionally, the Ray's origin and calculated endpoint are logged
 * for debugging purposes.
 *
 * @see getMouseRay for the ray computation logic.
 */
void SceneManager::select(const Vector2& mousePos, const bool preserve) {
	const auto ray = context->mouseRay;

	#ifdef DRAW_MOUSE_RAY
		// Update the visual Mouse Ray
		const auto directionScaled = ray.direction * MOUSE_RAY_LENGTH;
		vp->rayStart = ray.origin;
		vp->rayEnd   = ray.origin + directionScaled;
	#endif

	// If in Object Mode, select entire Objects
	if (selectionMode == OBJECT) {
		// Find Objects that intersect with the mouse Ray
		std::vector<std::shared_ptr<Object>> intersectingObjects;
		for (const auto& obj : sceneObjects) {
			// Attempt to cast Object to Mesh
			if (const auto mesh = dynamic_cast<Mesh*>(obj.get())) {
				if (ray->intersects(*mesh)) {
					intersectingObjects.push_back(obj);
				}
			} else {
				// TODO Implement selection logic for Objects that aren't Meshes
			}
		}

		if (!intersectingObjects.empty()) {
			// Select the Object that's closest to the Ray origin (the camera)
			selectObject(
				*std::ranges::min_element(
					intersectingObjects,
					[&ray](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
						return a->getPosition().distance(ray->origin) < b->getPosition().distance(ray->origin);
					}
				)
			);
		}

		if (!preserve && intersectingObjects.empty()) {
			// Deselect all previously selected Objects
			selectedObjects.clear();
			context->selectedObjects = selectedObjects;
		}
	}

	// If in Edit Mode, select specific Vertices
	else if (selectionMode == EDIT) {
		std::vector<std::shared_ptr<Vertex>> intersectingVertices;
		for (const auto& mesh : getSelectedMeshes()) {
			for (const auto& v : mesh->vertices) {
				if (const auto projV = project(*v, context->viewport, context->viewMatrix, context->projMatrix); Ray::intersects(projV, mousePos, SELECT_TOLERANCE)) {
					intersectingVertices.push_back(v);
				}
			}

			if (!intersectingVertices.empty()) {
				// Select the Vertex that's closest to the Ray origin (the camera)
				selectVertex(
					*std::ranges::min_element(
						intersectingVertices,
						[&ray](const std::shared_ptr<Vertex>& a, const std::shared_ptr<Vertex>& b) {
							return a->distance(ray->origin) < b->distance(ray->origin);
						}
					)
				);
			}
		}

		if (!preserve && intersectingVertices.empty()) {
			// Deselect all previously selected Vertices
			selectedVertices.clear();
			context->selectedVertices = selectedVertices;
		}
	}

	// When transforming a Mesh, clicking applies the transformation
	applyTransformation();
}

void SceneManager::selectObject(const std::shared_ptr<Object>& obj) {
	if (std::ranges::find(selectedObjects, obj) == selectedObjects.end()) {
		selectedObjects.push_back(obj);
		context->selectedObjects = selectedObjects;
	} else {
		deselectObject(obj);
	}
}

void SceneManager::deselectObject(const std::shared_ptr<Object>& obj) {
	if (const auto it = std::ranges::find(selectedObjects, obj); it != selectedObjects.end()) {
		selectedObjects.erase(it);
		context->selectedObjects = selectedObjects;
	}
}

void SceneManager::selectVertex(const std::shared_ptr<Vertex>& v) {
	if (std::ranges::find(selectedVertices, v) == selectedVertices.end()) {
		selectedVertices.push_back(v);
		context->selectedVertices = selectedVertices;
	} else {
		deselectVertex(v);
	}
}

void SceneManager::deselectVertex(const std::shared_ptr<Vertex>& v) {
	if (const auto it = std::ranges::find(selectedVertices, v); it != selectedVertices.end()) {
		selectedVertices.erase(it);
		context->selectedVertices = selectedVertices;
	}
}

std::vector<std::shared_ptr<Mesh>> SceneManager::getSelectedMeshes() const {
	std::vector<std::shared_ptr<Mesh>> meshes;
	for (const auto& obj : selectedObjects) {
		// Check if the object is a Mesh by using dynamic_pointer_cast
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(obj)) {
			meshes.push_back(mesh);
		}
	}
	return meshes;
}

void SceneManager::transform(const double mouseX, const double mouseY, const int width, const int height, const Vector3 worldPos, const Vector3 camPos) {
    // Get the selected Mesh
    const auto meshes= getSelectedMeshes();
    if (meshes.empty()) return;	// Selected Object is not a Mesh

	// Determine transformation direction
	const Vector3 direction = clampDirection(transformMode.subMode);
	if (direction == Vector3::ZERO) return;

	// Initialize lastTransform if zero
	if (lastTransform == Vector3::ZERO) lastTransform = worldPos;

	for (const auto& mesh : meshes) {
		switch (transformMode.mode) {
			case Mode::GRAB: {
				const Matrix4 transform = Matrix4::translate(
					direction													// Clamp direction
					* mesh->getPosition().distance(camPos)						// Distance from Object to camera
					* (worldPos - lastTransform)								// Difference from last transform
				);
				mesh->applyTransformation(transformMode, transform);
				break;
			}
			case Mode::SCALE: {
				// Compute scaling matrix based on mouse position and object distance
				const auto screenCenter = Vector2(width / 2.0, height / 2.0);
				const auto mousePos		= Vector2(mouseX, mouseY);
				const Matrix4 transform	= Matrix4::scale(
					direction													// Clamp direction
					* (static_cast<float>(screenCenter.distance(mousePos))		// Distance from mouse position to center of screen
					* (mesh->getPosition().distance(camPos) / scalingSens))		// Distance from Object to camera
					/ mesh->getScale()											// Difference from last transform
				);
				mesh->applyTransformation(transformMode, transform);
				break;
			}
			case Mode::ROTATE: {
				// Example for rotation: Calculate a basic rotation matrix for illustration
				constexpr float angle = 0.01f; // TODO (Replace with calculated rotation angle based on mouse input)

				const Matrix4 transformMatrix = Matrix4::rotateX(angle) * Matrix4::rotateY(angle) * Matrix4::rotateZ(angle);
				mesh->applyTransformation(transformMode, transformMatrix);
				break;
			}
			default: break;
		}
	}

	// Update lastTransform for next frame
	lastTransform = worldPos;
}

void SceneManager::applyTransformation() {
	if (transformMode != NONE) {
		lastTransform		  = Vector3::ZERO;	// Reset transformation data
		transformMode		  = NONE;			// Go back to View Mode
		transformMode.subMode = SubMode::NONE;
	}
}

void SceneManager::toggleSelectionMode() {
	selectionMode = selectionMode == OBJECT ? EDIT : OBJECT;
	context->selectionMode = selectionMode;
}

void SceneManager::setTransformMode(const Mode& mode) {
	if (selectedObjects.empty()) return;	// Don't change mode if no Object is selected
	transformMode = mode;
	transformMode.subMode = SubMode::NONE;		// Reset direction
}

void SceneManager::setTransformSubMode(const SubMode& subMode) {
	if (selectedObjects.empty()) return;	// Don't change mode if no Object is selected
	if (transformMode.type != ModeType::TRANSFORM) {
		transformMode.subMode = subMode;
	}
}

void SceneManager::toggleShadingMode() const {
	if (const auto meshes = getSelectedMeshes(); !meshes.empty()) {
		for (const auto& mesh : meshes) {
			mesh->setShadingMode(mesh->shadingMode == ShadingMode::SMOOTH ? ShadingMode::FLAT : ShadingMode::SMOOTH);
		}
	}
}
