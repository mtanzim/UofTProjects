/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1
  
  
    Instructions:
        See main.cpp for more instructions.

	This file contains a class for 4x4 matrices to represent 3D
	transformations.
**************************************************************************/

#ifndef __Matrix4x4_h__
#define __Matrix4x4_h__

#include "include_gl.h"
#include "Matrix3x3.h"
#include <cmath>

class Matrix4x4
{
public:
    // Note that the interface for this class and the internal representation
    // use row-major order.  OpenGL uses column-major order, so we transpose
    // the matrix when we send it to OpenGL.
    Matrix4x4(
	GLfloat v0,  GLfloat v1,  GLfloat v2,  GLfloat v3,
	GLfloat v4,  GLfloat v5,  GLfloat v6,  GLfloat v7,
	GLfloat v8,  GLfloat v9,  GLfloat v10, GLfloat v11,
	GLfloat v12, GLfloat v13, GLfloat v14, GLfloat v15)
    {
	v[ 0] = v0;  v[ 1] = v1;  v[ 2] = v2;  v[ 3] = v3;
	v[ 4] = v4;  v[ 5] = v5;  v[ 6] = v6;  v[ 7] = v7;
	v[ 8] = v8;  v[ 9] = v9;  v[10] = v10; v[11] = v11;
	v[12] = v12; v[13] = v13; v[14] = v14; v[15] = v15;
    }

    Matrix4x4(GLfloat d=0.0)
    {
	v[ 0] = d;   v[ 1] = 0.0; v[ 2] = 0.0; v[ 3] = 0.0;
	v[ 4] = 0.0; v[ 5] = d;   v[ 6] = 0.0; v[ 7] = 0.0;
	v[ 8] = 0.0; v[ 9] = 0.0; v[10] = d;   v[11] = 0.0;
	v[12] = 0.0; v[13] = 0.0; v[14] = 0.0; v[15] = d;
    }

    Matrix4x4(const Matrix3x3 &m)
    {
	v[ 0] = m[0]; v[ 1] = m[1]; v[ 2] = 0.0; v[ 3] = m[2];
	v[ 4] = m[3]; v[ 5] = m[4]; v[ 6] = 0.0; v[ 7] = m[5];
	v[ 8] = 0.0;  v[ 9] = 0.0;  v[10] = 1.0; v[11] = 0.0;
	v[12] = m[6]; v[13] = m[7]; v[14] = 0.0; v[15] = m[8];
    }

    const GLfloat *values() const
    { return v; }

    static Matrix4x4 buildIdentity()
    {
	return Matrix4x4(
	    1.0, 0.0, 0.0, 0.0,
	    0.0, 1.0, 0.0, 0.0,
	    0.0, 0.0, 1.0, 0.0,
	    0.0, 0.0, 0.0, 1.0);
    }

    static Matrix4x4 buildOrtho(
	GLfloat /*left*/ l, GLfloat /*right*/ r,
	GLfloat /*bottom*/ b, GLfloat /*top*/ t,
	GLfloat /*near*/ n, GLfloat /*far*/ f)
    {
	return Matrix4x4(
	    2.0/(r-l), 0.0, 0.0, (r+l)/(l-r),
	    0.0, 2.0/(t-b), 0.0, (t+b)/(b-t),
	    0.0, 0.0, 2.0/(n-f), (f+n)/(n-f),
	    0.0, 0.0, 0.0,       1.0);
    }

