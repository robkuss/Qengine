#ifndef MATRIX4_H
#define MATRIX4_H

#include <cmath>

#include "math/Util.h"
#include "math/vector/Vector4.h"

#include <array>


class Matrix4 {
public:
    explicit Matrix4(const std::array<float, 16>& m);
    ~Matrix4() = default;

    float operator[](int) const;                    // Access Matrix elements
    Vector4 operator*(const Vector4&) const;        // Matrix * Vector
    Matrix4 operator*(const Matrix4&) const;        // Matrix * Matrix
    [[nodiscard]] Matrix4 invert() const;           // Matrix inversion

    static Matrix4 translate(const Vector3& t);
    static Matrix4 scale(const Vector3& s);
    static Matrix4 rotateX(float angle);
    static Matrix4 rotateY(float angle);
    static Matrix4 rotateZ(float angle);
    static Matrix4 createTransformationMatrix(const Vector3& t, const Vector3& r, const Vector3& s);

private:
    std::array<float, 16> m{};
};


inline Matrix4::Matrix4(const std::array<float, 16>& m) : m(m) {}

inline float Matrix4::operator[](const int index) const {
    if (index < 0 || index >= 16) {
        throw std::out_of_range("Index out of bounds for Matrix4x4");
    }
    return m[index];
}

inline Vector4 Matrix4::operator*(const Vector4& vec) const {
    return {
        m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12] * vec.w,
        m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13] * vec.w,
        m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14] * vec.w,
        m[3] * vec.x + m[7] * vec.y + m[11] * vec.z + m[15] * vec.w
    };
}

inline Matrix4 Matrix4::operator*(const Matrix4& other) const {
    std::array<float, 16> result{};

    // Matrix multiplication (dot products)
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += m[i * 4 + k] * other[k * 4 + j];
            }
            result[i * 4 + j] = sum;
        }
    }

    return Matrix4(result);
}

inline Matrix4 Matrix4::translate(const Vector3& t) {
    return Matrix4({
        1, 0, 0, t.x,
        0, 1, 0, t.y,
        0, 0, 1, t.z,
        0, 0, 0, 1
    });
}

inline Matrix4 Matrix4::scale(const Vector3& s) {
    return Matrix4({
        s.x, 0, 0, 0,
        0, s.y, 0, 0,
        0, 0, s.z, 0,
        0, 0, 0, 1
    });
}

inline Matrix4 Matrix4::rotateX(const float angle) {
    const float c = cos(angle);
    const float s = sin(angle);
    return Matrix4({
        1, 0, 0, 0,
        0, c, -s, 0,
        0, s, c, 0,
        0, 0, 0, 1
    });
}

inline Matrix4 Matrix4::rotateY(const float angle) {
    const float c = cos(angle);
    const float s = sin(angle);
    return Matrix4({
        c, 0, s, 0,
        0, 1, 0, 0,
        -s, 0, c, 0,
        0, 0, 0, 1
    });
}

inline Matrix4 Matrix4::rotateZ(const float angle) {
    const float c = cos(angle);
    const float s = sin(angle);
    return Matrix4({
        c, -s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    });
}

inline Matrix4 Matrix4::createTransformationMatrix(const Vector3& t, const Vector3& r, const Vector3& s) {
    // Apply transformations in reverse order: scale -> rotate -> translate
    const Matrix4 scaleMatrix = scale(s);
    const Matrix4 rotateXMatrix = rotateX(r.x);
    const Matrix4 rotateYMatrix = rotateY(r.y);
    const Matrix4 rotateZMatrix = rotateZ(r.z);
    const Matrix4 translationMatrix = translate(t);

    // Combine them into a single transformation matrix
    const Matrix4 modelMatrix = translationMatrix * (rotateZMatrix * (rotateYMatrix * (rotateXMatrix * scaleMatrix)));
    return modelMatrix;
}

inline Matrix4 Matrix4::invert() const {
    std::array<float, 16> inv{};

    // Calculate the cofactor matrix
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

    return Matrix4(inv);
}

#endif // MATRIX4_H
