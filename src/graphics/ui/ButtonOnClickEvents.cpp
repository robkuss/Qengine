#include <algorithm>
#include <iostream>

#include "UI.h"

#include "math/Util.h"

#include "objects/mesh/cube/Cube.cpp"
#include "objects/mesh/sphere/Sphere.cpp"
#include <objects/light/Sun.h>
#include <viewport/scene/SceneManager.h>

#include "UIOption.h"


/**
 * Searches for a button with a given label in a given list of UIOptions
 * and set its onClick function to a given function parameter
 *
 * @param list The list in which to recursively look for the UIOption button
 * @param label The label of the button to look for
 * @param onClickAction The onClick function to be passed to the button
 * @return True if successful, false otherwise
 */
bool UI::setOnClickForOptionButton(	// NOLINT(*-no-recursion)
	const shared_ptr<UIOptionList>& list,
	const string& label,
	const function<void()>& onClickAction
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
			  holds_alternative<UIOption>(*innerOption)
			? get<UIOption>(*innerOption).button
			: get<UIOptionList>(*innerOption).button;

		if (button && button->label == label) {
			button->onClick = onClickAction;
			return true;
		}
	}

	// Recursively check all nested UIOptionLists
	if (ranges::any_of(list->options, [&](const auto& nestedList) {
		if (holds_alternative<UIOptionList>(*nestedList)) {
			const auto nestedOptionList = make_shared<UIOptionList>(get<UIOptionList>(*nestedList));
			return setOnClickForOptionButton(nestedOptionList, label, onClickAction);
		}
		return false;
	})) return true;

	return false;	// Return false if no matching button was found
}


void UI::setOptionButtonOnClickEvents(const shared_ptr<UIOptionList>& tab) {
	shared_ptr<Scene> foreground;
	for (const auto& scene : SceneManager::scenes) {
		if (scene->name == "Foreground") foreground = scene;
	}
	if (!foreground) cerr << "Foreground Scene not found" << endl;


	setOnClickForOptionButton(tab, "Exit", [] {
		exit(0);
	});

	setOnClickForOptionButton(tab, "Cube", [foreground] {
		foreground->addObject(
			make_shared<Cube>(
				"Cube",
				Vector3(0.0f, 0.0f, 0.0f),
				1.0f,
				Colors::WHITE,
				shared_ptr<Texture>{}
			)
		);
	});

	setOnClickForOptionButton(tab, "Sphere", [foreground] {
		foreground->addObject(
			make_shared<Sphere>(
				"Sphere",
				Vector3(0.0f, 0.0f, 0.0f),
				0.5f,
				64,
				32,
				Colors::WHITE,
				shared_ptr<Texture>{}
			)
		);
	});

	setOnClickForOptionButton(tab, "Point", [] {
		// TODO make Point lights
	});

	setOnClickForOptionButton(tab, "Sun", [foreground] {
		array lightPos = {-2.0f, -3.0f, -6.0f, 0.0f};
		foreground->addLight(
			make_shared<Sun>(
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
