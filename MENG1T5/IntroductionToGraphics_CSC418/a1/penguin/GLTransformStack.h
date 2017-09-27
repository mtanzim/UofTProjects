/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1

    Instructions:
        See main.cpp for more instructions.

	This file contains a class to represent a stack of 3x3 matrices that
	perform 2D transformations.  The matrix on the top of the stack is
	the active one.
**************************************************************************/

#ifndef __GLTransformStack_h__
#define __GLTransformStack_h__

#include "include_gl.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include <QtOpenGL/QGLFormat>
#include <cassert>
#include <vector>

class GLTransformStack
{
public:
    GLTransformStack()
	: m_shader_location(-1)
    {
	// Start with the identity matrix on the stack.
	m_stack.push_back(Matrix3x3::buildIdentity());
    }

    // This method tells this object the shader input location for the active
    // transformation matrix.
    void setShaderLocation(int shader_location)
    {
	assert(shader_location >= 0);
	m_shader_location = shader_location;
	updateShaderTransform();
    }

    // Push a new transformation on the stack, copying the transformation that
    // was on the top of the stack.
    void pushMatrix()
    {
	if (supportsNewOpenGL())
	    m_stack.push_back(top());
	else
	    glPushMatrix();
    }

    // Pop an element off the stack.
    void popMatrix()
    {
	if (supportsNewOpenGL())
	{
	    // After popping, we must always leave at least one element on the
	    // stack.
	    m_stack.pop_back();
	    assert(m_stack.size() > 0);
	    updateShaderTransform();
	}
	else
	    glPopMatrix();
    }

    // Set the top of the stack to the identity matrix.
    void loadIdentity()
    {
	if (supportsNewOpenGL())
	{
	    top() = Matrix3x3::buildIdentity();
	    updateShaderTransform();
	}
	else
	    glLoadIdentity();
    }

    // Update the top of the stack to include a scale that gets applied before
    // all previous transformations.
    void scale(GLfloat sx, GLfloat sy)
    {
	if (supportsNewOpenGL())
	{
	    top().postmultiplyBy(Matrix3x3::buildScale(sx, sy));
	    updateShaderTransform();
	}
	else
	    glScalef(sx, sy, 1.0);
    }

    // Update the top of the stack to include a rotation that gets applied
    // before all previous transformations.
    void rotateInDegrees(GLfloat angle)
    {
	if (supportsNewOpenGL())
	{
	    top().postmultiplyBy(Matrix3x3::buildRotationFromDegrees(angle));
	    updateShaderTransform();
	}
	else
	    glRotatef(angle, 0.0, 0.0, 1.0);
    }

    // Update the top of the stack to include a translation that gets applied
    // before all previous transformations.
    void translate(GLfloat tx, GLfloat ty)
    {
	if (supportsNewOpenGL())
	{
	    top().postmultiplyBy(Matrix3x3::buildTranslation(tx, ty));
	    updateShaderTransform();
	}
	else
	    glTranslatef(tx, ty, 0.0);
    }

private:
    Matrix3x3 &top()
    { return m_stack[m_stack.size() - 1]; }

    const Matrix3x3 &top() const
    { return m_stack[m_stack.size() - 1]; }

    void updateShaderTransform()
    {
	// Set this matrix into the current shader program.
	assert(supportsNewOpenGL());
	assert(m_shader_location >= 0);
	glUniformMatrix4fv(
	    m_shader_location,
	    /*count=*/1,
	    /*transpose=*/true,
	    Matrix4x4(top()).values());
    }

private:
    int m_shader_location;
    std::vector<Matrix3x3> m_stack;
};

#endif
