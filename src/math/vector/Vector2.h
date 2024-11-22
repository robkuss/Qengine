#ifndef VECTOR2_C
#define VECTOR2_C

#include <stdexcept>
#include <cmath>

class Vector2 {
public:
	double x, y;

	// Constructor
	Vector2(const double x, const double y) : x(x), y(y) {}

	// Access by index
	double operator[](const int index) const {
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

	Vector2 operator*(const double scalar) const {
		return {x * scalar, y * scalar};
	}

	Vector2 operator*(const Vector2& other) const {
		return {x * other.x, y * other.y};
	}

	Vector2 operator/(const double scalar) const {
		return {x / scalar, y / scalar};
	}

	Vector2 operator/(const Vector2& other) const {
		return {x / other.x, y / other.y};
	}

	Vector2 operator-() const {
		return {-x, -y};
	}

	// Utility functions
	[[nodiscard]] double length() const {
		return std::sqrt(x * x + y * y);
	}

	[[nodiscard]] Vector2 normalize() const {
		return length() > 0 ? *this / length() : *this; // Return normalized vector
	}

	[[nodiscard]] double distance(const Vector2& other) const {
		return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
	}

	[[nodiscard]] double dot(const Vector2& other) const {
		return x * other.x + y * other.y;
	}
};

#endif // VECTOR2_C
