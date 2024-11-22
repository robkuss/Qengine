#ifndef VECTOR4_H
#define VECTOR4_H

#include <stdexcept>
#include <cmath>

class Vector4 {
public:
	float x, y, z, w;

	// Constructor
	Vector4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}

	// Access by index
	float operator[](const int index) const {
		switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
			default: throw std::out_of_range("Index out of bounds for Vector4");
		}
	}

	// Arithmetic operators
	Vector4 operator+(const Vector4& other) const {
		return {x + other.x, y + other.y, z + other.z, w + other.w};
	}

	Vector4 operator-(const Vector4& other) const {
		return {x - other.x, y - other.y, z - other.z, w - other.w};
	}

	Vector4 operator*(const float scalar) const {
		return {x * scalar, y * scalar, z * scalar, w * scalar};
	}

	Vector4 operator*(const Vector4& other) const {
		return {x * other.x, y * other.y, z * other.z, w * other.w};
	}

	Vector4 operator/(const float scalar) const {
		return {x / scalar, y / scalar, z / scalar, w / scalar};
	}

	Vector4 operator/(const Vector4& other) const {
		return {x / other.x, y / other.y, z / other.z, w / other.w};
	}

	Vector4 operator-() const {
		return {-x, -y, -z, -w};
	}

	// Utility functions
	[[nodiscard]] float length() const {
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	[[nodiscard]] Vector4 normalize() const {
		return length() > 0 ? *this / length() : *this; // Return normalized vector
	}

	[[nodiscard]] float distance(const Vector4& other) const {
		return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z) + (w - other.w) * (w - other.w));
	}

	[[nodiscard]] float dot(const Vector4& other) const {
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}
};

#endif // VECTOR4_H
