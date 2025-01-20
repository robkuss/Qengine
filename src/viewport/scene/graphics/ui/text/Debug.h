#pragma once

#include <viewport/scene/Scene.h>

// Options
#define DEBUG	// For on-screen debug text

class Debug {
public:
	static void drawDebugText(const Viewport *vp) {
		const auto cube = vp->scene->sceneObjects[0];
		const auto mouseWorld = unproject(Vector2(*vp->activeCamera.mouseX, *vp->activeCamera.mouseY), &vp->viewport, vp->activeCamera.viewMatrix, vp->activeCamera.projMatrix);

		size_t vertexCount = 0;
		for (const auto& obj : vp->scene->sceneObjects) {
			vertexCount += dynamic_cast<Mesh*>(obj.get())->vertices.size();
		}

		for (int i = 0; i <= 11; i++) {
			std::ostringstream out;

			switch (i) {
				case 0:  out << "FPS: " << vp->fps; break;
				case 1:  out << "Camera Pos: " << vp->activeCamera.camPos.toString(); break;
				case 2:  out << "Camera Rot: " << std::fixed << std::setprecision(1) << vp->activeCamera.rotH << " / " << vp->activeCamera.rotV; break;
				case 3:  out << "Zoom: " << std::fixed << std::setprecision(3) << vp->activeCamera.camDist; break;
				case 4:  out << "Mouse Screen: " << vp->activeCamera.mouseX[0]  << " / " << vp->activeCamera.mouseY[0]; break;
				case 5:  out << "Mouse World: "  << mouseWorld.toString(); break;
				case 6:	 out << "Mode: " << vp->scene->selectionMode.modeToString();
				if (vp->scene->transformMode.mode    != Mode::NONE)    out << " " << vp->scene->transformMode.modeToString();
				if (vp->scene->transformMode.subMode != SubMode::NONE) out << " " << vp->scene->transformMode.subModeToString(); break;
				case 7:  out << "Cube:"; break;
				case 8:  out << "    Pos: "   << cube->position.toString();  break;
				case 9:  out << "    Scale: " << cube->scale.toString();     break;
				case 10: out << "    Rot: "   << cube->rotationEuler.toString(); break;
				default: out << "Vertex Count: " << vertexCount; break;
			}

			Text::renderText(out.str(), TextMode::LEFT, UI::firstLineX, Text::line(i), Text::fontSize, Colors::TEXT_COLOR);
		}
	}
};
