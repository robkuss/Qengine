#define GLFW_INCLUDE_GLEXT

#include "Scene.h"

#include <memory>
#include <math/ray/Ray.h>

#include "Camera.h"
#include "graphics/MeshRenderer.h"

constexpr float SELECT_TOLERANCE = 20.0f;		// Tolerance in pixel distance for mouse picking


Scene::Scene() {
	context = new RenderContext(selectionMode);
}

void Scene::render() const {
	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	// Loop through the sceneObjects and render Mesh instances
	for (const auto& obj : sceneObjects) {
		if (const auto mesh = dynamic_cast<Mesh*>(obj.get())) {
			MeshRenderer::render(*mesh, *context);
		}
	}

	// Disable lighting after rendering
	glDisable(GL_LIGHTING);

	// Loop through the sceneObjects and render Mesh instances
	for (const auto& obj : sceneObjects) {
		if (const auto mesh = dynamic_cast<Mesh*>(obj.get())) {
			MeshRenderer::renderSilhouette(*mesh, *context);
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
void Scene::select(const Vector2& mousePos, const bool preserve) {
	const auto ray = context->mouseRay;

	// If in Object Mode, select entire Objects
	if (selectionMode == OBJECT) {
		// Find Objects that intersect with the mouse Ray
		std::vector<std::shared_ptr<Object>> intersectingObjects;
		for (const auto& obj : sceneObjects) {
			// Attempt to cast Object to Mesh
			if (const auto mesh = dynamic_cast<const Mesh*>(obj.get())) {
				if (ray->intersects(*mesh)) {
					intersectingObjects.emplace_back(obj);
				}
			} else {
				// TODO Implement selection logic for Objects that aren't Meshes
			}
		}

		// Select the Object that's closest to the Ray origin (the camera)
		if (!intersectingObjects.empty()) {
			selectObject(
				*std::ranges::min_element(
					intersectingObjects,
					[&ray](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
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
		std::vector<Vertex> intersectingVertices;
		for (const auto& mesh : getSelectedMeshes()) {
			for (const auto& v : mesh->vertices) {
				if (Ray::intersects(project(
					v->position,
					context->viewport,
					context->activeCamera->viewMatrix,
					context->activeCamera->projMatrix
				), mousePos, SELECT_TOLERANCE)) {
					intersectingVertices.emplace_back(*v);
				}
			}

			// Select the Vertex that's closest to the Ray origin (the camera)
			if (!intersectingVertices.empty()) {
				selectVertex(
					*std::ranges::min_element(
						intersectingVertices,
						[&ray](const Vertex& a, const Vertex& b) {
							return a.position.distance(ray->origin) < b.position.distance(ray->origin);
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

void Scene::selectAllObjects() {
	for (const auto& obj : sceneObjects) {
		selectedObjects.emplace_back(obj);
	}
	context->selectedObjects = selectedObjects;
}

void Scene::deselectAllObjects() {
	selectedObjects.clear();
	context->selectedObjects.clear();
}

void Scene::selectAllVertices(const std::shared_ptr<Mesh>& mesh) {
	for (const auto& v : mesh->vertices) {
		selectedVertices.emplace_back(*v);
	}
	context->selectedVertices = selectedVertices;
}

void Scene::deselectAllVertices() {
	selectedVertices.clear();
	context->selectedVertices.clear();
}

void Scene::selectObject(const std::shared_ptr<Object>& obj) {
	if (std::ranges::find(selectedObjects, obj) == selectedObjects.end()) {
		selectedObjects.emplace_back(obj);
		context->selectedObjects = selectedObjects;
	} else {
		deselectObject(obj);
	}
}

void Scene::deselectObject(const std::shared_ptr<Object>& obj) {
	if (const auto it = std::ranges::find(selectedObjects, obj);
			it != selectedObjects.end()) {
		selectedObjects.erase(it);
		context->selectedObjects = selectedObjects;
	}
}

void Scene::selectVertex(const Vertex& v) {
	if (std::ranges::find(selectedVertices, v) == selectedVertices.end()) {
		selectedVertices.emplace_back(v);
		context->selectedVertices = selectedVertices;
	} else {
		deselectVertex(v);
	}
}

void Scene::deselectVertex(const Vertex& v) {
	if (const auto it = std::ranges::find(selectedVertices, v);
			it != selectedVertices.end()) {
		selectedVertices.erase(it);
		context->selectedVertices = selectedVertices;
	}
}

std::vector<std::shared_ptr<Mesh>> Scene::getSelectedMeshes() const {
	std::vector<std::shared_ptr<Mesh>> meshes;
	for (const auto& obj : selectedObjects) {
		// Check if the object is a Mesh by using dynamic_pointer_cast
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(obj)) {
			meshes.emplace_back(mesh);
		}
	}
	return meshes;
}

void Scene::transform(
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
			 project(mesh->position, context->viewport, context->activeCamera->viewMatrix, context->activeCamera->projMatrix)
			.distance(Vector2(mouseX, mouseY))
		);

		switch (transformMode.mode) {
			case Mode::GRAB: {
				const Matrix4 transform = Matrix4::translate(
					direction * camDist	// Clamp direction
					* dPos				// Difference from last transform
				);
				mesh->applyTransformation(selectionMode, transformMode, transform);
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
				mesh->applyTransformation(selectionMode, transformMode, transform);
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
				mesh->applyTransformation(selectionMode, transformMode, rotation);
				break;
			}
			default: throw std::invalid_argument("Invalid transformation: Wrong Mode");
		}
	}

	// Update lastTransform for next frame
	lastTransform = worldPos;
}

void Scene::applyTransformation() {
	if (transformMode != NONE) {
		lastTransform		  = Vector3::ZERO;	// Reset transformation data
		transformMode		  = NONE;			// Go back to View Mode
		transformMode.subMode = SubMode::NONE;	// Reset transformation direction
	}
}

void Scene::toggleSelectionMode() {
	if (selectionMode == OBJECT) {
		// Don't change mode if no Object is selected
		if (!selectedObjects.empty()) {
			selectionMode = EDIT;
		}
	} else if (selectionMode == EDIT) {
		selectionMode = OBJECT;
	}
	context->selectionMode = selectionMode;

	// Reset vertex selection data
	deselectAllVertices();
}

void Scene::setTransformMode(const Mode& mode) {
	// Don't change mode if no Object is selected
	if (selectedObjects.empty()) return;
	transformMode = mode;

	// Reset transformation direction
	transformMode.subMode = SubMode::NONE;
}

void Scene::setTransformSubMode(const SubMode& subMode) {
	// Don't change mode if no Object is selected
	if (selectedObjects.empty() || transformMode.type != ModeType::TRANSFORM) return;

	// Set transformation direction
	transformMode.subMode = subMode;
}

void Scene::toggleShadingMode() const {
	if (const auto meshes = getSelectedMeshes(); !meshes.empty()) {
		for (const auto& mesh : meshes) {
			mesh->setShadingMode(mesh->shadingMode == ShadingMode::SMOOTH
				? ShadingMode::FLAT
				: ShadingMode::SMOOTH);
		}
	}
}

void Scene::setLight(const Color& diffuse, const Color& ambient, const Color& specular) {
	constexpr float noLight[4] = {0, 0, 0, 1};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, noLight);

	const float diffuseF[3] = {diffuse.red(), diffuse.green(), diffuse.blue()};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseF);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseF);

	const float ambientF[3] = {ambient.red(), ambient.green(), ambient.blue()};
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientF);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientF);

	const float specularF[3] = {specular.red(), specular.green(), specular.blue()};
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularF);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specularF);
}

Vector3 Scene::mouseWorld() const {
	return unproject(
		Vector2(*mouseX, *mouseY),
		context->viewport,
		context->activeCamera->viewMatrix,
		context->activeCamera->projMatrix
	);
}
