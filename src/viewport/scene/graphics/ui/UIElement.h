#pragma once

#include <vector>

#include "math/vector/Vector2.h"

#define PC_100 (Dim(1.0f, DimType::Percent))		// 100%
#define PC_50  (Dim(0.5f, DimType::Percent))		// 50%
#define PC_25  (Dim(0.25f, DimType::Percent))		// 25%
#define UNIT   (Dim(unit, DimType::Pixels))			// Standard size unit
#define NODIM  (Dim(0.0f, DimType::Pixels))			// Default (0)

constexpr float unit		= 40.0f;	// UI base unit (most other elements should depend on this)

constexpr float firstTabX	= unit;
constexpr float tabPadding	= 1.5f;		// Upper and lower padding within the bar
constexpr float buttonWidth	= 3 * unit;

constexpr int tabFontSize	= unit / 2;


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
		const float x = 0,
		const float y = 0,
		const Dim sx = NODIM,
		const Dim sy = NODIM,
		const int vertexCount = 4
	) :   x(x), y(y), sx(sx), sy(sy),
	      vertexCount(vertexCount) {
		vertices.resize(vertexCount);	// Ensure vertices has the required number of elements
	}

	virtual ~UIElement() = default;

	virtual void render() const = 0;
	virtual void setVertices() = 0;
};
