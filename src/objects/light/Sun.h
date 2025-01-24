#pragma once

#include "Light.h"

class Sun : public Light {
public:
	explicit Sun(
		const std::string& name,
		const int macro,
		const std::array<float, 4> pos
	) :   Light(name, macro, pos) {}
};