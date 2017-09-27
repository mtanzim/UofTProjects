/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1

    Instructions:
        See main.cpp for more instructions.

        This file contains the class OpenGL portion of the main window.
**************************************************************************/

#include "GLWidget.h"
#include <iostream>

static QGLFormat createQGLFormat()
{
    // Return a QGLFormat object that tells Qt information about what
    // OpenGL context we would like.
    QGLFormat format(QGL::SampleBuffers);
    if (supportsNewOpenGL())
    {
	// Choose a version of OpenGL that supports vertex array objects and
	// tell it that we do not want support for deprecated functions.
        format.setVersion(3, 3);
        format.setProfile(QGLFormat::CoreProfile);
    }
    return format;
}

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(createQGLFormat(), parent)
    , m_is_animating(false)
    , m_animation_frame(0)
    , m_joint_angle(0)
{
    // Start a timer that will call the timerEvent method every 50ms.
    startTimer(/*milliseconds=*/50);
}

void GLWidget::initializeGL()
{

	//added variables to help in making trapezoids
	//factor to widen the base of the trapezoid
	const float TRAP_F=1.5;
	//only wides one side of the trapezoid
	const float BEAK_F=1.5;
    m_gl_state.initializeGL();

    // To aid with troubleshooting, print out which version of OpenGL we've
    // told the driver to use.
    std::cout << "Using OpenGL: " << glGetString(GL_VERSION) << std::endl;

    // Copy the data for the shapes we'll draw into the video card's memory.
    m_unit_square.initialize(m_gl_state.VERTEX_POSITION_SHADER_LOCATION);

	//initialize added shapes
	m_trapezoid.initialize(m_gl_state.VERTEX_POSITION_SHADER_LOCATION, TRAP_F);
	m_triangle.initialize(m_gl_state.VERTEX_POSITION_SHADER_LOCATION);
	m_beak.initialize(m_gl_state.VERTEX_POSITION_SHADER_LOCATION, BEAK_F);

    m_unit_circle.initialize(
	m_gl_state.VERTEX_POSITION_SHADER_LOCATION,
	/*num_circle_segments=*/100);

    // Tell OpenGL what color to fill the background to when clearing.
    glClearColor(/*red=*/1.0f, /*green=*/1.0f, /*blue=*/1.0f, /*alpha=*/1.0f);

}

void GLWidget::resizeGL(int width, int height)
{
    // Respond to the window being resized by updating the viewport and
    // projection matrices.

    checkForGLErrors();

    // Setup projection matrix for new window
    m_gl_state.setOrthographicProjectionFromWidthAndHeight(width, height);

    // Update OpenGL viewport and internal variables
    glViewport(0, 0, width, height);
    checkForGLErrors();
}

