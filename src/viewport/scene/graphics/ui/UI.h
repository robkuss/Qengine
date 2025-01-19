#pragma once

#include "UIElement.h"

#include <array>
#include <vector>
#include <memory>

// Options
#define TEXT	// For on-screen text
#ifdef TEXT
	#include "text/Text.h"
#endif

class Viewport;
class Vector2;

enum class DimType {
	Pixels,
	Percent
};

typedef struct {
	float value;
	DimType type;
} Dim;

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

	void addElement(const std::shared_ptr<UIElement>& element) {
		for (const auto& vertex : element->vertices) {
			vertexPointers.emplace_back(&vertex);
		}
		elements.push_back(element);
	}

private:
	friend class Debug;

	const Viewport *viewport;

	std::vector<std::shared_ptr<UIElement>> elements;
	std::vector<const Vector2*> vertexPointers;

	// Text
	#ifdef TEXT
		Text* text{};	// For on-screen debug text
	#endif
};
