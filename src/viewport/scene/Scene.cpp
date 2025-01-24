#define GLFW_INCLUDE_GLEXT

#include "Scene.h"

#include <iostream>
#include <memory>
#include <math/ray/Ray.h>

#include "viewport/Camera.h"
#include "graphics/MeshRenderer.h"


void Scene::addObject(const std::shared_ptr<Object>& obj) {
	sceneObjects.emplace_back(obj);
}

void Scene::removeObject(const std::shared_ptr<Object>& obj) {
	sceneObjects.erase(std::ranges::find(sceneObjects, obj));
}


void Scene::render() {
	// Enable lighting
	glEnable(GL_LIGHTING);

	for (const auto& light : lights) {
		glEnable(light->macro);
		glLightfv(light->macro, GL_POSITION, light->pos.data());
	}

	const auto& sceneMeshes = filterMeshes(sceneObjects);
	const auto& selectedVertices = SceneManager::selectedVertices;
	const auto selectionMode = SceneManager::selectionMode;
	const auto camPos = SceneManager::activeCamera->camPos;

	// Loop through the sceneObjects and render Mesh instances
	for (const auto& mesh : sceneMeshes) {
		MeshRenderer::render(*mesh, selectedVertices, selectionMode, SceneManager::isMeshSelected(mesh));
	}

	// Disable lighting after rendering
	glDisable(GL_LIGHTING);

	// Loop through the sceneObjects and render Mesh instances
	for (const auto& mesh : sceneMeshes) {
		MeshRenderer::renderSilhouette(*mesh, selectionMode, camPos, SceneManager::isMeshSelected(mesh));
	}
}

void Scene::addLight(
	const std::shared_ptr<Light>& light,
	const Color& diffuse,
	const Color& ambient,
	const Color& specular
) {
	// If this GL_LIGHT has already been set, return
	if (std::ranges::any_of(lights, [&](const auto& l) { return l->macro == light->macro; })) {
		std::cerr << "Light " << light->macro << " has already been set" << std::endl;
		return;
	}

	lights.emplace_back(light);

	constexpr float noLight[4] = {0, 0, 0, 1};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, noLight);

	const float diffuseF[3] = {diffuse.red(), diffuse.green(), diffuse.blue()};
	const float ambientF[3] = {ambient.red(), ambient.green(), ambient.blue()};
	const float specularF[3] = {specular.red(), specular.green(), specular.blue()};

	glLightfv(light->macro, GL_DIFFUSE, diffuseF);
	glLightfv(light->macro, GL_AMBIENT, ambientF);
	glLightfv(light->macro, GL_SPECULAR, specularF);
}