void GLWidget::timerEvent(QTimerEvent *)
{
    // Respond to a timer going off telling us to update the animation.
    if (!m_is_animating)
        return;

    // increment the frame number.
    m_animation_frame++;

    // Update joint angles.
    const double joint_rot_speed = 0.1;
    double joint_rot_t =
        (sin(m_animation_frame * joint_rot_speed) + 1.0) / 2.0;
    m_joint_angle = joint_rot_t * ARM_JOINT_MIN + (1 - joint_rot_t) * ARM_JOINT_MAX;




    //////////////////////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function to animate the character's joints
    //   Note: Nothing should be drawn in this function!
    //////////////////////////////////////////////////////////////////////////


	//joint for head rotation
    double joint_rot_head =
        (sin(m_animation_frame * joint_rot_speed) + 1.0) / 2.0;
    head_joint_angle = joint_rot_head * HEAD_JOINT_MIN + (1 - joint_rot_head) * HEAD_JOINT_MAX;

	//joint for left toe
    double joint_rot_ll =
        (sin(m_animation_frame * joint_rot_speed) + 1.0) / 2.0;
    ll_joint_angle = joint_rot_ll * LL_JOINT_MIN + (1 - joint_rot_ll) * LL_JOINT_MAX;

	//joint for left leg
    double joint_rot_lu =
        (sin(m_animation_frame * joint_rot_speed) + 1.0) / 2.0;
    lu_joint_angle = joint_rot_lu * LU_JOINT_MIN + (1 - joint_rot_lu) * LU_JOINT_MAX;
	//joint for right toe
    double joint_rot_rl =
        (sin(m_animation_frame * joint_rot_speed) + 1.0) / 2.0;
    rl_joint_angle = joint_rot_rl * RL_JOINT_MIN + (1 - joint_rot_rl) * RL_JOINT_MAX;

	//joint for right leg
    double joint_rot_ru =
        (sin(m_animation_frame * joint_rot_speed) + 1.0) / 2.0;
    ru_joint_angle = joint_rot_ru * RU_JOINT_MIN + (1 - joint_rot_ru) * RU_JOINT_MAX;



///beak up and down

    double joint_beak =(sin(m_animation_frame * joint_rot_speed) + 1.0) / 2.0;
    beak_joint_angle = joint_beak * BEAK_JOINT_MIN + (1 - joint_beak) * BEAK_JOINT_MAX;



///torso x

    double joint_x =(sin(m_animation_frame * joint_rot_speed) + 1.0) / 2.0;
    x_joint_angle = joint_x * X_JOINT_MIN + (1 - joint_x) * X_JOINT_MAX;

///torso y
    double joint_y =(sin(m_animation_frame * joint_rot_speed) + 1.0) / 2.0;
    y_joint_angle = joint_y * Y_JOINT_MIN + (1 - joint_y) * Y_JOINT_MAX;

    // Tell this widget to redraw itself.
    update();
}

