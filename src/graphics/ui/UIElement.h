#pragma once

#include <string>
#include <variant>
#include <vector>

#include "math/vector/Vector2.h"


#define PC_100 (Dim(1.0f, DimType::Percent))		// 100%
#define PC_50  (Dim(0.5f, DimType::Percent))		// 50%
#define PC_25  (Dim(0.25f, DimType::Percent))		// 25%
#define UNIT   (Dim(unit, DimType::Pixels))			// Standard size unit
#define NODIM  (Dim(0.0f, DimType::Pixels))			// Default (0)


constexpr float unit			= 40.0f;			// UI base unit (most other elements should depend on this)

constexpr float firstTabX		= unit;
constexpr float tabWidth		= 3.5f * unit;
constexpr float optionWidth		= 5 * unit;

constexpr float tabPadding		= 1.5f;				// Upper and lower padding within the bar
constexpr float tabAugment		= 2 * tabPadding;	// For crossing the gap between Tab and OptionList

constexpr int uiFontSize		= unit / 2;


class UIOption;
class UIOptionList;

using UIOptionVariant = std::variant<UIOption, UIOptionList>;


enum class DimType {
	Pixels,
	Percent
};

typedef struct {
	mutable float value;
	DimType type;
} Dim;


class UIElement {
public:
	std::string label;	// Name of (or text on) the element
	const float x;			// x pos
	const float y;			// y pos
	const Dim sx;			// Width
	const Dim sy;			// Height

	int vertexCount;
	std::vector<Vector2> vertices{};

	UIElement(
		std::string label,
		const float x,
		const float y,
		const Dim sx,
		const Dim sy,
		const int vertexCount = 4
	) :   label(std::move(label)),
		  x(x), y(y), sx(sx), sy(sy),
		  vertexCount(vertexCount) {

		// Ensure vertices has the required number of elements
		vertices.resize(vertexCount);
	}

	virtual ~UIElement() = default;

	virtual void render() = 0;
	virtual void setVertices() = 0;
};
