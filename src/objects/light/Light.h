#pragma once

#include <array>
#include <objects/Object.h>

class Light : public Object {
public:
	explicit Light(
		const std::string& name,
		const int macro,
		const std::array<float, 4> pos
	) : Object(name),
	    macro(macro),
	    pos(pos) {
	}

	const int macro;
	const std::array<float, 4> pos;
};
