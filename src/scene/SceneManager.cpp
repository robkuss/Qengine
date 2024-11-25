#include "SceneManager.h"

#include "math/matrix/Matrix4.h"
#include "math/vector/Vector2.h"
#include "scene/graphics/MeshRenderer.h"

#include <memory>   // For std::shared_ptr

SceneManager::SceneManager() {
	// Add Default Cube to scene
	const auto cube = std::make_shared<Cube>("Cube", 1.0f);
	cube->setPosition(Vector3(0.5f, 0.5f, 0.5f));
	cube->setScale(Vector3::ONE);
	cube->setRotation(Vector3::ZERO);
	addObject(cube);
}

void SceneManager::render(const Vector3 camPos) const {
	// Loop through the sceneObjects and render Mesh instances
	for (const auto& obj : sceneObjects) {
		// Check if the current Object is selected
		const bool isSelected = selectedObject && selectedObject == obj;

		// Attempt to cast Object to Mesh using dynamic_cast
		if (const auto mesh = dynamic_cast<Mesh*>(obj.get())) {
			MeshRenderer::render(*mesh, camPos, isSelected, viewportMode.mode == Mode::EDIT);
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
					return a->getPosition().distance(ray.origin) < b->getPosition().distance(ray.origin);
				}
			)
		);
	} else selectObject(nullptr);	// No Object selected

	// When transforming an Object, clicking applies the transformation
	if (transformMode.mode != Mode::NONE) {
		lastTransform		  = Vector3::ZERO;	// Reset transformation data
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

	// Define direction based on subMode
	Vector3 direction = Vector3::ZERO;
	switch (transformMode.subMode) {
		case SubMode::NONE: direction = Vector3::ONE; break;
		case SubMode::X:	direction = Vector3(1, 0, 0); break;
		case SubMode::Y:	direction = Vector3(0, 1, 0); break;
		case SubMode::Z:	direction = Vector3(0, 0, 1); break;
	}
    const Vector3 dirWorldPos = direction * worldPos;		// I forgot why

    switch (transformMode.mode) {
        case Mode::GRAB: {
        	lastTransform = lastTransform == Vector3::ZERO ? dirWorldPos : lastTransform;					// Ensure last transformation is non-zero
        	const float grabZ = (mesh->getPosition() - camPos).length();									// Get distance of the Object from the camera
	        const Matrix4 transformMatrix = Matrix4::translate((dirWorldPos - lastTransform) * grabZ);	// Calculate transformation matrix
        	lastTransform = dirWorldPos;																	// Store new transformation for next frame
        	mesh->applyTransformation(transformMode.mode, transformMatrix);
            break;
        }
        case Mode::SCALE: {
        	// Compute scaling factor based on mouse position and object distance
        	const float objDist     = mesh->getPosition().distance(camPos);
        	const auto screenCenter = Vector2(width / 2.0, height / 2.0);
        	const auto mousePos     = Vector2(mouseX, mouseY);
        	const float scale       = static_cast<float>(screenCenter.distance(mousePos)) * (objDist / scaleSens);

        	const Vector3 currentScale = mesh->getScale();													// Retrieve the current scale of the Mesh
        	const Vector3 newScale = direction * scale / currentScale;										// Scale factor adjustment
	        const Matrix4 transformMatrix = Matrix4::scale(newScale);										// Calculate transformation matrix
        	mesh->applyTransformation(transformMode.mode, transformMatrix);
        	break;
        }
        case Mode::ROTATE: {
            // Example for rotation: Calculate a basic rotation matrix for illustration
            // Assume rotation about Z-axis as an example (replace with desired axis/mouse inputs)
            constexpr float angle = 0.01f; // TODO (Replace with calculated rotation angle based on mouse input)

            const Matrix4 transformMatrix = Matrix4::rotateZ(angle);
        	mesh->applyTransformation(transformMode.mode, transformMatrix);
            break;
        }
    	default: break;
    }
}

void SceneManager::toggleViewportMode() {
	switch (viewportMode.mode) {
		case Mode::OBJECT: viewportMode.mode = Mode::EDIT; break;
		case Mode::EDIT: viewportMode.mode = Mode::OBJECT; break;
		default: {}
	}
}

void SceneManager::setTransformMode(const Mode::ModeEnum mode) {
	if (!selectedObject) return;	// Don't change mode if no Object is selected
	transformMode.mode = mode;
	transformMode.subMode = SubMode::NONE;		// Reset direction
}

void SceneManager::setTransformSubMode(const SubMode subMode) {
	if (!selectedObject) return;	// Don't change mode if no Object is selected
	if (transformMode.type != ModeType::TRANSFORM) {
		transformMode.subMode = subMode;
	}
}
