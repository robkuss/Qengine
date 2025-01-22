#pragma once

#include <vector>

#include "math/vector/Vector2.h"

class UI;

enum class DimType {
	Pixels,
	Percent
};

typedef struct {
	float value;
	DimType type;
} Dim;


class UIElement {
public:
	float x;		// x pos
	float y;		// y pos
	Dim sx;			// Width
	Dim sy;			// Height

	int vertexCount;
	std::vector<Vector2> vertices{};

	explicit UIElement(
		const float x = 0.0f,
		const float y = 0.0f,
		const Dim sx = Dim(),
		const Dim sy = Dim(),
		const int vertexCount = 4
	) :   x(x), y(y), sx(sx), sy(sy),
	      vertexCount(vertexCount) {
		vertices.resize(vertexCount);	// Ensure vertices has the required number of elements
	}

	virtual ~UIElement() = default;

	virtual void render() const = 0;
	virtual void setVertices() = 0;
};
