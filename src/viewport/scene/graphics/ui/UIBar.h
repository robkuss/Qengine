#pragma once

#include "UI.h"
#include "UITab.h"

#include <math/vector/Vector2.h>


class UIBar final : public UIElement {
public:
	// Constructor with all properties and variadic list of OptionLists
	template <typename... Args>
	UIBar(const Vector2 startPos, const Dim length, const Dim width, Args... args)
			: startPos(startPos), length(length), width(width) {
		for (int i = 0; i < vertexCount; i++) {
			vertices.emplace_back();
		}
		setVertices(startPos, length, width);
		addTabs(args...);
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

	void setVertices(Vector2 startPos, Dim length, Dim width);

	void update() override;
	void render() const override;

private:
	Vector2 startPos;                   			// Starting position
	Dim length;										// Length of the bar
	Dim width;										// Width of the bar
	int vertexCount = 4;

	std::vector<std::shared_ptr<UITab>> tabs;		// Tabs are roots of OptionLists
};
