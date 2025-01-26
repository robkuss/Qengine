#include <algorithm>
#include <iostream>

#include "UI.h"

#include "math/Util.h"

#include "objects/mesh/cube/Cube.cpp"
#include "objects/mesh/sphere/Sphere.cpp"
#include <objects/light/Sun.h>

#include "UIOption.h"


bool UI::setOnClickForButton(	// NOLINT(*-no-recursion)
	const std::shared_ptr<UIOptionList>& list,
	const std::string& label,
	const std::function<void()>& onClickAction
) {
	if (!list) return false;

	// Check the button of the current UIOptionList
	if (list->button && list->button->label == label) {
		list->button->onClick = onClickAction;
		return true;
	}

	// Check buttons in all UIOptions in the current list
	for (const auto& innerOption : list->options) {
		if (!innerOption) continue;
		const auto button =
			  std::holds_alternative<UIOption>(*innerOption)
			? std::get<UIOption>(*innerOption).button
			: std::get<UIOptionList>(*innerOption).button;

		if (button && button->label == label) {
			button->onClick = onClickAction;
			return true;
		}
	}

	// Recursively check all nested UIOptionLists
	if (std::ranges::any_of(list->options, [&](const auto& nestedList) {
		if (std::holds_alternative<UIOptionList>(*nestedList)) {
			const auto nestedOptionList = std::make_shared<UIOptionList>(std::get<UIOptionList>(*nestedList));
			return setOnClickForButton(nestedOptionList, label, onClickAction);
		}
		return false;
	})) return true;

	return false;	// Return false if no matching button was found
}


void UI::setButtonOnClickEvents(const std::shared_ptr<UIOptionList>& tab) {
	std::shared_ptr<Scene> foreground;
	for (const auto& scene : SceneManager::scenes) {
		if (scene->name == "Foreground") foreground = scene;
	}
	if (!foreground) std::cerr << "Foreground Scene not found" << std::endl;

	setOnClickForButton(tab, "Exit", [] {
		exit(0);
	});

	setOnClickForButton(tab, "Cube", [foreground] {
		foreground->addObject(
			std::make_shared<Cube>(
				"Cube",
				Vector3(0.0f, 0.0f, 0.0f),
				1.0f,
				Colors::WHITE,
				std::shared_ptr<Texture>{}
			)
		);
	});

	setOnClickForButton(tab, "Sphere", [foreground] {
		foreground->addObject(
			std::make_shared<Sphere>(
				"Sphere",
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

	setOnClickForButton(tab, "Sun", [foreground] {
		std::array lightPos = {-2.0f, -3.0f, -6.0f, 0.0f};
		foreground->addLight(
			std::make_shared<Sun>(
				"Sun",
				GL_LIGHT2,
				lightPos
			),
			Colors::LIGHT_SUN,
			Colors::LIGHT_AMBIENT,
			Colors::WHITE
		);
	});
}
