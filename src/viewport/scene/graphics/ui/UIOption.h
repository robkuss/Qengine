#pragma once

#include "UIElement.h"

#include <string>
#include <variant>
#include <vector>
#include <memory>

// Forward declaration
class UIOptionList;

// Define the Option type
using UIOption = std::variant<std::string, std::shared_ptr<UIOptionList>>;

// Define the OptionList class
class UIOptionList : UIElement {
public:
	std::vector<UIOption> options;

	// Default constructor
	UIOptionList() = default;

	// Variadic constructor to initialize with multiple options
	template <typename... Args>
	explicit UIOptionList(Args... args) {
		addOptions(args...);
	}

	// Add an Option to the list
	void addOption(const std::string& option) {
		options.emplace_back(option);
	}

	// Add a nested OptionList
	void addOption(const std::shared_ptr<UIOptionList>& optionList) {
		options.emplace_back(optionList);
	}

	// Helper to add multiple options recursively (variadic template unpacking)
	template <typename T, typename... Args>
	void addOptions(T first, Args... rest) {
		addOption(first);
		addOptions(rest...);
	}

	// Print all options recursively
	/*void print(const int depth = 0) const {
		const std::string indent(depth * 2, ' '); // Indentation for nesting
		for (const auto& option : options) {
			std::visit([&]<typename T0>(const T0& value) {
				using T = std::decay_t<T0>;
				if constexpr (std::is_same_v<T, std::string>) {
					std::cout << indent << "- " << value << "\n";
				} else if constexpr (std::is_same_v<T, std::shared_ptr<UIOptionList>>) {
					std::cout << indent << "- Nested OptionList:\n";
					value->print(depth + 1);
				}
			}, option);
		}
	}*/
};