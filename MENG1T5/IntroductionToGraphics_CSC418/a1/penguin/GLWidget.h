/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1

    Instructions:
        See main.cpp for more instructions.

        This file contains the OpenGL portion of the main window.
**************************************************************************/

#ifndef __GLWidget_h__
#define __GLWidget_h__

#include "include_gl.h"
#include "GLState.h"
#include "GLShape.h"
#include <QtOpenGL/QtOpenGL>

//triangle to draw the top of the head
class Triangle : public GLShape
{
public:
    using GLShape::initialize;

    void initialize(int shader_input_location)
    {

		//equilateral triangle


		// Use four triangles to create the torso.
        GLfloat triangle_vertices[][2] =
        {



            { -0.5, -0.5},  // Triangle 1
            {  0.5, -0.5 },
            {  0,  0.5 },

 				{ -0.5, -0.5},  // Triangle 1
            {  0.5, -0.5 },
            {  0,  0.5 },


        };

        initialize(
	    shader_input_location,
            reinterpret_cast<const GLfloat *>(triangle_vertices),
            /*num_vertices=*/6,
	    GL_TRIANGLES); // Each group of three coordinates is a triangle
    }
};



//assymetrical trapezoid to draw the beak
//width extension of one side depends on beak factor
class Beak_top : public GLShape
{
public:
    using GLShape::initialize;

    void initialize(int shader_input_location, float beak_factor)
    {

		//assume bottom is always wider


		// Use four triangles to create the torso.
        GLfloat beak_vertices[][2] =
        {



            { -0.5, -0.5},  // Triangle 1
            {  0.5, -0.5 },
            {  0.5,  0.5*beak_factor },
            { -0.5, -0.5 },  // Triangle 2
            {  0.5,  0.5*beak_factor },
            { -0.5,  0.5 },

        };

        initialize(
	    shader_input_location,
            reinterpret_cast<const GLfloat *>(beak_vertices),
            /*num_vertices=*/6,
	    GL_TRIANGLES); // Each group of three coordinates is a triangle
    }
};

//trapezoid class to draw the torso, and the head
//width extension of both sides depend on trap factor
class Trapezoid : public GLShape
{
public:
    using GLShape::initialize;

    void initialize(int shader_input_location, float trap_factor)
    {

		//assume bottom is always wider


		// Use four triangles to create the torso.
        GLfloat trapezoid_vertices[][2] =
        {



            { -0.5*trap_factor, -0.5},  // Triangle 1
            {  0.5*trap_factor, -0.5 },
            {  0.5,  0.5 },
            { -0.5*trap_factor, -0.5 },  // Triangle 2
            {  0.5,  0.5 },
            { -0.5,  0.5 },

        };

        initialize(
	    shader_input_location,
            reinterpret_cast<const GLfloat *>(trapezoid_vertices),
            /*num_vertices=*/6,
	    GL_TRIANGLES); // Each group of three coordinates is a triangle
    }
};


// Before transformed, this class displays a unit square centered at the
// origin.

class UnitSquare : public GLShape
{
public:
    using GLShape::initialize;

    void initialize(int shader_input_location)
    {
	// Use two triangles to create the square.
        GLfloat square_vertices[][2] =
        {
            { -0.5, -0.5},  // Triangle 1
            {  0.5, -0.5 },
            {  0.5,  0.5 },
            { -0.5, -0.5 },  // Triangle 2
            {  0.5,  0.5 },
            { -0.5,  0.5 },
        };

        initialize(
	    shader_input_location,
            reinterpret_cast<const GLfloat *>(square_vertices),
            /*num_vertices=*/6,
	    GL_TRIANGLES); // Each group of three coordinates is a triangle
    }
};

// Before transformed, this class displays a unit circle centered at the
// origin.
class UnitCircle : public GLShape
{
public:
    using GLShape::initialize;

