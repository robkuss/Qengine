#pragma once

#include <utility>
#include <string>

#include <math/vector/Vector3.h>
#include <math/matrix/Matrix4.h>

class Mode;

class Object {
public:
	std::string name;

	Vector3 position = Vector3::ZERO;
	Vector3 scale	 = Vector3::ONE;
	Vector3 rotation = Vector3::ZERO;
	Vector3 rotationEuler = Vector3::ZERO;	// Euler Angles for debug output

	// Constructor & Destructor
	explicit Object(std::string name) : name(std::move(name)), id(nextID++) {}
	virtual ~Object() = default;	// Virtual destructor to enable dynamic_cast

	virtual void applyTransformation(const Mode &selectionMode, const Mode &transformMode, const Matrix4 &transformation);

	bool operator==(const Object& other) const { return id == other.id; }	// Object == Object

private:
	int id;
	static int nextID;	// Static variable for automatic ID assignment
};

// Initialize Object ID
inline int Object::nextID = 0;

inline void Object::applyTransformation(const Mode &selectionMode, const Mode &transformMode, const Matrix4 &transformation) {
	// Update Object transformation TODO
	/*switch (transformMode.mode) {
		case Mode::GRAB:   position = vector3(transformation * vector4(position, 1.0f)); break;
		case Mode::SCALE:  scale	= vector3(transformation * vector4(scale, 1.0f));	 break;
		case Mode::ROTATE: rotation = vector3(transformation * vector4(rotation, 0.0f)); break;
		default: throw std::invalid_argument("Invalid transformation: Wrong Mode");
	}*/
}
