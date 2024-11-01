#ifndef MATRIX4X4_C
#define MATRIX4X4_C

#include <array>
#include <stdexcept>

#include "../Util.h"
#include "../vector/Vector4.cpp"


class Matrix4x4 {
public:
	// Constructor that initializes the matrix
	explicit Matrix4x4(const float *m);
	~Matrix4x4() = default;

	// [[nodiscard]] const std::array<float, 16>& get() const;	// Get the underlying matrix
	float operator[](int) const;					// Access matrix elements
	Vector4 operator*(const Vector4&) const;		// Matrix * vector
	Matrix4x4 operator*(const Matrix4x4&) const;	// Matrix * matrix

	[[nodiscard]] Matrix4x4 invert() const;			// Invert a 4x4 Matrix

private:
	const float *m;
};


Matrix4x4::Matrix4x4(const float *m) : m(m) {}

float Matrix4x4::operator[](const int index) const {
	if (index < 0 || index >= 16) {
		throw std::out_of_range("Index out of bounds for Matrix4x4");
	}
	return m[index];
}

Vector4 Matrix4x4::operator*(const Vector4& vec) const {
	return {
		m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12] * vec.w,
		m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13] * vec.w,
		m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14] * vec.w,
		m[3] * vec.x + m[7] * vec.y + m[11] * vec.z + m[15] * vec.w
	};
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
	float result[16];
	for (float& i : result) i = 0.0f;

	// Loop through rows of the first matrix (this)
	for (int i = 0; i < 4; ++i) {
		// Loop through columns of the second matrix (other)
		for (int j = 0; j < 4; ++j) {
			// Calculate the dot product for element (i, j)
			float sum = 0.0f;
			for (int k = 0; k < 4; ++k) {
				sum += this->m[i * 4 + k] * other[k * 4 + j];
			}
			result[i * 4 + j] = sum;
		}
	}

	return Matrix4x4(result);
}

Matrix4x4 Matrix4x4::invert() const {
	// The inverted matrix we'll return
	std::array<float, 16> inv{};

	// The following calculations find the inverse using cofactor expansion and determinant division
	inv[0]  =  m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
	inv[4]  = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
	inv[8]  =  m[4] * m[9]  * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
	inv[12] = -m[4] * m[9]  * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

	inv[1]  = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
	inv[5]  =  m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
	inv[9]  = -m[0] * m[9]  * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
	inv[13] =  m[0] * m[9]  * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

	inv[2]  =  m[1] * m[6]  * m[15] - m[1] * m[7]  * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7]  - m[13] * m[3] * m[6];
	inv[6]  = -m[0] * m[6]  * m[15] + m[0] * m[7]  * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7]  + m[12] * m[3] * m[6];
	inv[10] =  m[0] * m[5]  * m[15] - m[0] * m[7]  * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7]  - m[12] * m[3] * m[5];
	inv[14] = -m[0] * m[5]  * m[14] + m[0] * m[6]  * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6]  + m[12] * m[2] * m[5];

	inv[3]  = -m[1] * m[6]  * m[11] + m[1] * m[7]  * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9]  * m[2] * m[7]  + m[9]  * m[3] * m[6];
	inv[7]  =  m[0] * m[6]  * m[11] - m[0] * m[7]  * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8]  * m[2] * m[7]  - m[8]  * m[3] * m[6];
	inv[11] = -m[0] * m[5]  * m[11] + m[0] * m[7]  * m[9]  + m[4] * m[1] * m[11] - m[4] * m[3] * m[9]  - m[8]  * m[1] * m[7]  + m[8]  * m[3] * m[5];
	inv[15] =  m[0] * m[5]  * m[10] - m[0] * m[6]  * m[9]  - m[4] * m[1] * m[10] + m[4] * m[2] * m[9]  + m[8]  * m[1] * m[6]  - m[8]  * m[2] * m[5];

	// Calculate the determinant
	float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det > -EPSILON && det < EPSILON) throw std::runtime_error("Matrix is singular and cannot be inverted.");

	det = 1.0f / det;

	// Scale the inverse matrix by the determinant
	for (int i = 0; i < 16; i++) {
		inv[i] *= det;
	}

	// Return the inverted matrix
	return Matrix4x4(inv.data());
}

#endif // MATRIX4X4_C