    // Return "*this * m" (i.e. post-multiplication).
    Matrix4x4 operator*(const Matrix4x4 &m) const
    {
	return Matrix4x4(
	    v[ 0]*m.v[ 0] + v[ 1]*m.v[ 4] + v[ 2]*m.v[ 8] + v[ 3]*m.v[12],
	    v[ 0]*m.v[ 1] + v[ 1]*m.v[ 5] + v[ 2]*m.v[ 9] + v[ 3]*m.v[13],
	    v[ 0]*m.v[ 2] + v[ 1]*m.v[ 6] + v[ 2]*m.v[10] + v[ 3]*m.v[14],
	    v[ 0]*m.v[ 3] + v[ 1]*m.v[ 7] + v[ 2]*m.v[11] + v[ 3]*m.v[15],

	    v[ 4]*m.v[ 0] + v[ 5]*m.v[ 4] + v[ 6]*m.v[ 8] + v[ 7]*m.v[12],
	    v[ 4]*m.v[ 1] + v[ 5]*m.v[ 5] + v[ 6]*m.v[ 9] + v[ 7]*m.v[13],
	    v[ 4]*m.v[ 2] + v[ 5]*m.v[ 6] + v[ 6]*m.v[10] + v[ 7]*m.v[14],
	    v[ 4]*m.v[ 3] + v[ 5]*m.v[ 7] + v[ 6]*m.v[11] + v[ 7]*m.v[15],

	    v[ 8]*m.v[ 0] + v[ 9]*m.v[ 4] + v[10]*m.v[ 8] + v[11]*m.v[12],
	    v[ 8]*m.v[ 1] + v[ 9]*m.v[ 5] + v[10]*m.v[ 9] + v[11]*m.v[13],
	    v[ 8]*m.v[ 2] + v[ 9]*m.v[ 6] + v[10]*m.v[10] + v[11]*m.v[14],
	    v[ 8]*m.v[ 3] + v[ 9]*m.v[ 7] + v[10]*m.v[11] + v[11]*m.v[15],

	    v[12]*m.v[ 0] + v[13]*m.v[ 4] + v[14]*m.v[ 8] + v[15]*m.v[12],
	    v[12]*m.v[ 1] + v[13]*m.v[ 5] + v[14]*m.v[ 9] + v[15]*m.v[13],
	    v[12]*m.v[ 2] + v[13]*m.v[ 6] + v[14]*m.v[10] + v[15]*m.v[14],
	    v[12]*m.v[ 3] + v[13]*m.v[ 7] + v[14]*m.v[11] + v[15]*m.v[15]);
    }

    // Set this matrix to "m * *this" (i.e. pre-multiply this matrix by m).
    void premultiplyBy(const Matrix4x4 &m)
    {
	v[ 0] = m.v[ 0]*v[ 0] + m.v[ 1]*v[ 4] + m.v[ 2]*v[ 8] + m.v[ 3]*v[12];
	v[ 1] = m.v[ 0]*v[ 1] + m.v[ 1]*v[ 5] + m.v[ 2]*v[ 9] + m.v[ 3]*v[13];
	v[ 2] = m.v[ 0]*v[ 2] + m.v[ 1]*v[ 6] + m.v[ 2]*v[10] + m.v[ 3]*v[14];
	v[ 3] = m.v[ 0]*v[ 3] + m.v[ 1]*v[ 7] + m.v[ 2]*v[11] + m.v[ 3]*v[15];

	v[ 4] = m.v[ 4]*v[ 0] + m.v[ 5]*v[ 4] + m.v[ 6]*v[ 8] + m.v[ 7]*v[12];
	v[ 5] = m.v[ 4]*v[ 1] + m.v[ 5]*v[ 5] + m.v[ 6]*v[ 9] + m.v[ 7]*v[13];
	v[ 6] = m.v[ 4]*v[ 2] + m.v[ 5]*v[ 6] + m.v[ 6]*v[10] + m.v[ 7]*v[14];
	v[ 7] = m.v[ 4]*v[ 3] + m.v[ 5]*v[ 7] + m.v[ 6]*v[11] + m.v[ 7]*v[15];

	v[ 8] = m.v[ 8]*v[ 0] + m.v[ 9]*v[ 4] + m.v[10]*v[ 8] + m.v[11]*v[12];
	v[ 9] = m.v[ 8]*v[ 1] + m.v[ 9]*v[ 5] + m.v[10]*v[ 9] + m.v[11]*v[13];
	v[10] = m.v[ 8]*v[ 2] + m.v[ 9]*v[ 6] + m.v[10]*v[10] + m.v[11]*v[14];
	v[11] = m.v[ 8]*v[ 3] + m.v[ 9]*v[ 7] + m.v[10]*v[11] + m.v[11]*v[15];

	v[12] = m.v[12]*v[ 0] + m.v[13]*v[ 4] + m.v[14]*v[ 8] + m.v[15]*v[12];
	v[13] = m.v[12]*v[ 1] + m.v[13]*v[ 5] + m.v[14]*v[ 9] + m.v[15]*v[13];
	v[14] = m.v[12]*v[ 2] + m.v[13]*v[ 6] + m.v[14]*v[10] + m.v[15]*v[14];
	v[15] = m.v[12]*v[ 3] + m.v[13]*v[ 7] + m.v[14]*v[11] + m.v[15]*v[15];
    }

