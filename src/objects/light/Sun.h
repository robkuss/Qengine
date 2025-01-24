#pragma once

#include "Light.h"

class Sun : public Light {
public:
	explicit Sun(
		const int macro,
		const std::array<float, 4> pos
	) :   Light(macro, pos) {}
};