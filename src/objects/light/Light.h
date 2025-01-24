#pragma once

#include <array>

class Light {
public:
	explicit Light(
		const int macro,
		const std::array<float, 4> pos
	) :   macro(macro),
		  pos(pos) {}

	const int macro;
	const std::array<float, 4> pos;
};