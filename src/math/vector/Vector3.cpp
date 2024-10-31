#ifndef VECTOR3_C
#define VECTOR3_C

#include <stdexcept>
#include <cmath>

class Vector3 {
public:
	float x, y, z;

	// Constructor
	Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}

	// Static constants
	static const Vector3 ZERO;
	static const Vector3 ONE;

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
};

inline const Vector3 Vector3::ZERO = Vector3(0, 0, 0);
inline const Vector3 Vector3::ONE  = Vector3(1, 1, 1);

#endif // VECTOR3_C
