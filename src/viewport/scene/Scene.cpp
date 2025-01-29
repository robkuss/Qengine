#define GLFW_INCLUDE_GLEXT

#include "Scene.h"

#include <iostream>
#include <memory>

#include <graphics/MeshRenderer.h>
#include <graphics/ui/UISceneManager.h>
#include <math/ray/Ray.h>
#include <viewport/Camera.h>


void Scene::addObject(const std::shared_ptr<Object>& obj) {
	sceneObjects.emplace_back(obj);
	UISceneManager::update();
}

void Scene::removeObject(const std::shared_ptr<Object>& obj) {
	sceneObjects.erase(std::ranges::find(sceneObjects, obj));
	UISceneManager::update();
}


void Scene::render() {
	if (fixedPosition) {
		// Load view matrix for background
		SceneManager::activeCamera->loadFixedViewMatrix();
	} else {
		// Load view matrix for foreground
		SceneManager::activeCamera->loadViewMatrix();
	}

	// Enable lighting
	glEnable(GL_LIGHTING);
	for (const auto& light : lights) {
		glEnable(light->macro);
		glLightfv(light->macro, GL_POSITION, light->pos.data());
		glLightfv(light->macro, GL_AMBIENT, light->ambient);
		glLightfv(light->macro, GL_DIFFUSE, light->diffuse);
		glLightfv(light->macro, GL_SPECULAR, light->specular);
	}

	const auto& sceneMeshes = filterMeshes(sceneObjects);
	const auto& selectedVertices = SceneManager::selectedVertices;
	const auto selectionMode = SceneManager::selectionMode;
	const auto camPos = SceneManager::activeCamera->camPos;

	// Loop through the sceneObjects and render Mesh instances
	for (const auto& mesh : sceneMeshes) {
		MeshRenderer::render(*mesh, selectedVertices, selectionMode, SceneManager::isMeshSelected(mesh));
	}

	// Disable lighting for outline rendering
	for (const auto& light : lights) {
		glDisable(light->macro);
	}
	glDisable(GL_LIGHTING);

	// Loop through the sceneObjects and render Mesh outlines
	for (const auto& mesh : sceneMeshes) {
		MeshRenderer::renderSilhouette(*mesh, selectionMode, camPos, SceneManager::isMeshSelected(mesh));
	}

	if (depthIsolation) {
		glClear(GL_DEPTH_BUFFER_BIT);
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

	light->diffuse[0] = diffuse.red();
	light->diffuse[1] = diffuse.green();
	light->diffuse[2] = diffuse.blue();

	light->ambient[0] = ambient.red();
	light->ambient[1] = ambient.green();
	light->ambient[2] = ambient.blue();

	light->specular[0] = specular.red();
	light->specular[1] = specular.green();
	light->specular[2] = specular.blue();

	lights.emplace_back(light);

	UISceneManager::update();
}

void Scene::enableDepthIsolation() {
	depthIsolation = true;
}

void Scene::enableFixedPosition() {
	fixedPosition = true;
}
