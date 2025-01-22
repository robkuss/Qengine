#pragma once

#include "UIElement.h"

#include "viewport/scene/Scene.h"
#include "text/Text.h"

#include <vector>
#include <memory>

class Vector2;


class UI final : public Scene {
public:
	// Constructor & Destructor
	UI(const int *width, const int *height);
	~UI() override;

	void setup();
	void update() const;
	void render() const override;

	void addElement(const std::shared_ptr<UIElement>& element, int layer);

protected:
	friend class Text;
	friend class Debug;

	static float firstLineX;
	static float firstLineY;
	static float bottomLineY;

	static int boundTop, boundBottom, boundLeft, boundRight;

private:
	const int *width;
	const int *height;

	// Elements are stored as pointers, sorted by layer (lowest layer first)
	std::vector<std::vector<std::shared_ptr<UIElement>>> elements;
	std::vector<const Vector2*> vertexPointers;

	int highestRank = 0;
};
