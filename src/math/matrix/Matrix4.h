#pragma once

#include <array>

class Vector4;

class Matrix4 {
public:
	Matrix4() = default;								// Default Constructor
	explicit Matrix4(const float* arr);					// Constructor with array
	explicit Matrix4(const std::array<float, 16>& arr);	// Constructor with std::array
	Matrix4(std::initializer_list<float> list);			// Constructor with initializer list

	void toFloatArray(float out[16]) const;

	Matrix4 operator-(const Matrix4& other) const;
    Vector4 operator*(const Vector4 &v) const;
    Matrix4 operator*(const Matrix4 &other) const;

    static Matrix4 translate(const Vector3& t);
	static Matrix4 scale(const Vector3& s);
	static Matrix4 rotateX(float a);
	static Matrix4 rotateY(float a);
	static Matrix4 rotateZ(float a);

	[[nodiscard]] Matrix4 invert() const;

	[[nodiscard]] Vector3 extractEulerAngles() const;

	void toColumnMajor(float (&values)[16]) const;

private:
	float m11{}, m21{}, m31{}, m41{};
	float m12{}, m22{}, m32{}, m42{};
	float m13{}, m23{}, m33{}, m43{};
	float m14{}, m24{}, m34{}, m44{};
};
