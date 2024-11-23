#pragma once

#include <iostream>
#include <ostream>
#include <utility>
#include <string>
#include <cmath>

#include "math/vector/Vector3.h"
#include "math/matrix/Matrix4.h"


class Object {
public:
	std::string name;
	Matrix4 position, scale, rotation;

	// Constructor & Destructor
	Object(std::string name, const Matrix4& position, const Matrix4& scale, const Matrix4& rotation)
		: name(std::move(name)), position(position), scale(scale), rotation(rotation), id(nextID++) {}
	virtual ~Object() = default;	// Virtual destructor to enable dynamic_cast

	bool operator==(const Object& other) const { return id == other.id; }	// Object == Object

	[[nodiscard]] Vector3 getPosition() const { return {position.m41, position.m42, position.m43}; }
	[[nodiscard]] Vector3 getScale()	const { return {scale.m11, scale.m22, scale.m33}; }
	[[nodiscard]] Vector3 getRotation() const {
		if (rotation.m13 == 1) std::cerr << "Gimbal Lock" << std::endl;
		return {
			static_cast<float>(degrees(atan2(-scale.m21, scale.m22))),
			static_cast<float>(degrees(asin(rotation.m13))),
			static_cast<float>(degrees(atan2(-rotation.m12, rotation.m11)))
		};
	}

private:
	int id;
	static int nextID;	// Static variable for automatic ID assignment
};

// Initialize Object ID
inline int Object::nextID = 0;
