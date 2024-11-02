#ifndef OBJECT_H
#define OBJECT_H

#include <utility>
#include <string>

#include "../math/vector/Vector3.h"


class Object {
public:
	std::string name;
    Vector3 position, scale, rotation;

	// Constructor & Destructor
	Object(std::string name, const Vector3& position, const Vector3& scale, const Vector3& rotation)
		: name(std::move(name)), position(position), scale(scale), rotation(rotation), id(nextID++) {}
	virtual ~Object() = default;	// Virtual destructor to enable dynamic_cast

	bool operator==(const Object& other) const { return id == other.id; }	// Object == Object

private:
	int id;
	static int nextID;	// Static variable for automatic ID assignment
};

// Initialize Object ID
inline int Object::nextID = 0;

#endif // OBJECT_H
