#pragma once

#include <iostream>
#include <ostream>
#include <utility>
#include <string>
#include <cmath>

#include <math/vector/Vector3.h>
#include <math/matrix/Matrix4.h>

class Object {
public:
	std::string name;
	Matrix4 position = Matrix4::translate(Vector3::ZERO);
	Matrix4 scale    = Matrix4::scale(Vector3::ONE);
	Matrix4 rotation = Matrix4::rotateX(0.0f) * Matrix4::rotateY(0.0f) * Matrix4::rotateZ(0.0f);

	// Constructor & Destructor
	explicit Object(std::string name) : name(std::move(name)), id(nextID++) {}
	virtual ~Object() = default;	// Virtual destructor to enable dynamic_cast

	virtual void applyTransformation(const Mode& mode, const Matrix4& transformation);

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

inline void Object::applyTransformation(const Mode& mode, const Matrix4& transformation) {
	// Update Object transformation
	switch (mode.mode) {
		case Mode::GRAB:   position = transformation * position; break;
		case Mode::SCALE:  scale	= transformation * scale;	 break;
		case Mode::ROTATE: rotation = transformation * rotation; break;
		default: throw std::invalid_argument("Invalid transformation: Wrong Mode");
	}
}
