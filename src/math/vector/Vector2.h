#ifndef VECTOR2_C
#define VECTOR2_C

#include <stdexcept>
#include <cmath>

class Vector2 {
public:
	float x, y;

	// Constructor
	Vector2(const float x, const float y) : x(x), y(y) {}

	// Access by index
	float operator[](const int index) const {
		switch (index) {
			case 0: return x;
			case 1: return y;
			default: throw std::out_of_range("Index out of bounds for Vector2");
		}
	}

	// Arithmetic operators
	Vector2 operator+(const Vector2& other) const {
		return {x + other.x, y + other.y};
	}

	Vector2 operator-(const Vector2& other) const {
		return {x - other.x, y - other.y};
	}

	Vector2 operator*(const float scalar) const {
		return {x * scalar, y * scalar};
	}

	Vector2 operator*(const Vector2& other) const {
		return {x * other.x, y * other.y};
	}

	Vector2 operator/(const float scalar) const {
		return {x / scalar, y / scalar};
	}

	Vector2 operator/(const Vector2& other) const {
		return {x / other.x, y / other.y};
	}

	Vector2 operator-() const {
		return {-x, -y};
	}

	// Utility functions
	[[nodiscard]] float length() const {
		return std::sqrt(x * x + y * y);
	}

	[[nodiscard]] Vector2 normalize() const {
		return length() > 0 ? *this / length() : *this; // Return normalized vector
	}

	[[nodiscard]] float distance(const Vector2& other) const {
		return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
	}

	[[nodiscard]] float dot(const Vector2& other) const {
		return x * other.x + y * other.y;
	}
};

#endif // VECTOR2_C
