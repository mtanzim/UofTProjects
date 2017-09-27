/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1
  
  
    Instructions:
        See main.cpp for more instructions.

        This file contains a class to draw a series of vertices.  It it used
	to draw basic shapes using OpenGL.
**************************************************************************/

#ifndef __GLShape_h__
#define __GLShape_h__

#include "include_gl.h"
#include <QtOpenGL/QGLFormat>
#include <iostream>
#include <cassert>

class GLShape
{
public:
    GLShape()
        : m_num_vertices(0)
        , m_drawing_mode(GL_LINES)
        , m_vao(0)
        , m_buffer(0)
    {}

    void initialize(
	int shader_input_location,
        const GLfloat *vertices,
        int num_vertices,
        GLenum drawing_mode=GL_LINES)
    {
        m_num_vertices = num_vertices;
        m_drawing_mode = drawing_mode;

        checkForGLErrors();

        if (supportsNewOpenGL())
        {
            // Generate a vertex array object (VAO) and make it active.  Note
	    // that if an old OpenGL driver is being used and we try to
	    // generate a VAO, it will fail by returning zero but will not
	    // generate an OpenGL error.
            glGenVertexArrays(1, &m_vao);
            assert(m_vao != 0);
            checkForGLErrors();
            glBindVertexArray(m_vao);

            // Generate a buffer inside the current VAO and make it active.
            glGenBuffers(1, &m_buffer);
            assert(m_buffer != 0);
            glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

            // Copy the vertex data into the buffer.
            glBufferData(
                GL_ARRAY_BUFFER,
                sizeof(GLfloat) * 2 * num_vertices,
                vertices,
                GL_STATIC_DRAW);

            // Tell OpenGL that it can find the vertex data at the beginning of
            // the buffer.
            glVertexAttribPointer(
                shader_input_location,
                /*size=*/2,             // number of values per vertex
                /*type=*/GL_FLOAT,
                /*normalize=*/GL_FALSE, // don't normalize the data
                /*stride=*/0,           // no gaps between positions
                BUFFER_OFFSET(0));      // data starts at beginning of buffer
            glEnableVertexAttribArray(shader_input_location);
            checkForGLErrors();
        }
        else
        {
            // Copy the vertex data into a std::vector.
            m_vertex_data.assign(vertices, vertices + 2 * num_vertices);
        }
    }

    void draw()
    { drawWithMode(m_drawing_mode); }

    void drawWithMode(GLenum mode)
    {
        if (m_vao != 0)
        {
            // New OpenGL is supported: make the VAO active and draw its
            // contents.
            glBindVertexArray(m_vao);
            glDrawArrays(mode, /*first=*/0, m_num_vertices);
        }
        else
        {
            // Use old OpenGL's immediate mode.
            glBegin(mode);
            for (int i=0; i<(int)m_vertex_data.size(); i += 2)
                glVertex2f(m_vertex_data[i], m_vertex_data[i + 1]);
            glEnd();
        }
    }

private:
    int m_num_vertices;
    GLenum m_drawing_mode;

    // New versions of OpenGL use the VAO and buffer to refer to the vertex
    // data.
    GLuint m_vao;
    GLuint m_buffer;

    // For old versions of OpenGL, we use this vector is to hold the vertex
    // data.
    std::vector<GLfloat> m_vertex_data;
};

#endif
