#include "UI.h"

#include "math/Util.h"

#include "objects/mesh/cube/Cube.cpp"
#include "objects/mesh/sphere/Sphere.cpp"
#include <objects/light/Sun.h>


void UI::setButtonOnClickEvents(const std::shared_ptr<UIOptionList>& tab) {
	setOnClickForButton(tab, "Exit", [] {
		exit(0);
	});

	setOnClickForButton(tab, "Cube", [] {
		SceneManager::scenes[0]->addObject(
			std::make_shared<Cube>(
				"Demo Cube",
				Vector3(0.0f, 0.0f, 0.0f),
				1.0f,
				Colors::WHITE,
				std::shared_ptr<Texture>{}
			)
		);
	});

	setOnClickForButton(tab, "Sphere", [] {
		SceneManager::scenes[0]->addObject(
			std::make_shared<Sphere>(
				"Demo Sphere",
				Vector3(0.0f, 0.0f, 0.0f),
				0.5f,
				64,
				32,
				Colors::WHITE,
				std::shared_ptr<Texture>{}
			)
		);
	});

	setOnClickForButton(tab, "Point", [] {
		// TODO make Point lights
	});

	setOnClickForButton(tab, "Sun", [] {
		std::array lightPos = {-2.0f, -3.0f, -6.0f, 0.0f};
		SceneManager::scenes[0]->addLight(
			std::make_shared<Sun>(
				GL_LIGHT2,
				lightPos
			),
			Colors::LIGHT_SUN,
			Colors::LIGHT_AMBIENT,
			Colors::WHITE
		);
	});
}
