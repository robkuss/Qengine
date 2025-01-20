#define GLFW_INCLUDE_GLEXT

#include "Scene.h"

#include <memory>
#include <math/ray/Ray.h>

#include "Camera.h"
#include "graphics/MeshRenderer.h"

constexpr float SELECT_TOLERANCE = 20.0f;		// Tolerance in pixel distance for mouse picking


void Scene::render() const {
	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
	glLightfv(GL_LIGHT2, GL_POSITION, light2Pos);

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
void Scene::select(const Vector2& mousePos, const bool preserve) const {
	const auto ray = context->mouseRay;

	// If in Object Mode, select entire Objects
	if (context->selectionMode == OBJECT) {
		// Find Objects that intersect with the mouse Ray
		std::vector<std::shared_ptr<Object>> intersectingObjects;
		for (const auto& obj : sceneObjects) {
			// Attempt to cast Object to Mesh
			if (const auto mesh = dynamic_cast<const Mesh*>(obj.get())) {
				if (ray->intersects(*mesh)) {
					intersectingObjects.emplace_back(obj);
				}
			} else {
				// TODO Implement selection logic for Objects that aren't Meshes (e.g. Cameras, light sources etc.)
			}
		}

		// Select the Object that's closest to the Ray origin (the camera)
		if (!intersectingObjects.empty()) {
			context->selectObject(
				*std::ranges::min_element(
					intersectingObjects,
					[&ray](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
						return a->position.distance(ray->origin) < b->position.distance(ray->origin);
					}
				)
			);
		} else if (!preserve) {
			context->deselectAllObjects();
		}
	}

	// If in Edit Mode, select specific Vertices
	else if (context->selectionMode == EDIT) {
		// Find Vertices that intersect with the mouse Ray
		std::vector<Vertex> intersectingVertices;
		for (const auto& mesh : context->getSelectedMeshes()) {
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
				context->selectVertex(
					*std::ranges::min_element(
						intersectingVertices,
						[&ray](const Vertex& a, const Vertex& b) {
							return a.position.distance(ray->origin) < b.position.distance(ray->origin);
						}
					)
				);
			} else if (!preserve) {
				context->deselectAllVertices();
			}
		}
	}

	// When transforming a Mesh, clicking applies the transformation
	context->applyTransformation();
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

Vector3 Scene::mouseWorld(const double mouseX, const double mouseY) const {
	return unproject(
		Vector2(mouseX, mouseY),
		context->viewport,
		context->activeCamera->viewMatrix,
		context->activeCamera->projMatrix
	);
}
