#pragma once

#include "UI.h"
#include "UITab.h"


class UIBar final : public UIElement {
public:
	// Constructor with all properties and variadic list of OptionLists
	template <typename... Args>
	explicit UIBar(
		Args... args
	) {
		addTabs(args...);   // Add variadic tabs
	}

	// Helper to add multiple tabs recursively (variadic template unpacking)
	template <typename T, typename... Args>
	void addTabs(T first, Args... rest) {
		addTab(first);
		addTabs(rest...);
	}

	// Base case for recursion (no arguments)
	static void addTabs() {}

	// Add a single tab (root OptionList)
	void addTab(const std::shared_ptr<UITab>& tab) {
		tabs.emplace_back(tab);
	}

	void render() const override;
	void setVertices(float windowW, float windowH) override;

private:
	std::vector<std::shared_ptr<UITab>> tabs;
};
