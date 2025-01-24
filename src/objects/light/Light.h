#pragma once

#include <array>
#include <utility>

class Light {
public:
	explicit Light(
		std::string name,
		const int macro,
		const std::array<float, 4> pos
	) :   name(std::move(name)),
		  macro(macro),
	      pos(pos) {
	}

	const std::string name;
	const int macro;
	const std::array<float, 4> pos;
};
