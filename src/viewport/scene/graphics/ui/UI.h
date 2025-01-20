#pragma once

#include "UIElement.h"

#include <array>
#include <vector>
#include <memory>

#include "text/Text.h"


class Viewport;
class Vector2;

inline const std::array<int, 4>* vp;

class UI {
public:
	static float firstLineX;
	static float firstLineY;
	static float bottomLineY;

	static int boundTop, boundBottom, boundLeft, boundRight;

	explicit UI(const Viewport *viewport);
	~UI();

	void setup();
	void update() const;
	void render() const;

	void addElement(const std::shared_ptr<UIElement>& element, int layer);

private:
	friend class Debug;

	const Viewport *viewport;

	// Elements are stored as pointers, sorted by layer (lowest layer first)
	std::vector<std::vector<std::shared_ptr<UIElement>>> elements;
	std::vector<const Vector2*> vertexPointers;

	int highestRank = 0;
};