void GLWidget::paintGL()
{
    // This method gets called by the event handler to draw the scene, so
    // this is where you need to build your scene -- make your changes and
    // additions here.  All rendering happens in this function.

    checkForGLErrors();

    // Clear the screen with the background colour.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    // Setup the model-view transformation matrix stack.
    transformStack().loadIdentity();
    checkForGLErrors();

    //////////////////////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function draw the scene.  This should include function
    //   calls to pieces that apply the appropriate transformation matrices
    //   and render the individual body parts.
    //////////////////////////////////////////////////////////////////////////

    
	//Tanzim's Code starts here





	//trapezoid factors declared again for proper placement b/w parts
    const double TRAP_F=1.5;
	//these body variables can be used to scale the entire penguin
    const float BODY_WIDTH = 90.0;
    const float BODY_LENGTH = 140.0;

    float ARM_LENGTH = 0.45*BODY_LENGTH;
    float ARM_WIDTH = BODY_WIDTH/7;

    const float HEAD_LENGTH_FACTOR = 0.33;
    float HEAD_WIDTH_FACTOR = 1/TRAP_F;

	 //for the triangle placed on the trapezoidal head
	 const float TOP_HEAD_LENGTH_FACTOR = 0.2;
	 //float TOP_BEAK_LENGTH = HEAD_LENGTH/5;


	//beak factors relative to the head
	 const float BOT_BEAK_LENGTH_FACTOR = 0.2;
	 const float BOT_BEAK_WIDTH_FACTOR = 0.6;

	 const float TOP_BEAK_LENGTH_FACTOR = 0.2;
	 const float TOP_BEAK_WIDTH_FACTOR = 0.6;

	//for scaling the legs
	 float LEG_LENGTH = 0.75*ARM_LENGTH;
	 float LEG_WIDTH = ARM_WIDTH;



/////////////////TORSO////////////
//Start by drawing the lower torso, this is the 'root'


	transformStack().pushMatrix();

		transformStack().translate(x_joint_angle, y_joint_angle);
		//draw penguin torso upper (draw, colour, scale)
		transformStack().pushMatrix();
			transformStack().scale(BODY_WIDTH, BODY_LENGTH);
			m_gl_state.setColor(0.0, 0.0, 1.0);
			m_trapezoid.draw();

		transformStack().popMatrix();	

		//draw penguin lower torso relative to world (draw, colour, scale, translate) by inverting the trapezoid, and scaling it
    	transformStack().pushMatrix(); //lower torso start
				transformStack().translate(0, -0.75*BODY_LENGTH);
				transformStack().scale(1.0*BODY_WIDTH, -0.5*BODY_LENGTH);
				m_gl_state.setColor(0.0, 0.0, 1.0);
				m_trapezoid.draw();
		transformStack().popMatrix(); //lower torso end, back to upper torsso 

/////////PENGUIN HEAD///////////////
		//draw penguin head_bottom relative to torso
    	transformStack().pushMatrix(); //start head bottom related to upper torso

        transformStack().translate(0.0,1.5*HEAD_LENGTH_FACTOR*BODY_LENGTH);
        transformStack().rotateInDegrees(head_joint_angle);

        transformStack().scale(HEAD_WIDTH_FACTOR*BODY_WIDTH,HEAD_LENGTH_FACTOR*BODY_LENGTH);

		  ///draw joint circle
		  transformStack().pushMatrix();
				transformStack().scale(0.1, 0.1);
				m_gl_state.setColor(0.0, 1.0, 0.0);
				m_unit_circle.draw();
		  transformStack().popMatrix();
		  //end draw joint cirlce

			//set the location of the top joint
			transformStack().translate(0.0, 0.5);
         m_gl_state.setColor(1.0, 0.0, 0.0);
    		m_trapezoid.draw();

			//draw eyes relative to bottom head
			transformStack().pushMatrix();

				transformStack().translate(-0.25, 0.25);
				transformStack().scale(0.15, 0.12);
				m_gl_state.setColor(1.0, 1.0, 1.0);
				m_unit_circle.draw();


				//pupils relative to eye
				transformStack().scale(0.6, 0.6);
				m_gl_state.setColor(0.0, 0.0, 0.0);
				m_unit_circle.draw();

			transformStack().popMatrix();
			//end eye

			//draw penguin head_top relative to head_bottom
			transformStack().pushMatrix(); //start head top relative to head_bot
				transformStack().translate(0.0, 0.5+(TOP_HEAD_LENGTH_FACTOR*0.5));
				transformStack().scale(1.0, TOP_HEAD_LENGTH_FACTOR);
				m_gl_state.setColor(1.0, 0.0, 0.0);
				m_triangle.draw();
			transformStack().popMatrix(); //close head bot




			//draw penguin beak_top relative to the head_bot
			//Note: this must be isolated to avoid shearing
			transformStack().pushMatrix(); 
				transformStack().translate(-0.9, 0.0);
				//animation factor
				transformStack().translate(0.0,0.2*(beak_joint_angle/100));
			
				transformStack().scale(TOP_BEAK_WIDTH_FACTOR, TOP_BEAK_LENGTH_FACTOR);
				m_gl_state.setColor(0.0, 0.0, 0.0);
				m_beak.draw();
			transformStack().popMatrix(); //close beak top
		
			//draw penguin beak_bottom
		 	transformStack().pushMatrix(); //start beak_bot relative to head bot

				transformStack().translate(-0.9, -1.5*TOP_BEAK_LENGTH_FACTOR);
				transformStack().scale(BOT_BEAK_WIDTH_FACTOR, BOT_BEAK_LENGTH_FACTOR);
				m_gl_state.setColor(0.0, 0.0, 0.0);
				m_unit_square.draw();
			transformStack().popMatrix();//close beak bot

		transformStack().popMatrix(); //close head bot

/////////PENGUIN HEAD END///////////////
//The head is the 'root' of beaks, eye, but a branch of torso

///////////////////ARM
//draw the arm relative to world coordinates
		transformStack().pushMatrix();
			transformStack().translate(0.25*BODY_WIDTH,0.25*BODY_LENGTH);
			// Rotate along the hinge
			transformStack().rotateInDegrees(m_joint_angle);
			transformStack().scale(ARM_WIDTH, -ARM_LENGTH);
			//draw joint cirlce
	  		transformStack().pushMatrix();
				transformStack().scale(0.25, 0.075);
				m_gl_state.setColor(0.0, 1.0, 0.0);
				m_unit_circle.draw();
	  		transformStack().popMatrix();
			//end draw joint cirlce
			// Move to center location of arm, under previous rotation
			transformStack().translate(0.0, 0.5);
			m_gl_state.setColor(1.0, 0.0, 0.0);
			m_trapezoid.draw();
		transformStack().popMatrix();
///////////////////ARM END

//////////LEFT LEGS
//draw legs relative to world coordinates
		//UPPER LEG
		transformStack().pushMatrix();

			// Move the leg to the joint hinge
			transformStack().translate(-BODY_WIDTH/3,-0.875*BODY_LENGTH);
			// Rotate along the hinge
			transformStack().rotateInDegrees(lu_joint_angle);
			//ISOLATE SCALE TO AVOID SHEARING
			transformStack().pushMatrix();
				// Scale square to size of body
				transformStack().scale(LEG_WIDTH, LEG_LENGTH);

				///draw joint circle
				transformStack().pushMatrix();
					transformStack().scale(0.25, 0.1);
					m_gl_state.setColor(0.0, 1.0, 0.0);
					m_unit_circle.draw();
				transformStack().popMatrix();
				//end draw joint cirlce
				transformStack().translate(0.0, -0.5);
				m_gl_state.setColor(1.0, 0.0, 0.0);
				m_unit_square.draw();
			transformStack().popMatrix();

			///LOWER LEG
			//draw lower leg relative to upper leg
			transformStack().translate(0.0, -LEG_LENGTH);
			transformStack().rotateInDegrees(ll_joint_angle);
			transformStack().rotateInDegrees(-80);
			transformStack().scale(LEG_WIDTH, LEG_LENGTH);

			///draw joint circle
			transformStack().pushMatrix();
				transformStack().scale(0.25, 0.1);
				m_gl_state.setColor(0.0, 1.0, 0.0);
				m_unit_circle.draw();
			transformStack().popMatrix();

			transformStack().translate(0.0, -0.5);
			m_gl_state.setColor(0.0, 0.0, 1.0);
			m_unit_square.draw();

		transformStack().popMatrix();

/////////////////LEFT LEGS END
//draw legs relative to world coordinates

	//////////RIGHT LEGS
	//draw legs relative to world

		//UPPER LEG
		// Draw the 'body'
		transformStack().pushMatrix();
			transformStack().translate(BODY_WIDTH/3,-0.875*BODY_LENGTH);
			// Rotate along the hinge
			transformStack().rotateInDegrees(ru_joint_angle);
			//GET RID OF SCALE TO AVOID SHEARING
			transformStack().pushMatrix();
			// Scale square to size of body
				transformStack().scale(LEG_WIDTH, LEG_LENGTH);
				///draw joint circle
				transformStack().pushMatrix();
					transformStack().scale(0.25, 0.1);
					m_gl_state.setColor(0.0, 1.0, 0.0);
					m_unit_circle.draw();
				transformStack().popMatrix();
				//end draw joint cirlce
				transformStack().translate(0.0, -0.5);
				m_gl_state.setColor(1.0, 0.0, 0.0);
				m_unit_square.draw();
			transformStack().popMatrix();


			///LOWER LEG
			//draw toes relative to leg
			transformStack().translate(0.0, -LEG_LENGTH);
			transformStack().rotateInDegrees(rl_joint_angle);
			transformStack().rotateInDegrees(-80);
			transformStack().scale(LEG_WIDTH, LEG_LENGTH);

			///draw joint circle
			transformStack().pushMatrix();
				transformStack().scale(0.25, 0.1);
				m_gl_state.setColor(0.0, 1.0, 0.0);
				m_unit_circle.draw();
			transformStack().popMatrix();
			//end draw joint cirlce

			transformStack().translate(0.0, -0.5);
			m_gl_state.setColor(0.0, 0.0, 1.0);
			m_unit_square.draw();

		transformStack().popMatrix();
/////////////////RIGHT LEGS END

	transformStack().popMatrix(); //close torso***************************

/////////////////TORSO END

    // Execute any GL functions that are in the queue just to be safe
    glFlush();
    checkForGLErrors();
}

