#ifndef MATRIX4_H
#define MATRIX4_H

#include <cmath> // For sin, cos, etc.

#include "../Util.h"
#include "../vector/Vector4.h"


struct Matrix4 {
    // Matrix elements
    double m11, m21, m31, m41;
    double m12, m22, m32, m42;
    double m13, m23, m33, m43;
    double m14, m24, m34, m44;

    // Constructors
    Matrix4()
        : m11(1), m21(0), m31(0), m41(0),
          m12(0), m22(1), m32(0), m42(0),
          m13(0), m23(0), m33(1), m43(0),
          m14(0), m24(0), m34(0), m44(1) {}

    Matrix4(
        const double m11, const double m21, const double m31, const double m41,
        const double m12, const double m22, const double m32, const double m42,
        const double m13, const double m23, const double m33, const double m43,
        const double m14, const double m24, const double m34, const double m44)
        : m11(m11), m21(m21), m31(m31), m41(m41),
          m12(m12), m22(m22), m32(m32), m42(m42),
          m13(m13), m23(m23), m33(m33), m43(m43),
          m14(m14), m24(m24), m34(m34), m44(m44) {}

    // Matrix multiplication
    Matrix4 operator*(const Matrix4& other) const {
        return {
            // First row
            m11 * other.m11 + m12 * other.m21 + m13 * other.m31 + m14 * other.m41,
            m21 * other.m11 + m22 * other.m21 + m23 * other.m31 + m24 * other.m41,
            m31 * other.m11 + m32 * other.m21 + m33 * other.m31 + m34 * other.m41,
            m41 * other.m11 + m42 * other.m21 + m43 * other.m31 + m44 * other.m41,
            // Second row
            m11 * other.m12 + m12 * other.m22 + m13 * other.m32 + m14 * other.m42,
            m21 * other.m12 + m22 * other.m22 + m23 * other.m32 + m24 * other.m42,
            m31 * other.m12 + m32 * other.m22 + m33 * other.m32 + m34 * other.m42,
            m41 * other.m12 + m42 * other.m22 + m43 * other.m32 + m44 * other.m42,
            // Third row
            m11 * other.m13 + m12 * other.m23 + m13 * other.m33 + m14 * other.m43,
            m21 * other.m13 + m22 * other.m23 + m23 * other.m33 + m24 * other.m43,
            m31 * other.m13 + m32 * other.m23 + m33 * other.m33 + m34 * other.m43,
            m41 * other.m13 + m42 * other.m23 + m43 * other.m33 + m44 * other.m43,
            // Fourth row
            m11 * other.m14 + m12 * other.m24 + m13 * other.m34 + m14 * other.m44,
            m21 * other.m14 + m22 * other.m24 + m23 * other.m34 + m24 * other.m44,
            m31 * other.m14 + m32 * other.m24 + m33 * other.m34 + m34 * other.m44,
            m41 * other.m14 + m42 * other.m24 + m43 * other.m34 + m44 * other.m44
        };
    }

    // Matrix * Vector
    Vector4 operator*(const Vector4& vec) const {
        return {
            m11 * vec.x + m12 * vec.y + m13 * vec.z + m14 * vec.w,
            m21 * vec.x + m22 * vec.y + m23 * vec.z + m24 * vec.w,
            m31 * vec.x + m32 * vec.y + m33 * vec.z + m34 * vec.w,
            m41 * vec.x + m42 * vec.y + m43 * vec.z + m44 * vec.w
        };
    }

    // Static factory methods for transformation
    static Matrix4 translate(const Vector3& t) {
        return {
            1, 0, 0, t.x,
            0, 1, 0, t.y,
            0, 0, 1, t.z,
            0, 0, 0, 1
        };
    }

    static Matrix4 scale(const Vector3& s) {
        return {
            s.x, 0,   0,   0,
            0,   s.y, 0,   0,
            0,   0,   s.z, 0,
            0,   0,   0,   1
        };
    }

    static Matrix4 rotateX(double angle) {
        const double c = cos(angle);
        const double s = sin(angle);
        return {
            1, 0,  0, 0,
            0, c, -s, 0,
            0, s,  c, 0,
            0, 0,  0, 1
        };
    }

    static Matrix4 rotateY(double angle) {
        const double c = cos(angle);
        const double s = sin(angle);
        return {
            c,  0, s, 0,
            0,  1, 0, 0,
            -s, 0, c, 0,
            0,  0, 0, 1
        };
    }

    static Matrix4 rotateZ(double angle) {
        const double c = cos(angle);
        const double s = sin(angle);
        return {
            c, -s, 0, 0,
            s,  c, 0, 0,
            0,  0, 1, 0,
            0,  0, 0, 1
        };
    }

    static Matrix4 createTransformationMatrix(const Vector3& t, const Vector3& r, const Vector3& s) {
        return translate(t) * rotateZ(r.z) * rotateY(r.y) * rotateX(r.x) * scale(s);
    }

