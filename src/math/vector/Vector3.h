#ifndef VECTOR3_H
#define VECTOR3_H

#include <string>
#include <stdexcept>
#include <cmath>

#include "Vector4.h"

class Vector3 {
public:
	double x, y, z;

	// Constructor
	Vector3(const double x, const double y, const double z) : x(x), y(y), z(z) {}

	// Static constants
	static const Vector3& ZERO;
	static const Vector3& ONE;
	static const Vector3& MINUS_ONE;

	// Check equality
	bool operator==(const Vector3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	// Access by index
	double operator[](const int index) const {
		switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default: throw std::out_of_range("Index out of bounds for Vector3");
		}
	}

	// Arithmetic operators
	Vector3 operator+(const Vector3& other) const {
		return {x + other.x, y + other.y, z + other.z};
	}

	Vector3 operator-(const Vector3& other) const {
		return {x - other.x, y - other.y, z - other.z};
	}

	Vector3 operator*(const double scalar) const {
		return {x * scalar, y * scalar, z * scalar};
	}

	Vector3 operator*(const Vector3& other) const {
		return {x * other.x, y * other.y, z * other.z};
	}

	Vector3 operator/(const double scalar) const {
		return {x / scalar, y / scalar, z / scalar};
	}

	Vector3 operator/(const Vector3& other) const {
		return {x / other.x, y / other.y, z / other.z};
	}

	Vector3 operator-() const {
		return {-x, -y, -z};
	}

	// Utility functions
	[[nodiscard]] double length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	[[nodiscard]] Vector3 normalize() const {
		return length() > 0.0 ? *this / length() : *this; // Return normalized vector
	}

	[[nodiscard]] double distance(const Vector3& other) const {
		return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z));
	}

	[[nodiscard]] double dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	[[nodiscard]] Vector3 cross(const Vector3& other) const {
		return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
	}

	[[nodiscard]] Vector4 toVector4() const {
		return {x, y, z, 1.0f};
	}

	[[nodiscard]] std::string toString() const {
		return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
	}
};

inline const Vector3& Vector3::ZERO		 = Vector3(0.0, 0.0, 0.0);
inline const Vector3& Vector3::ONE		 = Vector3(1.0, 1.0, 1.0);
inline const Vector3& Vector3::MINUS_ONE = Vector3(-1.0, -1.0, -1.0);

#endif // VECTOR3_H
