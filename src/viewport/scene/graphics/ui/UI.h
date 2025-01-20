#pragma once

#include "UIElement.h"

#include "viewport/scene/Scene.h"
#include "text/Text.h"

#include <vector>
#include <memory>

class Vector2;


class UI : public Scene {
public:
	static float firstLineX;
	static float firstLineY;
	static float bottomLineY;

	static int boundTop, boundBottom, boundLeft, boundRight;

	explicit UI(RenderContext *context);
	~UI();

	void setup();
	void update() const;
	void render() const;

	void addElement(const std::shared_ptr<UIElement>& element, int layer);

private:
	// Elements are stored as pointers, sorted by layer (lowest layer first)
	std::vector<std::vector<std::shared_ptr<UIElement>>> elements;
	std::vector<const Vector2*> vertexPointers;

	int highestRank = 0;
};
