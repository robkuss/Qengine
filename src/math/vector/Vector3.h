#pragma once

#include <string>
#include <stdexcept>
#include <cmath>
#include <iomanip>
#include <sstream>


class Vector3 {
public:
	float x, y, z;

	// Constructor
	Vector3() = default;
	Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}

	// Static constants
	static const Vector3& ZERO;
	static const Vector3& ONE;
	static const Vector3& MINUS_ONE;

	// Check equality
	bool operator==(const Vector3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	// Access by index
	float operator[](const int index) const {
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

	Vector3 operator*(const float scalar) const {
		return {x * scalar, y * scalar, z * scalar};
	}

	Vector3 operator*(const Vector3& other) const {
		return {x * other.x, y * other.y, z * other.z};
	}

	Vector3 operator/(const float scalar) const {
		return {x / scalar, y / scalar, z / scalar};
	}

	Vector3 operator/(const Vector3& other) const {
		return {x / other.x, y / other.y, z / other.z};
	}

	Vector3 operator-() const {
		return {-x, -y, -z};
	}

	// Utility functions
	[[nodiscard]] float length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	[[nodiscard]] Vector3 normalize() const {
		return length() > 0 ? *this / length() : *this; // Return normalized vector
	}

	[[nodiscard]] float distance(const Vector3& other) const {
		return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z));
	}

	[[nodiscard]] float dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	[[nodiscard]] Vector3 cross(const Vector3& other) const {
		return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
	}

	[[nodiscard]] std::string toString() const {
		std::ostringstream out;
		out << std::fixed << std::setprecision(3) << x << ", " << y << ", " << z;
		return out.str();
	}
};

inline const Vector3& Vector3::ZERO		 = Vector3(0.0f, 0.0f, 0.0f);
inline const Vector3& Vector3::ONE		 = Vector3(1.0f, 1.0f, 1.0f);
inline const Vector3& Vector3::MINUS_ONE = Vector3(-1.0f, -1.0f, -1.0f);

template <>
struct std::hash<Vector3> {
	size_t operator()(const Vector3& v) const noexcept {
		// Combine the hashes of the three components (x, y, z)
		const size_t h1 = hash<float>()(v.x);
		const size_t h2 = hash<float>()(v.y);
		const size_t h3 = hash<float>()(v.z);

		// Combine the three hash values using XOR and bit shifting
		return h1 ^ h2 << 1 ^ h3 << 2;
	}
};