    [[nodiscard]] Matrix4 invert() const {
        // Compute the cofactors of the matrix
        const double cofactor11 = m22 * (m33 * m44 - m34 * m43) - m23 * (m32 * m44 - m34 * m42) + m24 * (m32 * m43 - m33 * m42);
        const double cofactor12 = -(m21 * (m33 * m44 - m34 * m43) - m23 * (m31 * m44 - m34 * m41) + m24 * (m31 * m43 - m33 * m41));
        const double cofactor13 = m21 * (m32 * m44 - m34 * m42) - m22 * (m31 * m44 - m34 * m41) + m24 * (m31 * m42 - m32 * m41);
        const double cofactor14 = -(m21 * (m32 * m43 - m33 * m42) - m22 * (m31 * m43 - m33 * m41) + m23 * (m31 * m42 - m32 * m41));

        const double cofactor21 = -(m12 * (m33 * m44 - m34 * m43) - m13 * (m32 * m44 - m34 * m42) + m14 * (m32 * m43 - m33 * m42));
        const double cofactor22 = m11 * (m33 * m44 - m34 * m43) - m13 * (m31 * m44 - m34 * m41) + m14 * (m31 * m43 - m33 * m41);
        const double cofactor23 = -(m11 * (m32 * m44 - m34 * m42) - m12 * (m31 * m44 - m34 * m41) + m14 * (m31 * m42 - m32 * m41));
        const double cofactor24 = m11 * (m32 * m43 - m33 * m42) - m12 * (m31 * m43 - m33 * m41) + m13 * (m31 * m42 - m32 * m41);

        const double cofactor31 = m12 * (m23 * m44 - m24 * m43) - m13 * (m22 * m44 - m24 * m42) + m14 * (m22 * m43 - m23 * m42);
        const double cofactor32 = -(m11 * (m23 * m44 - m24 * m43) - m13 * (m21 * m44 - m24 * m41) + m14 * (m21 * m43 - m23 * m41));
        const double cofactor33 = m11 * (m22 * m44 - m24 * m42) - m12 * (m21 * m44 - m24 * m41) + m14 * (m21 * m42 - m22 * m41);
        const double cofactor34 = -(m11 * (m22 * m43 - m23 * m42) - m12 * (m21 * m43 - m23 * m41) + m13 * (m21 * m42 - m22 * m41));

        const double cofactor41 = -(m12 * (m23 * m34 - m24 * m33) - m13 * (m22 * m34 - m24 * m32) + m14 * (m22 * m33 - m23 * m32));
        const double cofactor42 = m11 * (m23 * m34 - m24 * m33) - m13 * (m21 * m34 - m24 * m31) + m14 * (m21 * m33 - m23 * m31);
        const double cofactor43 = -(m11 * (m22 * m34 - m24 * m32) - m12 * (m21 * m34 - m24 * m31) + m14 * (m21 * m32 - m22 * m31));
        const double cofactor44 = m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31);

        // Compute determinant
		const double determinant = m11 * cofactor11 + m12 * cofactor12 + m13 * cofactor13 + m14 * cofactor14;

        if (std::fabs(determinant) < EPSILON) {
            throw std::runtime_error("Matrix is singular and cannot be inverted.");
        }

        // Compute the inverse matrix by dividing the cofactors by the determinant
		const double invDet = 1.0 / determinant;
        return {
            cofactor11 * invDet, cofactor21 * invDet, cofactor31 * invDet, cofactor41 * invDet,
            cofactor12 * invDet, cofactor22 * invDet, cofactor32 * invDet, cofactor42 * invDet,
            cofactor13 * invDet, cofactor23 * invDet, cofactor33 * invDet, cofactor43 * invDet,
            cofactor14 * invDet, cofactor24 * invDet, cofactor34 * invDet, cofactor44 * invDet
        };
    }
};

/*class Matrix4 {
public:
    explicit Matrix4(const std::array<float, 16>& m);
    ~Matrix4() = default;

    float operator[](int) const;                    // Access matrix elements
    Vector4 operator*(const Vector4&) const;        // Matrix * vector
    Matrix4 operator*(const Matrix4&) const;        // Matrix * matrix

    static Matrix4 translate(const Vector3& t);
    static Matrix4 scale(const Vector3& s);
    static Matrix4 rotateX(float angle);
    static Matrix4 rotateY(float angle);
    static Matrix4 rotateZ(float angle);
    static Matrix4 createTransformationMatrix(const Vector3& t, const Vector3& r, const Vector3& s);

    [[nodiscard]] Matrix4 invert() const;           // Invert a 4x4 Matrix

private:
    std::array<float, 16> m{};  // Matrix stored in a std::array for safety and ease of use
};


inline Matrix4::Matrix4(const std::array<float, 16>& m) : m(m) {}

/** Matrix array access #1#
inline float Matrix4::operator[](const int index) const {
    if (index < 0 || index >= 16) {
        throw std::out_of_range("Index out of bounds for Matrix4x4");
    }
    return m[index];
}

/** Matrix times Vector #1#
inline Vector4 Matrix4::operator*(const Vector4& vec) const {
    return {
        m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12] * vec.w,
        m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13] * vec.w,
        m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14] * vec.w,
        m[3] * vec.x + m[7] * vec.y + m[11] * vec.z + m[15] * vec.w
    };
}

/** Matrix times Matrix #1#
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

/** Matrix inversion #1#
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
}*/

#endif // MATRIX4_H
