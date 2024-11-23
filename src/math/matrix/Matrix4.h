#pragma once

#include <cmath>

#include "math/Util.h"
#include "math/vector/Vector4.h"


struct Matrix4 {
    float m11, m21, m31, m41;
    float m12, m22, m32, m42;
    float m13, m23, m33, m43;
    float m14, m24, m34, m44;

	// Constructor with initializer list
	Matrix4(const std::initializer_list<float> list) {
		if (list.size() != 16) {
			throw std::invalid_argument("Initializer list must have exactly 16 elements.");
		}
		auto it = list.begin();
		m11 = *it++; m21 = *it++; m31 = *it++; m41 = *it++;
		m12 = *it++; m22 = *it++; m32 = *it++; m42 = *it++;
		m13 = *it++; m23 = *it++; m33 = *it++; m43 = *it++;
		m14 = *it++; m24 = *it++; m34 = *it++; m44 = *it++;
	}

	// Constructor with array
	explicit Matrix4(const float* data) {
		m11 = data[0];  m21 = data[4];  m31 = data[8];   m41 = data[12];
		m12 = data[1];  m22 = data[5];  m32 = data[9];   m42 = data[13];
		m13 = data[2];  m23 = data[6];  m33 = data[10];  m43 = data[14];
		m14 = data[3];  m24 = data[7];  m34 = data[11];  m44 = data[15];
	}

    /**
     * @brief Multiplies this matrix by a given Vector4.
     *
     * This operator performs matrix-vector multiplication, resulting in a new Vector4.
     * Each component of the resulting vector is computed as the dot product of the
     * corresponding row of the matrix and the input vector.
     *
     * @param v The Vector4 to be multiplied by this matrix.
     * @return A new Vector4 that is the result of the matrix-vector multiplication.
     */
    Vector4 operator*(const Vector4 &v) const {
		return Vector4({
            m11 * v.x + m21 * v.y + m31 * v.z + m41 * v.w,
            m12 * v.x + m22 * v.y + m32 * v.z + m42 * v.w,
            m13 * v.x + m23 * v.y + m33 * v.z + m43 * v.w,
            m14 * v.x + m24 * v.y + m34 + v.z + m44 * v.w
        });
    }

    /**
     * @brief Overloaded multiplication operator for Matrix4.
     *
     * This operator multiplies the current Matrix4 instance with another Matrix4 instance.
     * The multiplication is performed in a standard matrix multiplication manner.
     *
     * @param other The right-hand side Matrix4 to be multiplied with the current instance.
     * @return Matrix4 The result of the matrix multiplication.
     */
    Matrix4 operator*(const Matrix4 &other) const {
		return Matrix4({
            m11 * other.m11 + m21 * other.m12 + m31 * other.m13 + m41 * other.m14,
            m11 * other.m21 + m21 * other.m22 + m31 * other.m23 + m41 * other.m24,
            m11 * other.m31 + m21 * other.m32 + m31 * other.m33 + m41 * other.m34,
            m11 * other.m41 + m21 * other.m42 + m31 * other.m43 + m41 * other.m44,

            m12 * other.m11 + m22 * other.m12 + m32 * other.m13 + m42 * other.m14,
            m12 * other.m21 + m22 * other.m22 + m32 * other.m23 + m42 * other.m24,
            m12 * other.m31 + m22 * other.m32 + m32 * other.m33 + m42 * other.m34,
            m12 * other.m41 + m22 * other.m42 + m32 * other.m43 + m42 * other.m44,

            m13 * other.m11 + m23 * other.m12 + m33 * other.m13 + m43 * other.m14,
            m13 * other.m21 + m23 * other.m22 + m33 * other.m23 + m43 * other.m24,
            m13 * other.m31 + m23 * other.m32 + m33 * other.m33 + m43 * other.m34,
            m13 * other.m41 + m23 * other.m42 + m33 * other.m43 + m43 * other.m44,

            m14 * other.m11 + m24 * other.m12 + m34 * other.m13 + m44 * other.m14,
            m14 * other.m21 + m24 * other.m22 + m34 * other.m23 + m44 * other.m24,
            m14 * other.m31 + m24 * other.m32 + m34 * other.m33 + m44 * other.m34,
            m14 * other.m41 + m24 * other.m42 + m34 * other.m43 + m44 * other.m44
        });
    }

	static Matrix4 translate(const Vector3& t) {
    	return Matrix4({
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1
		});
    }

	static Matrix4 scale(const Vector3& s) {
    	return Matrix4({
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		});
    }

	static Matrix4 rotateX(const float a) {
    	return Matrix4({
    		1,      0,       0, 0,
			0, cos(a), -sin(a), 0,
			0, sin(a),  cos(a), 0,
			0,      0,       0, 1
		});
    }