    void initialize(int shader_input_location, int num_circle_segments)
    {
        // We will draw a circle as a triangle fan.  We are careful to send
	// the second coordinate twice to properly close the circle.
        //        3     2     1
        //         +----+----+
        //        / \   |   /
        //       /   \  |  /
        //      /     \ | /
        //     /       \|/
        //   4+---------+ 0
        //        ...
        std::vector<GLfloat> circle_vertices;
        circle_vertices.push_back(0.0);
        circle_vertices.push_back(0.0);
        for (int i=0; i<=num_circle_segments; ++i)
        {
            double angle = (2 * M_PI * i) / num_circle_segments;
            circle_vertices.push_back(cos(angle));
            circle_vertices.push_back(sin(angle));
        }

        initialize(
	    shader_input_location,
            &circle_vertices[0],
            num_circle_segments + 1,
	    GL_TRIANGLE_FAN);
    }
};

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    // These values control the bounds to display on the joint angle sliders.
    //////////////////////////////////////////////////////////////////////////
    // TODO:
    //   Add different ranges for the different joints.  Use these ranges
    //   when constructing sliders and when animating joints.
    //////////////////////////////////////////////////////////////////////////

	//max and min values limiting animation
    static const int ARM_JOINT_MIN = -45;
    static const int ARM_JOINT_MAX = 45;

	 static const int HEAD_JOINT_MIN = -15;
    static const int HEAD_JOINT_MAX = 15;

	 static const int LU_JOINT_MIN = -15;
    static const int LU_JOINT_MAX = 15;

	 static const int LL_JOINT_MIN = -10;
    static const int LL_JOINT_MAX = 10;

	 static const int RU_JOINT_MIN = -15;
    static const int RU_JOINT_MAX = 15;

	 static const int RL_JOINT_MIN = -10;
    static const int RL_JOINT_MAX = 10;

	 static const int BEAK_JOINT_MIN = -50;
    static const int BEAK_JOINT_MAX = 45;

	 static const int X_JOINT_MIN = -45;
    static const int X_JOINT_MAX = 45;

	 static const int Y_JOINT_MIN = -45;
    static const int Y_JOINT_MAX = 45;

    GLWidget(QWidget *parent=NULL);

public slots:
    // This method is called when the user changes the joint angle slider.
    //////////////////////////////////////////////////////////////////////////
    // TODO:
    //   There is currently only one joint, but you need to add more.
    //////////////////////////////////////////////////////////////////////////
    void setJointAngle(int angle)
    {
        // This method is called when the user changes the slider value.
        m_joint_angle = angle;

        // Call update() to trigger a redraw.
        update();
    }

    //added functions to animate different body parts
    void setHeadJointAngle(int angle)
    {
        // This method is called when the user changes the slider value.
        head_joint_angle = angle;

        // Call update() to trigger a redraw.
        update();
    }


    void setllJointAngle(int angle)
    {
        // This method is called when the user changes the slider value.
        ll_joint_angle = angle;

        // Call update() to trigger a redraw.
        update();
    }

    void setruJointAngle(int angle)
    {
        // This method is called when the user changes the slider value.
        ru_joint_angle = angle;

        // Call update() to trigger a redraw.
        update();
    }



    void setrlJointAngle(int angle)
    {
        // This method is called when the user changes the slider value.
        rl_joint_angle = angle;

        // Call update() to trigger a redraw.
        update();
    }

    void setluJointAngle(int angle)
    {
        // This method is called when the user changes the slider value.
        lu_joint_angle = angle;

        // Call update() to trigger a redraw.
        update();
    }

    void setBeakJointAngle(int angle)
    {
        // This method is called when the user changes the slider value.
        beak_joint_angle = angle;

        // Call update() to trigger a redraw.
        update();
    }

    void setXAngle(int angle)
    {
        // This method is called when the user changes the slider value.
        x_joint_angle = angle;

        // Call update() to trigger a redraw.
        update();
    }


    void setYAngle(int angle)
    {
        // This method is called when the user changes the slider value.
        y_joint_angle = angle;

        // Call update() to trigger a redraw.
        update();
    }

    void onPressAnimate(int is_animating)
    {
        // This method is called when the user changes the animation checkbox.
        m_is_animating = (bool)is_animating;
        m_animation_frame = 0;
        update();
    }

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void timerEvent(QTimerEvent *event);

private:
    GLTransformStack &transformStack()
    { return m_gl_state.transformStack(); }

private:
    GLState m_gl_state;
    bool m_is_animating;
    int m_animation_frame;
    UnitSquare m_unit_square;
    UnitCircle m_unit_circle;

	//tm add body parts
	Trapezoid m_trapezoid;
	Triangle m_triangle;
	Beak_top m_beak;

    //////////////////////////////////////////////////////////////////////////
    // TODO: Add additional joint parameters.
    //////////////////////////////////////////////////////////////////////////

	//added joint parameters
         double m_joint_angle;
	 double head_joint_angle;
	 double ll_joint_angle;
	 double lu_joint_angle;
	 double rl_joint_angle;
	 double ru_joint_angle;
	 double beak_joint_angle;

	 double x_joint_angle;
	 double y_joint_angle;
};

#endif
