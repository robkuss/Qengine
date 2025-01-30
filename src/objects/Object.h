#pragma once

using namespace std;

#include <utility>
#include <string>

#include "math/vector/Vector3.h"

class Matrix4;
class Mode;


class Object {
public:
	string name;

	Vector3 position		= Vector3::ZERO;
	Vector3 scale			= Vector3::ONE;
	Vector3 rotation		= Vector3::ZERO;
	Vector3 rotationEuler	= Vector3::ZERO;	// Euler angles for debug output

	// Constructor & Destructor
	explicit Object(string name) : name(move(name)), id(nextID++) {}
	virtual ~Object() = default;	// Virtual destructor to enable dynamic_cast

	virtual void applyTransformation(const Mode &selectionMode, const Mode &transformMode, const Matrix4 &transformation);

	bool operator==(const Object& other) const { return id == other.id; }	// Object == Object

private:
	int id;
	static int nextID;	// Static variable for automatic ID assignment
};

// Initialize Object ID
inline int Object::nextID = 0;

inline void Object::applyTransformation(const Mode &selectionMode, const Mode &transformMode, const Matrix4 &transformation) {}