	static Matrix4 rotateY(const float a) {
    	return Matrix4({
    		cos(a), 0, sin(a), 0,
				 0, 1,      0, 0,
		   -sin(a), 0, cos(a), 0,
				 0, 0,      0, 1
		});
    }

	static Matrix4 rotateZ(const float a) {
    	return Matrix4({
    		cos(a), -sin(a), 0, 0,
			sin(a),  cos(a), 0, 0,
				 0,       0, 1, 0,
				 0,       0, 0, 1
		});
    }

	[[nodiscard]] Matrix4 invert() const {
	   float inv[16];

	    // Calculate the cofactor matrix
	    inv[0]  =  m22 * m33 * m44 - m22 * m34 * m43 - m32 * m23 * m44 + m32 * m24 * m43 + m42 * m23 * m34 - m42 * m24 * m33;
		inv[4]  = -m21 * m33 * m44 + m21 * m34 * m43 + m31 * m23 * m44 - m31 * m24 * m43 - m41 * m23 * m34 + m41 * m24 * m33;
		inv[8]  =  m21 * m32 * m44 - m21 * m34 * m42 - m31 * m22 * m44 + m31 * m24 * m42 + m41 * m22 * m34 - m41 * m24 * m32;
		inv[12] = -m21 * m32 * m43 + m21 * m33 * m42 + m31 * m22 * m43 - m31 * m23 * m42 - m41 * m22 * m33 + m41 * m23 * m32;

		inv[1]  = -m12 * m33 * m44 + m12 * m34 * m43 + m32 * m13 * m44 - m32 * m14 * m43 - m42 * m13 * m34 + m42 * m14 * m33;
		inv[5]  =  m11 * m33 * m44 - m11 * m34 * m43 - m31 * m13 * m44 + m31 * m14 * m43 + m41 * m13 * m34 - m41 * m14 * m33;
		inv[9]  = -m11 * m32 * m44 + m11 * m34 * m42 + m31 * m12 * m44 - m31 * m14 * m42 - m41 * m12 * m34 + m41 * m14 * m32;
		inv[13] =  m11 * m32 * m43 - m11 * m33 * m42 - m31 * m12 * m43 + m31 * m13 * m42 + m41 * m12 * m33 - m41 * m13 * m32;

		inv[2]  =  m12 * m23 * m44 - m12 * m24 * m43 - m22 * m13 * m44 + m22 * m14 * m43 + m42 * m13 * m24 - m42 * m14 * m23;
		inv[6]  = -m11 * m23 * m44 + m11 * m24 * m43 + m21 * m13 * m44 - m21 * m14 * m43 - m41 * m13 * m24 + m41 * m14 * m23;
		inv[10] =  m11 * m22 * m44 - m11 * m24 * m42 - m21 * m12 * m44 + m21 * m14 * m42 + m41 * m12 * m24 - m41 * m14 * m22;
		inv[14] = -m11 * m22 * m43 + m11 * m23 * m42 + m21 * m12 * m43 - m21 * m13 * m42 - m41 * m12 * m23 + m41 * m13 * m22;

		inv[3]  = -m12 * m23 * m34 + m12 * m24 * m33 + m22 * m13 * m34 - m22 * m14 * m33 - m32 * m13 * m24 + m32 * m14 * m23;
		inv[7]  =  m11 * m23 * m34 - m11 * m24 * m33 - m21 * m13 * m34 + m21 * m14 * m33 + m31 * m13 * m24 - m31 * m14 * m23;
		inv[11] = -m11 * m22 * m34 + m11 * m24 * m32 + m21 * m12 * m34 - m21 * m14 * m32 - m31 * m12 * m24 + m31 * m14 * m22;
		inv[15] =  m11 * m22 * m33 - m11 * m23 * m32 - m21 * m12 * m33 + m21 * m13 * m32 + m31 * m12 * m23 - m31 * m13 * m22;

		// Calculate the determinant
		float det = m11 * inv[0] + m12 * inv[4] + m13 * inv[8] + m14 * inv[12];

		if (det > -EPSILON && det < EPSILON) throw std::runtime_error("Matrix is singular and cannot be inverted.");

		det = 1.0f / det;

		// Scale the inverse matrix by the determinant
		for (float& i : inv) {
			i *= det;
		}

	    return Matrix4(inv);
	}

	/**
	 * Converts the matrix to column-major order and stores the result in the provided array.
	 *
	 * @param values A reference to a 16-element array of floats where the column-major matrix elements will be stored.
	 */
	void toColumnMajor(float (&values)[16]) const {
    	values[0] = m11;
    	values[1] = m12;
    	values[2] = m13;
    	values[3] = m14;
    	values[4] = m21;
    	values[5] = m22;
    	values[6] = m23;
    	values[7] = m24;
    	values[8] = m31;
    	values[9] = m32;
    	values[10] = m33;
    	values[11] = m34;
    	values[12] = m41;
    	values[13] = m42;
    	values[14] = m43;
    	values[15] = m44;
    }
};
