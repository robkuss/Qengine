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


void Scene::render() const {
	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
	glLightfv(GL_LIGHT2, GL_POSITION, light2Pos);

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
