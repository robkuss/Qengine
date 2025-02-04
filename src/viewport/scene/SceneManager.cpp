#include "SceneManager.h"

#include <iostream>

#include "Scene.h"
#include "viewport/Camera.h"
#include "objects/mesh/skybox/Skybox.cpp"

// Constants
constexpr float SCALING_SENS		= 0.001;	// Scaling sensitivity
constexpr float ROTATION_SENS		= 10.0f;	// Rotation sensitivity
constexpr float SELECT_TOLERANCE	= 20.0f;	// Tolerance in pixel distance for mouse picking

inline bool depthIsolation			= false;
inline bool fixedPosition			= false;
inline Vector3 lastTransform		= Vector3::ZERO;


// Pointers from Viewport
shared_ptr<array<int, 4>> SceneManager::viewport	= nullptr;
shared_ptr<Camera> SceneManager::activeCamera		= nullptr;
shared_ptr<Ray> SceneManager::mouseRay				= nullptr;

// Scene Management
vector<shared_ptr<Scene>> SceneManager::scenes				= vector<shared_ptr<Scene>>();
vector<shared_ptr<Object>> SceneManager::selectedObjects	= vector<shared_ptr<Object>>();

// Modes
Mode SceneManager::selectionMode = OBJECT;
Mode SceneManager::transformMode = NONE;

// Mouse data
double* SceneManager::mouseX = new double[1];
double* SceneManager::mouseY = new double[1];


void SceneManager::addScene(const shared_ptr<Scene>& scene) {
	scenes.emplace_back(scene);
}

void SceneManager::deleteScene(const shared_ptr<Scene>& scene) {
	scenes.erase(
		ranges::remove_if(scenes,
            [&scene](const weak_ptr<Scene>& weakScene) {
                const auto lockedScene = weakScene.lock();
                return lockedScene && lockedScene == scene; // Compare shared pointers
            }
		).begin(),
		scenes.end()
	);
}

