#pragma once

#include <array>
#include <objects/Object.h>

class Light : public Object {
public:
	explicit Light(
		const string& name,
		const int macro,
		const array<float, 4> pos
	) : Object(name),
	    macro(macro),
	    pos(pos) {
	}

	const int macro;
	const array<float, 4> pos;

	float ambient[3]  = {0.0f, 0.0f, 0.0f};
	float diffuse[3]  = {0.0f, 0.0f, 0.0f};
	float specular[3] = {0.0f, 0.0f, 0.0f};
};
