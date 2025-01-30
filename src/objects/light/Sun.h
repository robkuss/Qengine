#pragma once

#include "Light.h"

class Sun final : public Light {
public:
	explicit Sun(
		const string& name,
		const int macro,
		const array<float, 4> pos
	) :   Light(name, macro, pos) {}
};