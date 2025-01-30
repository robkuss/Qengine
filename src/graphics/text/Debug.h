#pragma once

using namespace std;

#include <iostream>

#include "viewport/Viewport.h"

// Options
#define DEBUG	// For on-screen debug text

constexpr float debugTextSize	= 24;
const Color debugTextColor		= Colors::TEXT_COLOR;


class Debug {
public:
	static void drawDebugText() {
		shared_ptr<Scene> foreground;
		for (const auto& scene : SceneManager::scenes) {
			if (scene->name == "Foreground") foreground = scene;
		}
		if (!foreground) cerr << "Foreground Scene not found" << endl;

		const auto camera = SceneManager::activeCamera;
		const auto cube = foreground->sceneObjects[0];
		const auto mouseWorld = SceneManager::mouseWorld();

		size_t vertexCount = 0;
		for (const auto& obj : foreground->sceneObjects) {
			vertexCount += dynamic_cast<Mesh*>(obj.get())->vertices.size();
		}

		for (int i = 0; i <= 11; i++) {
			ostringstream out;

			switch (i) {
				case 0:  out << "FPS: " << fps; break;
				case 1:  out << "Camera Pos: " << camera->camPos.toString(); break;
				case 2:  out << "Camera Rot: " << fixed << setprecision(1) << camera->rotH << " / " << camera->rotV; break;
				case 3:  out << "Zoom: " << fixed << setprecision(3) << camera->camDist; break;
				case 4:  out << "Mouse Screen: " << *SceneManager::mouseX  << " / " << *SceneManager::mouseY; break;
				case 5:  out << "Mouse World: "  << mouseWorld.toString(); break;
				case 6:	 out << "Mode: " << SceneManager::selectionMode.modeToString();
				if (SceneManager::transformMode.mode    != Mode::NONE)    out << " " << SceneManager::transformMode.modeToString();
				if (SceneManager::transformMode.subMode != SubMode::NONE) out << " " << SceneManager::transformMode.subModeToString(); break;
				case 7:  out << "Cube:"; break;
				case 8:  out << "    Pos: "   << cube->position.toString();  break;
				case 9:  out << "    Scale: " << cube->scale.toString();     break;
				case 10: out << "    Rot: "   << cube->rotationEuler.toString(); break;
				default: out << "Vertex Count: " << vertexCount; break;
			}

			Text::renderText(out.str(), TextMode::LEFT, UI::firstLineX, Text::line(i, debugTextSize), debugTextSize, debugTextColor);
		}
	}
};
