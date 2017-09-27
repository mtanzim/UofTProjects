/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1
  
  
    Instructions:
        See main.cpp for more instructions.

	This file contains a class for 3x3 matrices to represent 2D
	transformations.
**************************************************************************/

#ifndef __Matrix3x3_h__
#define __Matrix3x3_h__

#include "include_gl.h"
#include <iostream>
#include <cmath>

class Matrix3x3
{
public:
    // Note that the interface for this class and the internal representation
    // use row-major order.  OpenGL uses column-major order, so we transpose
    // the matrix when we send it to OpenGL.
    Matrix3x3(
	GLfloat v0, GLfloat v1, GLfloat v2,
	GLfloat v3, GLfloat v4, GLfloat v5,
	GLfloat v6, GLfloat v7, GLfloat v8)
    {
	v[0] = v0; v[1] = v1; v[2] = v2;
	v[3] = v3; v[4] = v4; v[5] = v5;
	v[6] = v6; v[7] = v7; v[8] = v8;
    }

    Matrix3x3(GLfloat d=0.0)
    {
	v[0] = d;   v[1] = 0.0; v[2] = 0.0;
	v[3] = 0.0; v[4] = d;   v[5] = 0.0;
	v[6] = 0.0; v[7] = 0.0; v[8] = d;
    }

    const GLfloat *values() const
    { return v; }

    GLfloat operator[](int index) const
    { return v[index]; }

    static Matrix3x3 buildIdentity()
    {
	return Matrix3x3(
	    1.0, 0.0, 0.0,
	    0.0, 1.0, 0.0,
	    0.0, 0.0, 1.0);
    }

    static Matrix3x3 buildScale(GLfloat sx, GLfloat sy)
    {
	return Matrix3x3(
	    sx,  0.0, 0.0,
	    0.0, sy,  0.0,
	    0.0, 0.0, 1.0);
    }

    static Matrix3x3 buildRotationFromRadians(GLfloat angle)
    {
	GLfloat cos_angle = cos(angle);
	GLfloat sin_angle = sin(angle);
	return Matrix3x3(
	    cos_angle, -sin_angle, 0.0,
	    sin_angle,  cos_angle, 0.0,
	    0.0,        0.0,       1.0);
    }

    static Matrix3x3 buildRotationFromDegrees(GLfloat angle)
    { return buildRotationFromRadians(M_PI * angle / 180.0); }

    static Matrix3x3 buildTranslation(GLfloat tx, GLfloat ty)
    {
	return Matrix3x3(
	    1.0, 0.0, tx,
	    0.0, 1.0, ty,
	    0.0, 0.0, 1.0);
    }

    // Return "*this * m" (i.e. post-multiplication).
    Matrix3x3 operator*(const Matrix3x3 &m) const
    {
	return Matrix3x3(
	    v[0] * m.v[0] + v[1] * m.v[3] + v[2] * m.v[6],
	    v[0] * m.v[1] + v[1] * m.v[4] + v[2] * m.v[7],
	    v[0] * m.v[2] + v[1] * m.v[5] + v[2] * m.v[8],

	    v[3] * m.v[0] + v[4] * m.v[3] + v[5] * m.v[6],
	    v[3] * m.v[1] + v[4] * m.v[4] + v[5] * m.v[7],
	    v[3] * m.v[2] + v[4] * m.v[5] + v[5] * m.v[8],

	    v[6] * m.v[0] + v[7] * m.v[3] + v[8] * m.v[6],
	    v[6] * m.v[1] + v[7] * m.v[4] + v[8] * m.v[7],
	    v[6] * m.v[2] + v[7] * m.v[5] + v[8] * m.v[8]);
    }

    void premultiplyBy(const Matrix3x3 &m)
    { *this = m * *this; }

    void postmultiplyBy(const Matrix3x3 &m)
    { *this = *this * m; }

    friend std::ostream &operator<<(std::ostream &os, const Matrix3x3 &m);

private:
    GLfloat v[9];
};

inline std::ostream &operator<<(std::ostream &os, const Matrix3x3 &m)
{
    const GLfloat *v = m.v;
    os << "[" << v[0] << " " << v[1] << " " << v[2] << "]\n";
    os << "[" << v[3] << " " << v[4] << " " << v[5] << "]\n";
    os << "[" << v[6] << " " << v[7] << " " << v[8] << "]";
    return os;
}

#endif
