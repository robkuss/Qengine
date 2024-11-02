#include "SceneManager.h"

#include "../scene/graphics/MeshRenderer.h"
#include "../math/vector/Vector2.cpp"

#include <memory>   // For std::shared_ptr


void SceneManager::render(const Mode mode, const Vector3 camPos) const {
	// Loop through the sceneObjects and render Mesh instances
	for (const auto& obj : sceneObjects) {
		// Check if the current Object is selected
		const bool isSelected = selectedObject && selectedObject == obj;

		// Attempt to cast Object to Mesh using dynamic_cast
		if (const auto mesh = dynamic_cast<const Mesh*>(obj.get())) {
			MeshRenderer::render(*mesh, camPos, isSelected, mode.mode == Mode::EDIT);
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
void SceneManager::select(const Ray& ray) {
	#ifdef DRAW_MOUSE_RAY
		// Update the visual Mouse Ray
		const auto directionScaled = ray.direction * MOUSE_RAY_LENGTH;
		rayStart = ray.origin;
		rayEnd   = ray.origin + directionScaled;
	#endif

	// Find Objects that intersect with the Mouse Ray
	std::vector<std::shared_ptr<Object>> intersectingObjects;
	intersectingObjects.reserve(sceneObjects.size());

	for (const auto& obj : sceneObjects) {
		// Attempt to cast Object to Mesh using dynamic_cast
		if (const auto mesh = dynamic_cast<Mesh*>(obj.get())) {
			if (ray.intersects(mesh->getTriangles())) {
				intersectingObjects.push_back(obj);
			}
		} else throw std::runtime_error("Selected Object is not a Mesh!");
	}

	// Select the Object that's closest to the ray origin (the camera)
	if (!intersectingObjects.empty()) {
		selectObject(
			*std::ranges::min_element(
				intersectingObjects,
				[&ray](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
					return a->position.distance(ray.origin) < b->position.distance(ray.origin);
				}
			)
		);
	} else selectObject(nullptr);	// No Object selected

	// When transforming an Object, clicking applies the transformation
	if (transformMode.mode != Mode::NONE) {
		transformation		  = Vector3::ZERO;	// Reset transformation vector
		lastTransform		  = Vector3::ZERO;
		transformMode.mode	  = Mode::NONE;		// Go back to View Mode
		transformMode.subMode = SubMode::NONE;
	}
}

void SceneManager::transform(const double mouseX, const double mouseY, const int width, const int height, const Vector3 worldPos, const Vector3 camPos) {
	// Get the selected Object
	const auto obj = selectedObject.get();
	if (!obj) {
		//text->setErrorText("No object selected.");
		return;
	}
	const auto mesh = dynamic_cast<Mesh*>(obj);
	if (!mesh) {
		//text->setErrorText("Selected Object is not a Mesh.");
		return;
	}

	switch (transformMode.mode) {
		case Mode::GRAB: {
			// Truncate world position vector based on the selected direction (Transformation Sub Mode)
			Vector3 wpDirectional = Vector3::ZERO;
			switch (transformMode.subMode) {
				case SubMode::NONE: wpDirectional = worldPos; break;
				// TODO: Make the unidirectional translation also follow the cursor
				case SubMode::X: wpDirectional = Vector3(worldPos.x, 0, 0); break;
				case SubMode::Y: wpDirectional = Vector3(0, worldPos.y, 0); break;
				case SubMode::Z: wpDirectional = Vector3(0, 0, worldPos.z); break;
			}

			const float grabZ = (mesh->position - camPos).length();						// Get distance of the object from the camera
			lastTransform  = lastTransform == Vector3::ZERO ? wpDirectional : lastTransform;	// Ensure last transformation is non-zero
			transformation = (wpDirectional - lastTransform) * grabZ;							// Calculate transformation vector
			mesh->applyTransformation(transformMode.mode, transformation);						// Apply transformation
			lastTransform  = wpDirectional;
			break;
		}
		case Mode::SCALE: {
			const auto screenCenter = Vector2(static_cast<float>(width) / 2, static_cast<float>(height) / 2);
			const auto mousePos = Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY));
			const float scale = screenCenter.distance(mousePos) / 100.0f;
			mesh->applyTransformation(transformMode.mode, Vector3(scale, scale, scale));
			break;
		}
		case Mode::ROTATE: {
			return;
		}
		case Mode::EXTRUDE: {
			return;
		}
		case Mode::FILL: {
			return;
		}
		case Mode::MERGE: {
			return;
		}
		default: /*text->setErrorText("Error: Invalid transform mode.")*/;
	}
}

void SceneManager::changeTransformMode(const Mode::ModeEnum mode) {
	if (!selectedObject) return;	// Don't change mode if no Object is selected
	transformMode.mode = mode;
	transformMode.subMode = SubMode::NONE;		// Reset direction
}

void SceneManager::changeTransformSubMode(const SubMode subMode) {
	if (!selectedObject) return;	// Don't change mode if no Object is selected
	if (transformMode.type != ModeType::TRANSFORM) {
		transformMode.subMode = subMode;
	}
}