    // Set this matrix to "*this * m" (i.e. post-multiply this matrix by m).
    void postmultiplyBy(const Matrix4x4 &m)
    {
	v[ 0] = v[ 0]*m.v[ 0] + v[ 1]*m.v[ 4] + v[ 2]*m.v[ 8] + v[ 3]*m.v[12];
	v[ 1] = v[ 0]*m.v[ 1] + v[ 1]*m.v[ 5] + v[ 2]*m.v[ 9] + v[ 3]*m.v[13];
	v[ 2] = v[ 0]*m.v[ 2] + v[ 1]*m.v[ 6] + v[ 2]*m.v[10] + v[ 3]*m.v[14];
	v[ 3] = v[ 0]*m.v[ 3] + v[ 1]*m.v[ 7] + v[ 2]*m.v[11] + v[ 3]*m.v[15];

	v[ 4] = v[ 4]*m.v[ 0] + v[ 5]*m.v[ 4] + v[ 6]*m.v[ 8] + v[ 7]*m.v[12];
	v[ 5] = v[ 4]*m.v[ 1] + v[ 5]*m.v[ 5] + v[ 6]*m.v[ 9] + v[ 7]*m.v[13];
	v[ 6] = v[ 4]*m.v[ 2] + v[ 5]*m.v[ 6] + v[ 6]*m.v[10] + v[ 7]*m.v[14];
	v[ 7] = v[ 4]*m.v[ 3] + v[ 5]*m.v[ 7] + v[ 6]*m.v[11] + v[ 7]*m.v[15];

	v[ 8] = v[ 8]*m.v[ 0] + v[ 9]*m.v[ 4] + v[10]*m.v[ 8] + v[11]*m.v[12];
	v[ 9] = v[ 8]*m.v[ 1] + v[ 9]*m.v[ 5] + v[10]*m.v[ 9] + v[11]*m.v[13];
	v[10] = v[ 8]*m.v[ 2] + v[ 9]*m.v[ 6] + v[10]*m.v[10] + v[11]*m.v[14];
	v[11] = v[ 8]*m.v[ 3] + v[ 9]*m.v[ 7] + v[10]*m.v[11] + v[11]*m.v[15];

	v[12] = v[12]*m.v[ 0] + v[13]*m.v[ 4] + v[14]*m.v[ 8] + v[15]*m.v[12];
	v[13] = v[12]*m.v[ 1] + v[13]*m.v[ 5] + v[14]*m.v[ 9] + v[15]*m.v[13];
	v[14] = v[12]*m.v[ 2] + v[13]*m.v[ 6] + v[14]*m.v[10] + v[15]*m.v[14];
	v[15] = v[12]*m.v[ 3] + v[13]*m.v[ 7] + v[14]*m.v[11] + v[15]*m.v[15];
    }

    friend std::ostream &operator<<(std::ostream &os, const Matrix4x4 &m);

private:
    GLfloat v[16];
};

inline std::ostream &operator<<(std::ostream &os, const Matrix4x4 &m)
{
    const GLfloat *v = m.v;
    os << "[" << v[ 0] <<" "<< v[ 1] <<" "<< v[ 2] <<" "<< v[ 3] << "]\n";
    os << "[" << v[ 4] <<" "<< v[ 5] <<" "<< v[ 6] <<" "<< v[ 7] << "]\n";
    os << "[" << v[ 8] <<" "<< v[ 9] <<" "<< v[10] <<" "<< v[11] << "]\n";
    os << "[" << v[12] <<" "<< v[13] <<" "<< v[14] <<" "<< v[15] << "]";
    return os;
}

#endif