void SceneManager::cleanupScenes() {
	scenes.erase(
		ranges::remove_if(scenes,
            [](const weak_ptr<Scene>& weakScene) {
                return weakScene.expired(); // Remove expired weak pointers
            }
		).begin(),
		scenes.end()
	);
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

void SceneManager::toggleShadingMode() {
	if (const auto meshes = getSelectedMeshes(); !meshes.empty()) {
		for (const auto& mesh : meshes) {
			mesh->setShadingMode(mesh->shadingMode == ShadingMode::SMOOTH
				? ShadingMode::FLAT
				: ShadingMode::SMOOTH);
		}
	}
}

Vector3 SceneManager::mouseWorld() {
	return unproject(
		Vector2(*mouseX, *mouseY),
		viewport.get(),
		activeCamera->viewMatrix,
		activeCamera->projMatrix
	);
}


// Scene rendering

void SceneManager::renderScenes() {
	for (const auto& scene : scenes) {
		scene->render(); // Call the render method on the scene
	}
}


// Object and Vertex operations that apply across all Scenes, e.g. selection

/**
 * Handles the selection of Objects in the scene based on the current mouse position.
 *
 * This function retrieves the mouse Ray using the current mouse coordinates and checks for
 * intersections with any Mesh in the scene. If a Mesh is intersected, it is selected
 * by the SceneManager. Additionally, the Ray's origin and calculated endpoint are logged
 * for debugging purposes.
 *
 * @see getMouseRay for the ray computation logic.
 */
void SceneManager::select(const Vector2& mousePos, const bool preserve) {
    const auto ray = mouseRay;

    // If in Object Mode, select entire Objects
    if (selectionMode == OBJECT) {
        // Find Objects that intersect with the mouse Ray
        vector<shared_ptr<Object>> intersectingObjects;
    	for (const auto& scene : scenes) {
    		for (const auto& obj : scene.get()->sceneObjects) {
    			// Attempt to cast Object to Mesh
    			if (const auto mesh = dynamic_cast<const Mesh*>(obj.get())) {
    				if (!dynamic_cast<const Skybox*>(obj.get())) {
    					if (ray->intersects(*mesh)) {
							intersectingObjects.emplace_back(obj);
						}
    				}
    			} else {
    				// TODO Implement selection logic for Objects that aren't Meshes (e.g. Cameras, light sources, etc.)
    			}
    		}
    	}


        // Select the Object that's closest to the Ray origin (the camera)
        if (!intersectingObjects.empty()) {
            selectObject(
                *ranges::min_element(
                    intersectingObjects,
                    [&ray](const shared_ptr<Object>& a, const shared_ptr<Object>& b) {
                        return a->position.distance(ray->origin) < b->position.distance(ray->origin);
                    }
                )
            );
        } else if (!preserve) {
            deselectAllObjects();
        }
    }

    // If in Edit Mode, select specific Vertices
    else if (selectionMode == EDIT) {
        // Find Vertices that intersect with the mouse Ray
        vector<shared_ptr<Vertex>> intersectingVertices;
        for (const auto& mesh : getSelectedMeshes()) {
            for (const auto& v : mesh->vertices) {
                if (Ray::intersects(project(
                        v->position,
                        viewport.get(),
                        activeCamera->viewMatrix,
                        activeCamera->projMatrix
                    ), mousePos, SELECT_TOLERANCE)) {
                    intersectingVertices.push_back(v);
                }
            }

            // Select the Vertex that's closest to the Ray origin (the camera)
            if (!intersectingVertices.empty()) {
            	selectVertex(
	                *ranges::min_element(
		                intersectingVertices,
		                [&ray](const shared_ptr<Vertex>& a, const shared_ptr<Vertex>& b) {
			                return a->position.distance(ray->origin) < b->position.distance(ray->origin);
		                }
	                )
                );
            } else if (!preserve) {
                deselectAllVertices();
            }
        }
    }

    // When transforming a Mesh, clicking applies the transformation
    applyTransformation();
}

bool SceneManager::isMeshSelected(const shared_ptr<Mesh>& mesh) {
	return ranges::any_of(getSelectedMeshes(), [&mesh](const shared_ptr<Mesh>& element) {
		return *element == *mesh;
	});
}

vector<shared_ptr<Mesh>> SceneManager::getSelectedMeshes() {
	return filterMeshes(selectedObjects);
}


void SceneManager::selectAllObjects(const vector<shared_ptr<Object>>& sceneObjects) {
	for (const auto& obj : sceneObjects) {
		selectedObjects.emplace_back(obj);
	}
}

void SceneManager::deselectAllObjects() {
	selectedObjects.clear();
}

void SceneManager::selectAllVertices(const shared_ptr<Mesh>& mesh) {
	for (const auto& v : mesh->vertices) {
		v->isSelected = true;
	}
}

void SceneManager::deselectAllVertices() {
	for (const auto& mesh : getSelectedMeshes()) {
		for (const auto& v : mesh->vertices) {
			v->isSelected = false;
		}
	}
}

void SceneManager::selectObject(const shared_ptr<Object>& obj) {
	// Check if the object is already selected
	if (ranges::find(selectedObjects, obj) == selectedObjects.end()) {
		// Add the object to selectedObjects and stop searching
		selectedObjects.emplace_back(obj);
	} else {
		// If the object is already selected, deselect it
		deselectObject(obj);
	}
}

void SceneManager::selectObject(const string& label) {
	// Check if the object is already selected
	const auto it = ranges::find_if(selectedObjects, [&label](const auto& obj) {
		return obj->name == label;
	});

	if (it == selectedObjects.end()) {
		// If not already selected, search through all scenes for the object with the given label
		for (const auto& scene : scenes) {
			auto objIt = ranges::find_if(scene->sceneObjects, [&label](const auto& obj) {
				return obj->name == label;
			});

			if (objIt != scene->sceneObjects.end()) {
				// Add the object to selectedObjects and stop searching
				selectedObjects.emplace_back(*objIt);
				return;
			}
		}
	} else {
		// If the object is already selected, deselect it
		deselectObject(*it);
	}
}

void SceneManager::deselectObject(const shared_ptr<Object>& obj) {
	if (const auto it = ranges::find(selectedObjects, obj); it != selectedObjects.end()) {
		selectedObjects.erase(it);
	}
}

void SceneManager::selectVertex(const shared_ptr<Vertex>& v) {
	v->isSelected = !v->isSelected;
}


/** Scene-side part of Object transformation
 * (User input processing, environment-dependant calculations)
 */
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
			 project(mesh->position, viewport.get(), activeCamera->viewMatrix, activeCamera->projMatrix)
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
				const float scaleFactor = camDist * mouseDist * SCALING_SENS;
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
				const float angle = dPos.length() * ROTATION_SENS;

				// Create rotation matrix
				Matrix4 rotation =
					  Matrix4::rotateX(angle * direction.x)
					* Matrix4::rotateY(angle * direction.y)
					* Matrix4::rotateZ(angle * direction.z);

				// Apply rotation
				mesh->applyTransformation(transformMode, transformMode, rotation);
				break;
			}
			default: throw invalid_argument("Invalid transformation: Wrong Mode");
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
