#pragma once

#include <viewport/scene/Scene.h>

// Options
#define DEBUG	// For on-screen debug text

class Debug {
public:
	static void drawDebugText(const Scene* scene, const Camera& camera, const int fps) {
		const auto cube = scene->sceneObjects[0];
		const auto mouseWorld = scene->mouseWorld();

		size_t vertexCount = 0;
		for (const auto& obj : scene->sceneObjects) {
			vertexCount += dynamic_cast<Mesh*>(obj.get())->vertices.size();
		}

		for (int i = 0; i <= 11; i++) {
			std::ostringstream out;

			switch (i) {
				case 0:  out << "FPS: " << fps; break;
				case 1:  out << "Camera Pos: " << camera.camPos.toString(); break;
				case 2:  out << "Camera Rot: " << std::fixed << std::setprecision(1) << camera.rotH << " / " << camera.rotV; break;
				case 3:  out << "Zoom: " << std::fixed << std::setprecision(3) << camera.camDist; break;
				case 4:  out << "Mouse Screen: " << *scene->mouseX  << " / " << *scene->mouseY; break;
				case 5:  out << "Mouse World: "  << mouseWorld.toString(); break;
				case 6:	 out << "Mode: " << scene->selectionMode.modeToString();
				if (scene->transformMode.mode    != Mode::NONE)    out << " " << scene->transformMode.modeToString();
				if (scene->transformMode.subMode != SubMode::NONE) out << " " << scene->transformMode.subModeToString(); break;
				case 7:  out << "Cube:"; break;
				case 8:  out << "    Pos: "   << cube->position.toString();  break;
				case 9:  out << "    Scale: " << cube->scale.toString();     break;
				case 10: out << "    Rot: "   << cube->rotationEuler.toString(); break;
				default: out << "Vertex Count: " << vertexCount; break;
			}

			Text::renderText(out.str(), TextMode::LEFT, UI::firstLineX, Text::line(i, 24), 24, Colors::TEXT_COLOR);
		}
	}
};
