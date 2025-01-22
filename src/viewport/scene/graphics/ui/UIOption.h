#pragma once

#include "UIElement.h"

#include <string>
#include <variant>
#include <vector>


using UIOptionList = std::map<
	std::string,
	std::vector<std::string>
>;

using UIOption = std::variant<
	std::vector<std::string>,
	UIOptionList
>;

// Define the OptionList class
/*
class UIOptionList final : public UIElement {
public:
	// Variadic constructor to initialize with multiple options
	template <typename... Args>
	explicit UIOptionList(
		Args... args
	) {
		addOptions(args...);	// Add options using variadic template
	}

	// Helper to add multiple options recursively (variadic template unpacking)
	template <typename T, typename... Args>
	void addOptions(T first, Args... rest) {
		addOption(first);
		addOptions(rest...);
	}

	// Base case for recursion (no arguments)
	static void addOptions() {}

	// Add an Option to the list
	void addOption(const std::string& option) {
		options.emplace_back(option);
	}

	// Add a nested OptionList
	void addOption(const std::shared_ptr<UIOptionList>& optionList) {
		options.emplace_back(optionList);
	}

	void render() const override;
	void setVertices() override;

private:
	std::map<std::string, UIOption> options;
};


inline void UIOptionList::render() const {

}

inline void UIOptionList::setVertices() {

}
*/
