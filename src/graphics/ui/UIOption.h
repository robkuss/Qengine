#pragma once

#include "UIButtonElement.h"

#include <string>
#include <variant>


class UIOptionBase : public UIButtonElement {
public:
	bool isTabBelowMouse;

	explicit UIOptionBase(
		const std::string& label,
		const float x, const float y, const Dim sx, const Dim sy
	) :	  UIButtonElement(label, x, y, sx, sy) {

		isTabBelowMouse = false;
	}

	// Optionally define a virtual destructor to support polymorphism
	~UIOptionBase() override = default;

	void render() override {
		button->render();
	}

	void setVertices() override {}
};


/** A UIOptionList is a list of UIOption variants */
class UIOptionList final : public UIOptionBase {
public:
	std::vector<std::shared_ptr<UIOptionVariant>> options;

	explicit UIOptionList(
		const std::string& label,
		const std::vector<std::shared_ptr<UIOptionVariant>>& options,
		const float x, const float y, const Dim sx, const Dim sy
	) :   UIOptionBase(label, x, y, sx, sy),
		  options(options) {}

	void addOption(const std::shared_ptr<UIOptionVariant>& option) {
		options.emplace_back(option);
	}

	void render() override {
		button->render();

		// Draw an arrow to the right to signify Option List
		glBegin(GL_TRIANGLES);
		color3f(Colors::UI_ARROW_COLOR);
		glVertex2f(x + sx.value - 25.0f, y + sy.value - 10.0f);
		glVertex2f(x + sx.value - 25.0f, y + 10.0f);
		glVertex2f(x + sx.value - 10.0f, y + sy.value / 2.0f);
		glEnd();
	}
};


/** A UIOption represents an element in a UIOptionList */
class UIOption final : public UIOptionBase {
public:
	explicit UIOption(
		const std::string& label,
		const float x, const float y, const Dim sx, const Dim sy
	) :   UIOptionBase(label, x, y, sx, sy) {}
};


inline std::string getLabel(const UIOptionVariant& optionVariant) {
	return std::visit(
		[](const UIOptionBase& option) -> std::string {
			return option.label;
		},
		optionVariant
	);
}
