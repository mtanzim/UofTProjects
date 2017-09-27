/***********************************************************
             CSC418, FALL 2009
 
                 penguin.cpp
                 author: Mike Pratscher
                 based on code by: Eron Steger, J. Radulovich

		Main source file for assignment 2
		Uses OpenGL, GLUT and GLUI libraries
  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design. In particular, see lines marked 'README'.
		
		Be sure to also look over keyframe.h and vector.h.
		While no changes are necessary to these files, looking
		them over will allow you to better understand their
		functionality and capabilites.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.

***********************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "keyframe.h"
#include "timer.h"
#include "vector.h"


// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

const float SPINNER_SPEED = 0.1;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;				// Glut window ID (for display)
int Win[2];					// window (x,y) size

GLUI* glui_joints;			// Glui window with joint controls
GLUI* glui_keyframe;		// Glui window with keyframe controls
GLUI* glui_render;			// Glui window for render style

char msg[256];				// String used for status message
GLUI_StaticText* status;	// Status message ("Status: <msg>")


// ---------------- ANIMATION VARIABLES ---------------------

// Camera settings
bool updateCamZPos = false;
//for rotating the penguin with the LMB
bool updateAngle=false;
int  lastX = 0;
int  lastY = 0;
const float ZOOM_SCALE = 0.01;

GLdouble camXPos =  0.0;
GLdouble camYPos =  0.0;
GLdouble camZPos = -9.5;

const GLdouble CAMERA_FOVY = 60.0;
const GLdouble NEAR_CLIP   = 0.1;
const GLdouble FAR_CLIP    = 1000.0;

// Render settings
enum { WIREFRAME, SOLID, OUTLINED, METALLIC, MATTE };	// README: the different render styles
enum { POLY_DEF, POLY_OUT };			// for the outlined render style while while drawing polygons
int renderStyle = OUTLINED;			// README: the selected render style

// Animation settings
int animate_mode = 0;			// 0 = no anim, 1 = animate

// Keyframe settings
const char filenameKF[] = "keyframes.txt";	// file for loading / saving keyframes

Keyframe* keyframes;			// list of keyframes

int maxValidKeyframe   = 0;		// index of max VALID keyframe (in keyframe list)
const int KEYFRAME_MIN = 0;
const int KEYFRAME_MAX = 32;	// README: specifies the max number of keyframes

// Frame settings
char filenameF[128];			// storage for frame filename

int frameNumber = 0;			// current frame being dumped
int frameToFile = 0;			// flag for dumping frames to file

const float DUMP_FRAME_PER_SEC = 24.0;		// frame rate for dumped frames
const float DUMP_SEC_PER_FRAME = 1.0 / DUMP_FRAME_PER_SEC;

// Time settings
Timer* animationTimer;
Timer* frameRateTimer;

const float TIME_MIN = 0.0;
const float TIME_MAX = 100.0;	// README: specifies the max time of the animation
const float SEC_PER_FRAME = 1.0 / 60.0;

// Joint settings

// README: This is the key data structure for
// updating keyframes in the keyframe list and
// for driving the animation.
//   i) When updating a keyframe, use the values
//      in this data structure to update the
//      appropriate keyframe in the keyframe list.
//  ii) When calculating the interpolated pose,
//      the resulting pose vector is placed into
//      this data structure. (This code is already
//      in place - see the animate() function)
// iii) When drawing the scene, use the values in
//      this data structure (which are set in the
//      animate() function as described above) to
//      specify the appropriate transformations.
Keyframe* joint_ui_data;

// README: To change the range of a particular DOF,
// simply change the appropriate min/max values below
const float ROOT_TRANSLATE_X_MIN = -5.0;
const float ROOT_TRANSLATE_X_MAX =  5.0;
const float ROOT_TRANSLATE_Y_MIN = -5.0;
const float ROOT_TRANSLATE_Y_MAX =  5.0;
const float ROOT_TRANSLATE_Z_MIN = -50.0;
const float ROOT_TRANSLATE_Z_MAX =  5.0;
const float ROOT_ROTATE_X_MIN    = -180.0;
const float ROOT_ROTATE_X_MAX    =  180.0;
const float ROOT_ROTATE_Y_MIN    = -180.0;
const float ROOT_ROTATE_Y_MAX    =  180.0;
const float ROOT_ROTATE_Z_MIN    = -180.0;
const float ROOT_ROTATE_Z_MAX    =  180.0;
const float HEAD_MIN             = -180.0;
const float HEAD_MAX             =  180.0;
const float SHOULDER_PITCH_MIN   = 	0.0;
const float SHOULDER_PITCH_MAX   =  140.0;
const float SHOULDER_YAW_MIN     = 	-45.0;
const float SHOULDER_YAW_MAX     =  45.0;
const float SHOULDER_ROLL_MIN    = -90.0;
const float SHOULDER_ROLL_MAX    =  90.0;
const float HIP_PITCH_MIN        = -45.0;
const float HIP_PITCH_MAX        =  45.0;
const float HIP_YAW_MIN          = -45.0;
const float HIP_YAW_MAX          =  45.0;
const float HIP_ROLL_MIN         = -45.0;
const float HIP_ROLL_MAX         =  45.0;
const float BEAK_MIN             =  0.0;
const float BEAK_MAX             =  1.0;
const float ELBOW_MIN            =  0.0;
const float ELBOW_MAX            = 75.0;
const float KNEE_MIN             =  -45.0;
const float KNEE_MAX             = 75.0;

const float LIGHT_ANGLE_MIN      = -180;
const float LIGHT_ANGLE_MAX      = 180;

//place here for now
//const float LIGHT_ANGLE=90.0;

//const variables for body parts
const float TRAP_F=1.3;

const float LAMP_SCALE_X=0.1;
const float LAMP_SCALE_Y=0.1;
const float LAMP_SCALE_Z=0.1;

const float LAMP_DISTANCE=5.5;

const float BODY_SCALE_X=1.0;
const float BODY_SCALE_Y=2.2;
const float BODY_SCALE_Z=0.75;

//head is relative to world
const float HEAD_SCALE_X=BODY_SCALE_X/TRAP_F;
const float HEAD_SCALE_Y=0.25*BODY_SCALE_Y;
const float HEAD_SCALE_Z=BODY_SCALE_Z;
const float HEAD_LOCATION=BODY_SCALE_Y*1.25;

//BEAK is relative to HEAD
const float BEAK_SCALE_X=0.65;
const float BEAK_SCALE_Y=0.15;
const float BEAK_SCALE_Z=BODY_SCALE_Z;
const float BEAK_TOP_X_LOCATION=-1.75;
const float BEAK_TOP_Y_LOCATION=0.4;

//BEAK BOT is relative to TOP
const float BEAK_BOT_X_LOCATION=0.0;
const float BEAK_BOT_Y_LOCATION=-3.1;

//ARM is relative to WORLD
const float ARM_SCALE_X=0.3*BODY_SCALE_X;
const float ARM_SCALE_Y=0.6*BODY_SCALE_Y;
const float ARM_SCALE_Z=0.4*BODY_SCALE_Z;

const float F_ARM_X_LOCATION=0.0;
const float F_ARM_Y_LOCATION=BODY_SCALE_Y*0.75;
const float F_ARM_Z_LOCATION=BODY_SCALE_Z*1.0;


const float B_ARM_X_LOCATION=0.0;
const float B_ARM_Y_LOCATION=BODY_SCALE_Y*0.75;
const float B_ARM_Z_LOCATION=BODY_SCALE_Z*-1.0;


//HAND is relative to the WORLD
const float HAND_SCALE_X=ARM_SCALE_X;
const float HAND_SCALE_Y=ARM_SCALE_Y*0.2;
const float HAND_SCALE_Z=ARM_SCALE_Z;

const float F_HAND_X_LOCATION=0.0;
const float F_HAND_Y_LOCATION=-2*(ARM_SCALE_Y);
const float F_HAND_Z_LOCATION=2*HAND_SCALE_Z;

const float B_HAND_X_LOCATION=0.0;
const float B_HAND_Y_LOCATION=-2*(ARM_SCALE_Y);
const float B_HAND_Z_LOCATION=-2*HAND_SCALE_Z;



//LEG is relative to the world
const float LEG_SCALE_X=0.3*BODY_SCALE_X;
const float LEG_SCALE_Y=0.25*BODY_SCALE_Y;
const float LEG_SCALE_Z=0.3*BODY_SCALE_Z;

const float F_LEG_X_LOCATION=0.0;
const float F_LEG_Y_LOCATION=-1*BODY_SCALE_Y;
const float F_LEG_Z_LOCATION=BODY_SCALE_Z*0.65;

const float B_LEG_X_LOCATION=0.0;
const float B_LEG_Y_LOCATION=-1*BODY_SCALE_Y;
const float B_LEG_Z_LOCATION=BODY_SCALE_Z*-0.65;


const float FOOT_SCALE_X=LEG_SCALE_X*1.2;
const float FOOT_SCALE_Y=LEG_SCALE_Y*0.1;
const float FOOT_SCALE_Z=LEG_SCALE_Z*2;

const float F_FOOT_X_LOCATION=0.0;
const float F_FOOT_Y_LOCATION=-2*(LEG_SCALE_Y);
const float F_FOOT_Z_LOCATION=F_LEG_Z_LOCATION-FOOT_SCALE_Z;

const float B_FOOT_X_LOCATION=0.0;
const float B_FOOT_Y_LOCATION=-2*(LEG_SCALE_Y);
const float B_FOOT_Z_LOCATION=B_LEG_Z_LOCATION+FOOT_SCALE_Z;


// ***********  FUNCTION HEADER DECLARATIONS ****************


// Initialization functions
void initDS();
void initGlut(int argc, char** argv);
void initGlui();
void initGl();


// Callbacks for handling events in glut
void reshape(int w, int h);
void animate();
void display(void);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);


// Functions to help draw the object
Vector getInterpolatedJointDOFS(float time);
void drawCube(int is_line=0);
void drawBody(int is_line=0);
void drawTrip(int is_line=0);
// Image functions
void writeFrame(char* filename, bool pgm, bool frontBuffer);


// ******************** FUNCTIONS ************************



// main() function
// Initializes the user interface (and any user variables)
// then hands over control to the event handler, which calls 
// display() whenever the GL window needs to be redrawn.
int main(int argc, char** argv)
{

    // Process program arguments
    if(argc != 3) {
        printf("Usage: demo [width] [height]\n");
        printf("Using 640x480 window by default...\n");
        Win[0] = 640;
        Win[1] = 480;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }


    // Initialize data structs, glut, glui, and opengl
	initDS();
    initGlut(argc, argv);
    initGlui();
    initGl();

    // Invoke the standard GLUT main event loop
    glutMainLoop();

    return 0;         // never reached
}


// Create / initialize global data structures
void initDS()
{
	keyframes = new Keyframe[KEYFRAME_MAX];
	for( int i = 0; i < KEYFRAME_MAX; i++ )
		keyframes[i].setID(i);

	animationTimer = new Timer();
	frameRateTimer = new Timer();
	joint_ui_data  = new Keyframe();
}


// Initialize glut and create a window with the specified caption 
void initGlut(int argc, char** argv)
{
	// Init GLUT
	glutInit(&argc, argv);

    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(argv[0]);

    // Setup callback functions to handle events
    glutReshapeFunc(reshape);	// Call reshape whenever window resized
    glutDisplayFunc(display);	// Call display whenever new frame needed
	glutMouseFunc(mouse);		// Call mouse whenever mouse button pressed
	glutMotionFunc(motion);		// Call motion whenever mouse moves while button pressed
}


// Load Keyframe button handler. Called when the "load keyframe" button is pressed
void loadKeyframeButton(int)
{
	// Get the keyframe ID from the UI
	int keyframeID = joint_ui_data->getID();

	// Update the 'joint_ui_data' variable with the appropriate
	// entry from the 'keyframes' array (the list of keyframes)
	*joint_ui_data = keyframes[keyframeID];

	// Sync the UI with the 'joint_ui_data' values
	glui_joints->sync_live();
	glui_keyframe->sync_live();

	// Let the user know the values have been loaded
	sprintf(msg, "Status: Keyframe %d loaded successfully", keyframeID);
	status->set_text(msg);
}

// Update Keyframe button handler. Called when the "update keyframe" button is pressed
void updateKeyframeButton(int)
{
	///////////////////////////////////////////////////////////
	// TODO:
	//   Modify this function to save the UI joint values into
	//   the appropriate keyframe entry in the keyframe list
	//   when the user clicks on the 'Update Keyframe' button.
	//   Refer to the 'loadKeyframeButton' function for help.
	///////////////////////////////////////////////////////////

	// Get the keyframe ID from the UI
	int keyframeID = joint_ui_data->getID();

	// Update the 'maxValidKeyframe' index variable
	// (it will be needed when doing the interpolation)
	if (keyframeID>maxValidKeyframe) {
		maxValidKeyframe=keyframeID;
	}
	

	// Update the appropriate entry in the 'keyframes' array
	// with the 'joint_ui_data' data
	keyframes[keyframeID].setTime(joint_ui_data->getTime());
	
	for( int j = 0; j < Keyframe::NUM_JOINT_ENUM; j++ )
		keyframes[keyframeID].setDOF(j, *joint_ui_data->getDOFPtr(j));

	// Let the user know the values have been updated
	sprintf(msg, "Status: Keyframe %d updated successfully", keyframeID);
	status->set_text(msg);
}

// Load Keyframes From File button handler. Called when the "load keyframes from file" button is pressed
//
// ASSUMES THAT THE FILE FORMAT IS CORRECT, ie, there is no error checking!
//
void loadKeyframesFromFileButton(int)
{
	// Open file for reading
	FILE* file = fopen(filenameKF, "r");
	if( file == NULL )
	{
		sprintf(msg, "Status: Failed to open file %s", filenameKF);
		status->set_text(msg);
		return;
	}

	// Read in maxValidKeyframe first
	fscanf(file, "%d", &maxValidKeyframe);

	// Now read in all keyframes in the format:
	//    id
	//    time
	//    DOFs
	//
	for( int i = 0; i <= maxValidKeyframe; i++ )
	{
		fscanf(file, "%d", keyframes[i].getIDPtr());
		fscanf(file, "%f", keyframes[i].getTimePtr());

		for( int j = 0; j < Keyframe::NUM_JOINT_ENUM; j++ )
			fscanf(file, "%f", keyframes[i].getDOFPtr(j));
	}

	// Close file
	fclose(file);

	// Let the user know the keyframes have been loaded
	sprintf(msg, "Status: Keyframes loaded successfully");
	status->set_text(msg);
}

// Save Keyframes To File button handler. Called when the "save keyframes to file" button is pressed
void saveKeyframesToFileButton(int)
{
	// Open file for writing
	FILE* file = fopen(filenameKF, "w");
	if( file == NULL )
	{
		sprintf(msg, "Status: Failed to open file %s", filenameKF);
		status->set_text(msg);
		return;
	}

	// Write out maxValidKeyframe first
	fprintf(file, "%d\n", maxValidKeyframe);
	fprintf(file, "\n");

	// Now write out all keyframes in the format:
	//    id
	//    time
	//    DOFs
	//
	for( int i = 0; i <= maxValidKeyframe; i++ )
	{
		fprintf(file, "%d\n", keyframes[i].getID());
		fprintf(file, "%f\n", keyframes[i].getTime());

		for( int j = 0; j < Keyframe::NUM_JOINT_ENUM; j++ )
			fprintf(file, "%f\n", keyframes[i].getDOF(j));

		fprintf(file, "\n");
	}

	// Close file
	fclose(file);

	// Let the user know the keyframes have been saved
	sprintf(msg, "Status: Keyframes saved successfully");
	status->set_text(msg);
}

// Animate button handler.  Called when the "animate" button is pressed.
void animateButton(int)
{
  // synchronize variables that GLUT uses
  glui_keyframe->sync_live();

  // toggle animation mode and set idle function appropriately
  if( animate_mode == 0 )
  {
	// start animation
	frameRateTimer->reset();
	animationTimer->reset();

	animate_mode = 1;
	GLUI_Master.set_glutIdleFunc(animate);

	// Let the user know the animation is running
	sprintf(msg, "Status: Animating...");
	status->set_text(msg);
  }
  else
  {
	// stop animation
	animate_mode = 0;
	GLUI_Master.set_glutIdleFunc(NULL);

	// Let the user know the animation has stopped
	sprintf(msg, "Status: Animation stopped");
	status->set_text(msg);
  }
}

// Render Frames To File button handler. Called when the "Render Frames To File" button is pressed.
void renderFramesToFileButton(int)
{
	// Calculate number of frames to generate based on dump frame rate
	int numFrames = int(keyframes[maxValidKeyframe].getTime() * DUMP_FRAME_PER_SEC) + 1;

	// Generate frames and save to file
	frameToFile = 1;
	for( frameNumber = 0; frameNumber < numFrames; frameNumber++ )
	{
		// Get the interpolated joint DOFs
		joint_ui_data->setDOFVector( getInterpolatedJointDOFS(frameNumber * DUMP_SEC_PER_FRAME) );

		// Let the user know which frame is being rendered
		sprintf(msg, "Status: Rendering frame %d...", frameNumber);
		status->set_text(msg);

		// Render the frame
		display();
	}
	frameToFile = 0;

	// Let the user know how many frames were generated
	sprintf(msg, "Status: %d frame(s) rendered to file", numFrames);
	status->set_text(msg);
}

// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Initialize GLUI and the user interface
void initGlui()
{
	GLUI_Panel* glui_panel;
	GLUI_Spinner* glui_spinner;
	GLUI_RadioGroup* glui_radio_group;

    GLUI_Master.set_glutIdleFunc(NULL);


	// Create GLUI window (joint controls) ***************
	//
	glui_joints = GLUI_Master.create_glui("Joint Control", 0, Win[0]+12, 0);

    // Create controls to specify root position and orientation
	glui_panel = glui_joints->add_panel("Root");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X));
	glui_spinner->set_float_limits(ROOT_TRANSLATE_X_MIN, ROOT_TRANSLATE_X_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Y));
	glui_spinner->set_float_limits(ROOT_TRANSLATE_Y_MIN, ROOT_TRANSLATE_Y_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "translate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z));
	glui_spinner->set_float_limits(ROOT_TRANSLATE_Z_MIN, ROOT_TRANSLATE_Z_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate x:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_X));
	glui_spinner->set_float_limits(ROOT_ROTATE_X_MIN, ROOT_ROTATE_X_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate y:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Y));
	glui_spinner->set_float_limits(ROOT_ROTATE_Y_MIN, ROOT_ROTATE_Y_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "rotate z:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Z));
	glui_spinner->set_float_limits(ROOT_ROTATE_Z_MIN, ROOT_ROTATE_Z_MAX, GLUI_LIMIT_WRAP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify head rotation
	glui_panel = glui_joints->add_panel("Head");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "head:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::HEAD));
	glui_spinner->set_float_limits(HEAD_MIN, HEAD_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify beak
	glui_panel = glui_joints->add_panel("Beak");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "beak:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::BEAK));
	glui_spinner->set_float_limits(BEAK_MIN, BEAK_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);


	glui_joints->add_column(false);


	// Create controls to specify right arm
	glui_panel = glui_joints->add_panel("Right arm");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_PITCH));
	glui_spinner->set_float_limits(SHOULDER_PITCH_MIN, SHOULDER_PITCH_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_YAW));
	glui_spinner->set_float_limits(SHOULDER_YAW_MIN, SHOULDER_YAW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_ROLL));
	glui_spinner->set_float_limits(SHOULDER_ROLL_MIN, SHOULDER_ROLL_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "elbow:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_ELBOW));
	glui_spinner->set_float_limits(ELBOW_MIN, ELBOW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify left arm
	glui_panel = glui_joints->add_panel("Left arm");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_SHOULDER_PITCH));
	glui_spinner->set_float_limits(SHOULDER_PITCH_MIN, SHOULDER_PITCH_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_SHOULDER_YAW));
	glui_spinner->set_float_limits(SHOULDER_YAW_MIN, SHOULDER_YAW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "shoulder roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_SHOULDER_ROLL));
	glui_spinner->set_float_limits(SHOULDER_ROLL_MIN, SHOULDER_ROLL_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "elbow:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_ELBOW));
	glui_spinner->set_float_limits(ELBOW_MIN, ELBOW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);


	glui_joints->add_column(false);


	// Create controls to specify right leg
	glui_panel = glui_joints->add_panel("Right leg");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_HIP_PITCH));
	glui_spinner->set_float_limits(HIP_PITCH_MIN, HIP_PITCH_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_HIP_YAW));
	glui_spinner->set_float_limits(HIP_YAW_MIN, HIP_YAW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_HIP_ROLL));
	glui_spinner->set_float_limits(HIP_ROLL_MIN, HIP_ROLL_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "knee:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_KNEE));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create controls to specify left leg
	glui_panel = glui_joints->add_panel("Left leg");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_HIP_PITCH));
	glui_spinner->set_float_limits(HIP_PITCH_MIN, HIP_PITCH_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_HIP_YAW));
	glui_spinner->set_float_limits(HIP_YAW_MIN, HIP_YAW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "hip roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_HIP_ROLL));
	glui_spinner->set_float_limits(HIP_ROLL_MIN, HIP_ROLL_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "knee:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_KNEE));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);


	glui_panel = glui_joints->add_panel("Light Controls");
	// Create a control to specify the time (for setting a keyframe)
	//glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_spinner = glui_keyframe->add_spinner_to_panel(glui_panel, "Light Angle:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE));
	glui_spinner->set_float_limits(LIGHT_ANGLE_MIN, LIGHT_ANGLE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	
	///////////////////////////////////////////////////////////
	// TODO (for controlling light source position & additional
	//      rendering styles):
	//   Add more UI spinner elements here. Be sure to also
	//   add the appropriate min/max range values to this
	//   file, and to also add the appropriate enums to the
	//   enumeration in the Keyframe class (keyframe.h).
	///////////////////////////////////////////////////////////

	//
	// ***************************************************


/*
	// Create GLUI window (light controls) ************
	//
	glui_keyframe = GLUI_Master.create_glui("Light Control", 0, 50, Win[1]+64);

	// Create a control to specify the time (for setting a keyframe)
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_spinner = glui_keyframe->add_spinner_to_panel(glui_panel, "Light:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE));
	glui_spinner->set_float_limits(LIGHT_ANGLE_MIN, LIGHT_ANGLE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

////////////END light controls
*/

	// Create GLUI window (keyframe controls) ************
	//
	glui_keyframe = GLUI_Master.create_glui("Keyframe Control", 0, 0, Win[1]+64);

	// Create a control to specify the time (for setting a keyframe)
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_spinner = glui_keyframe->add_spinner_to_panel(glui_panel, "Time:", GLUI_SPINNER_FLOAT, joint_ui_data->getTimePtr());
	glui_spinner->set_float_limits(TIME_MIN, TIME_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	// Create a control to specify a keyframe (for updating / loading a keyframe)
	glui_keyframe->add_column_to_panel(glui_panel, false);
	glui_spinner = glui_keyframe->add_spinner_to_panel(glui_panel, "Keyframe ID:", GLUI_SPINNER_INT, joint_ui_data->getIDPtr());
	glui_spinner->set_int_limits(KEYFRAME_MIN, KEYFRAME_MAX-1, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);

	glui_keyframe->add_separator();

	// Add buttons to load and update keyframes
	// Add buttons to load and save keyframes from a file
	// Add buttons to start / stop animation and to render frames to file
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_keyframe->add_button_to_panel(glui_panel, "Load Keyframe", 0, loadKeyframeButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Load Keyframes From File", 0, loadKeyframesFromFileButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Start / Stop Animation", 0, animateButton);
	glui_keyframe->add_column_to_panel(glui_panel, false);
	glui_keyframe->add_button_to_panel(glui_panel, "Update Keyframe", 0, updateKeyframeButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Save Keyframes To File", 0, saveKeyframesToFileButton);
	glui_keyframe->add_button_to_panel(glui_panel, "Render Frames To File", 0, renderFramesToFileButton);

	glui_keyframe->add_separator();

	// Add status line
	glui_panel = glui_keyframe->add_panel("");
	status = glui_keyframe->add_statictext_to_panel(glui_panel, "Status: Ready");

	// Add button to quit
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_keyframe->add_button_to_panel(glui_panel, "Quit", 0, quitButton);
	//
	// ***************************************************


	// Create GLUI window (render controls) ************
	//
	glui_render = GLUI_Master.create_glui("Render Control", 0, 367, Win[1]+64);

	// Create control to specify the render style
	glui_panel = glui_render->add_panel("Render Style");
	glui_radio_group = glui_render->add_radiogroup_to_panel(glui_panel, &renderStyle);
	glui_render->add_radiobutton_to_group(glui_radio_group, "Wireframe");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Solid");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Solid w/ outlines");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Metalllic");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Matte");
	//
	// ***************************************************


	// Tell GLUI windows which window is main graphics window
	glui_joints->set_main_gfx_window(windowID);
	glui_keyframe->set_main_gfx_window(windowID);
	glui_render->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
    
}


// Calculates the interpolated joint DOF vector
// using Catmull-Rom interpolation of the keyframes
Vector getInterpolatedJointDOFS(float time)
{
	// Need to find the keyframes bewteen which
	// the supplied time lies.
	// At the end of the loop we have:
	//    keyframes[i-1].getTime() < time <= keyframes[i].getTime()
	//
	int i = 0;
	while( i <= maxValidKeyframe && keyframes[i].getTime() < time )
		i++;

	// If time is before or at first defined keyframe, then
	// just use first keyframe pose
	if( i == 0 )
		return keyframes[0].getDOFVector();

	// If time is beyond last defined keyframe, then just
	// use last keyframe pose
	if( i > maxValidKeyframe )
		return keyframes[maxValidKeyframe].getDOFVector();

	// Need to normalize time to (0, 1]
	time = (time - keyframes[i-1].getTime()) / (keyframes[i].getTime() - keyframes[i-1].getTime());

	// Get appropriate data points and tangent vectors
	// for computing the interpolation
	Vector p0 = keyframes[i-1].getDOFVector();
	Vector p1 = keyframes[i].getDOFVector();

	Vector t0, t1;
	if( i == 1 )							// special case - at beginning of spline
	{
		t0 = keyframes[i].getDOFVector() - keyframes[i-1].getDOFVector();
		t1 = (keyframes[i+1].getDOFVector() - keyframes[i-1].getDOFVector()) * 0.5;
	}
	else if( i == maxValidKeyframe )		// special case - at end of spline
	{
		t0 = (keyframes[i].getDOFVector() - keyframes[i-2].getDOFVector()) * 0.5;
		t1 = keyframes[i].getDOFVector() - keyframes[i-1].getDOFVector();
	}
	else
	{
		t0 = (keyframes[i].getDOFVector()   - keyframes[i-2].getDOFVector()) * 0.5;
		t1 = (keyframes[i+1].getDOFVector() - keyframes[i-1].getDOFVector()) * 0.5;
	}

	// Return the interpolated Vector
	Vector a0 = p0;
	Vector a1 = t0;
	Vector a2 = p0 * (-3) + p1 * 3 + t0 * (-2) + t1 * (-1);
	Vector a3 = p0 * 2 + p1 * (-2) + t0 + t1;

	return (((a3 * time + a2) * time + a1) * time + a0);
}


// Callback idle function for animating the scene
void animate()
{
	// Only update if enough time has passed
	// (This locks the display to a certain frame rate rather
	//  than updating as fast as possible. The effect is that
	//  the animation should run at about the same rate
	//  whether being run on a fast machine or slow machine)
	if( frameRateTimer->elapsed() > SEC_PER_FRAME )
	{
		// Tell glut window to update itself. This will cause the display()
		// callback to be called, which renders the object (once you've written
		// the callback).
		glutSetWindow(windowID);
		glutPostRedisplay();

		// Restart the frame rate timer
		// for the next frame
		frameRateTimer->reset();
	}
}


// Handles the window being resized by updating the viewport
// and projection matrices
void reshape(int w, int h)
{
	// Update internal variables and OpenGL viewport
	Win[0] = w;
	Win[1] = h;
	glViewport(0, 0, (GLsizei)Win[0], (GLsizei)Win[1]);

    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(CAMERA_FOVY, (GLdouble)Win[0]/(GLdouble)Win[1], NEAR_CLIP, FAR_CLIP);
}



// display callback
//
// README: This gets called by the event handler
// to draw the scene, so this is where you need
// to build your scene -- make your changes and
// additions here. All rendering happens in this
// function. For Assignment 2, updates to the
// joint DOFs (joint_ui_data) happen in the
// animate() function.
void display(void)
{
    // Clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //added enables for z-buffer
    glClearDepth(1);
    glEnable(GL_DEPTH_TEST);
  
  
	//various material components


	//material properties
	//material properties, make it a chrome hue gold, according to:
	//http://devernay.free.fr/cours/opengl/materials.html
	GLfloat met_amb_mat_col[] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat met_diff_mat_col[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat met_spec_mat_col[] = {0.2f, 0.0f, 0.0f, 1.0f};
	GLfloat met_shine=0.07f;

	//material properties, make it a chrome hue gold, according to:
	//http://devernay.free.fr/cours/opengl/materials.html
	GLfloat mat_amb_mat_col[] = {0.0f, 0.0f, 1.0f, 1.0f};
	GLfloat mat_diff_mat_col[] = {0.0f, 0.0f, 0.5f, 1.0f};
	GLfloat mat_spec_mat_col[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat mat_shine=0.01f;
	
	//light
	GLfloat lightpos []= {cos(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), sin(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), 0, 0.0001};
	
	if (renderStyle==METALLIC) {
		//disable coloring due for metallic and matte appearence
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		

		//enable ligting
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		
		glMaterialfv(GL_FRONT, GL_AMBIENT, met_amb_mat_col); 
		glMaterialfv(GL_FRONT, GL_DIFFUSE, met_diff_mat_col); 
		glMaterialfv(GL_FRONT, GL_SPECULAR, met_spec_mat_col); 
		glMaterialf(GL_FRONT, GL_SHININESS, met_shine*128.0);
		
		//glLightfv(GL_LIGHT0, GL_SPECULAR, met_spec_mat_col);
		//lightpos= {cos(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), sin(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), 0, 0.0001};
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	}

	else if (renderStyle==MATTE) {
		//disable coloring due for metallic and matte appearence
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		

		//enable ligting
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb_mat_col); 
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff_mat_col); 
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec_mat_col); 
		glMaterialf(GL_FRONT, GL_SHININESS, mat_shine*128.0);
		
		//lightpos= {cos(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), sin(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), 0, 0.0001};
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	}

	else if (renderStyle==WIREFRAME) {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	//for solid and solid w/ outlines
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		
		//for solid with outlines, without line dropouts
		glPolygonOffset(0.5, 2);
	}
	

	// Specify camera transformation
	glTranslatef(camXPos, camYPos, camZPos);


	// Get the time for the current animation step, if necessary
	if( animate_mode )
	{
		float curTime = animationTimer->elapsed();

		if( curTime >= keyframes[maxValidKeyframe].getTime() )
		{
			// Restart the animation
			animationTimer->reset();
			curTime = animationTimer->elapsed();
		}

		///////////////////////////////////////////////////////////
		// README:
		//   This statement loads the interpolated joint DOF vector
		//   into the global 'joint_ui_data' variable. Use the
		//   'joint_ui_data' variable below in your model code to
		//   drive the model for animation.
		///////////////////////////////////////////////////////////
		// Get the interpolated joint DOFs
		joint_ui_data->setDOFVector( getInterpolatedJointDOFS(curTime) );

		// Update user interface
		joint_ui_data->setTime(curTime);
		glui_keyframe->sync_live();
	}


    ///////////////////////////////////////////////////////////
    // TODO:
	//   Modify this function to draw the scene.
	//   This should include function calls that apply
	//   the appropriate transformation matrices and render
	//   the individual body parts.
	//   Use the 'joint_ui_data' data structure to obtain
	//   the joint DOFs to specify your transformations.
	//   Sample code is provided below and demonstrates how
	//   to access the joint DOF values. This sample code
	//   should be replaced with your own.
	//   Use the 'renderStyle' variable and the associated
	//   enumeration to determine how the geometry should be
	//   rendered.
    ///////////////////////////////////////////////////////////

	// SAMPLE CODE **********
	//
	
	
	//draw the light source indicator
	glPushMatrix();
			
			glTranslatef((LAMP_DISTANCE*(cos(joint_ui_data->getDOF(Keyframe::LIGHT_ANGLE)*(PI/180)))),
					 (LAMP_DISTANCE*(sin(joint_ui_data->getDOF(Keyframe::LIGHT_ANGLE)*(PI/180)))),
					 0);
					 
			 //rotate for easier visibility
			glRotatef(0, 1.0, 1.0, 1.0);
					 
			glScalef(LAMP_SCALE_X,LAMP_SCALE_Y,LAMP_SCALE_Z);
			//draw the outline
			if (renderStyle==OUTLINED){
				glColor3f(0.0, 0.0, 0.0);
				//drawCube(POLY_OUT);
				
			}
			glColor3f(1.0, 1.0, 1.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			//drawCube();
			glDisable(GL_POLYGON_OFFSET_FILL);
			
	glPopMatrix();

		//draw the penguin body
	glPushMatrix();


		// setup transformation for body part
		glTranslatef(joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z));
					 
					 

		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_X),1.0,0.0,0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Y),0.0,1.0,0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Z),0.0,0.0,1.0);
		
		//rotate for easier visibility
		glRotatef(45, 0.0, 1.0, 0.0);
		
		//draw the penguin BODY////////////////////////
		glPushMatrix();
			
			//glRotatef(-60.0, 0.0, 1.0, 0.0);
			
			
			glScalef(BODY_SCALE_X,BODY_SCALE_Y,BODY_SCALE_Z);
			//glScalef(1.0,2.5,0.4);
		
			//draw the outline
			if (renderStyle==OUTLINED){
				glColor3f(0.0, 0.0, 0.0);
				drawBody(POLY_OUT);
				
			}
			glColor3f(1.0, 0.0, 0.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			drawBody();
			glDisable(GL_POLYGON_OFFSET_FILL);
		
		glPopMatrix();
		//END BODY////////////////////////////////////
		
		/////DRAW HEAD RELATIVE TO WORLD//////////////////////////
		glPushMatrix();
		
			glTranslatef(0.0, HEAD_LOCATION, 0.0);
			//glPushMatrix();
			glRotatef(joint_ui_data->getDOF(Keyframe::HEAD),0.0,1.0,0.0);
			//glPopMatrix();
			

			glScalef(HEAD_SCALE_X,HEAD_SCALE_Y,HEAD_SCALE_Z);
			//translate the joint of the head to neck
			//glTranslatef(0.0, -0.75, 0.0);
			
			//draw the outline
			if (renderStyle==OUTLINED){
				glColor3f(0.0, 0.0, 0.0);
				drawBody(POLY_OUT);
				
			}
			glColor3f(0.0, 1.0, 0.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			drawBody();
			glDisable(GL_POLYGON_OFFSET_FILL);
			
			
			//DRAW BEAK TOP RELATIVE TO HEAD
			glPushMatrix();
			
				glTranslatef(BEAK_TOP_X_LOCATION, BEAK_TOP_Y_LOCATION, 0.0);
				glScalef(BEAK_SCALE_X,BEAK_SCALE_Y,BEAK_SCALE_Z);
				
					//draw the outline
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				drawCube();
				glDisable(GL_POLYGON_OFFSET_FILL);
				
				//DRAW BEAK TOP RELATIVE TO BOTTOM
				glPushMatrix();
				
					glTranslatef(0.0, joint_ui_data->getDOF(Keyframe::BEAK) , 0.0);
					glTranslatef(BEAK_BOT_X_LOCATION, BEAK_BOT_Y_LOCATION, 0.0);
					//glScalef(BEAK_SCALE_X,BEAK_SCALE_Y,BEAK_SCALE_Z);
					
						//draw the outline
					if (renderStyle==OUTLINED){
						glColor3f(0.0, 0.0, 0.0);
						drawCube(POLY_OUT);
						
					}
					glColor3f(0.0, 0.0, 1.0);
					glEnable(GL_POLYGON_OFFSET_FILL);
					drawCube();
					glDisable(GL_POLYGON_OFFSET_FILL);
				glPopMatrix();
				//ENDBEAK BOT
			
			glPopMatrix();
			//ENDBEAK TOP
			
		glPopMatrix();
		//////////////////////////END HEAD
		
		/////////////////////////////////////////////////////////////LEFT/FRONT ARM
		//Draw FRONT ARMS and HAND relative to the world
		glPushMatrix();
		
			
			
				glTranslatef(F_ARM_X_LOCATION, F_ARM_Y_LOCATION, F_ARM_Z_LOCATION);
				
				glRotatef(joint_ui_data->getDOF(Keyframe::L_SHOULDER_PITCH)*-1,1.0,0.0,0.0);
				glRotatef(joint_ui_data->getDOF(Keyframe::L_SHOULDER_YAW),0.0,1.0,0.0);
				glRotatef(joint_ui_data->getDOF(Keyframe::L_SHOULDER_ROLL)*-1,0.0,0.0,1.0);
				
				//glTranslatef(0.0, joint_ui_data->getDOF(Keyframe::BEAK) , 0.0);
				
			//Draw FRONT ARMS 	
			glPushMatrix();
				
				glScalef(ARM_SCALE_X,-ARM_SCALE_Y,ARM_SCALE_Z);
				
				
				//translate the center of rotation
				glTranslatef(0.0, 1, 1);
				
				//draw the outline
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					drawBody(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				drawBody();
				glDisable(GL_POLYGON_OFFSET_FILL);
			
			glPopMatrix();
			//END HAND
			//Draw HANDS relative to world
			glPushMatrix();
			
				
			
			
				glTranslatef(F_HAND_X_LOCATION, F_HAND_Y_LOCATION, F_HAND_Z_LOCATION);
				
				glRotatef(joint_ui_data->getDOF(Keyframe::L_ELBOW)*-1,1.0,0.0,0.0);
			
				glScalef(HAND_SCALE_X,HAND_SCALE_Y, HAND_SCALE_Z);
				//draw the outline
				
				//translate the center of rotation of the arm
				glTranslatef(0.0, -1, -1);
				
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 1.0, 0.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				drawCube();
				glDisable(GL_POLYGON_OFFSET_FILL);
				
			glPopMatrix();
			//END HAND
			
			
			
			
		glPopMatrix();
		//END ARM FRONT + HAND
		/////////////////////////////////////////////////////////////LEFT/FRONT ARM END

		
		
		
		
		
		/////////////////////////////////////////////////////////////RIGHT/BACK ARM
		//Draw BACK ARMS and HAND relative to the world
		glPushMatrix();
		
			
			
			glTranslatef(B_ARM_X_LOCATION, B_ARM_Y_LOCATION, B_ARM_Z_LOCATION);
			
			glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH)*1,1.0,0.0,0.0);
			glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW)*-1,0.0,1.0,0.0);
			glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL)*-1,0.0,0.0,1.0);
			
			//glTranslatef(0.0, joint_ui_data->getDOF(Keyframe::BEAK) , 0.0);
				
			//Draw BACK ARMS 	
			glPushMatrix();
				
				glScalef(ARM_SCALE_X,-ARM_SCALE_Y,ARM_SCALE_Z);
				
				
				//translate the center of rotation
				glTranslatef(0.0, 1, -1);
				
				//draw the outline
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					drawBody(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				drawBody();
				glDisable(GL_POLYGON_OFFSET_FILL);
			
			glPopMatrix();
			//END HAND
			//Draw HANDS relative to world
			glPushMatrix();
			
				
			
			
				glTranslatef(B_HAND_X_LOCATION, B_HAND_Y_LOCATION, B_HAND_Z_LOCATION);
				
				glRotatef(joint_ui_data->getDOF(Keyframe::R_ELBOW),1.0,0.0,0.0);
			
				glScalef(HAND_SCALE_X,HAND_SCALE_Y, HAND_SCALE_Z);
				//draw the outline
				
				//translate the center of rotation of the arm
				glTranslatef(0.0, -1, 1);
				
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 1.0, 0.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				drawCube();
				glDisable(GL_POLYGON_OFFSET_FILL);
				
			glPopMatrix();
			//END HAND
			
			
			
			
		glPopMatrix();
		//END ARM BACK + HAND
		/////////////////////////////////////////////////////////////RIGHT/BACK ARM END
		
		
		//START FRONT LEG+FOOT////////////////////
		
		glPushMatrix();
		
		
			glTranslatef(F_LEG_X_LOCATION, F_LEG_Y_LOCATION, F_LEG_Z_LOCATION);
			
			glRotatef(joint_ui_data->getDOF(Keyframe::L_HIP_PITCH)*-1,1.0,0.0,0.0);
			glRotatef(joint_ui_data->getDOF(Keyframe::L_HIP_YAW),0.0,1.0,0.0);
			glRotatef(joint_ui_data->getDOF(Keyframe::L_HIP_ROLL)*-1,0.0,0.0,1.0);
		
			//Draw front leg	
			glPushMatrix();
				
				glScalef(LEG_SCALE_X,LEG_SCALE_Y,LEG_SCALE_Z);
				
				
				//translate the center of rotation
				glTranslatef(1.0, -1.0, 0.0);
				
				//draw the outline
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				drawCube();
				glDisable(GL_POLYGON_OFFSET_FILL);
			
			glPopMatrix();
			//END FRONT LEG////
			
			
			//START FRONT FEET
			glPushMatrix();
			
			
				glTranslatef(F_FOOT_X_LOCATION, F_FOOT_Y_LOCATION, F_FOOT_Z_LOCATION);
				
				glRotatef(joint_ui_data->getDOF(Keyframe::L_KNEE)*-1,0.0,0.0,1.0);
			
				glScalef(FOOT_SCALE_X,FOOT_SCALE_Y, FOOT_SCALE_Z);
				//draw the outline
				
				//translate the center of rotation of the feet
				
				
				//set the correct oriantation for the feet
				glRotatef(90,1.0,0.0,0.0);
				glRotatef(-90,0.0,0.0,1.0);
				
				glTranslatef(0.0, -1.0, 1.0);
				
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					drawTrip(POLY_OUT);
					
				}
				glColor3f(0.0, 1.0, 0.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				drawTrip();
				glDisable(GL_POLYGON_OFFSET_FILL);
			
			//START FRONT FOOT
			
			glPopMatrix();
			
			
		glPopMatrix();
			
		//END FRONT LEG+FOOT
		
		
		
		
		
		//START BACK LEG+FOOT////////////////////
		
		glPushMatrix();
		
		
			glTranslatef(B_LEG_X_LOCATION, B_LEG_Y_LOCATION, B_LEG_Z_LOCATION);
			
			glRotatef(joint_ui_data->getDOF(Keyframe::R_HIP_PITCH),1.0,0.0,0.0);
			glRotatef(joint_ui_data->getDOF(Keyframe::R_HIP_YAW)*-1,0.0,1.0,0.0);
			glRotatef(joint_ui_data->getDOF(Keyframe::R_HIP_ROLL)*-1,0.0,0.0,1.0);
		
			//Draw back leg	
			glPushMatrix();
				
				glScalef(LEG_SCALE_X,LEG_SCALE_Y,LEG_SCALE_Z);
				
				
				//translate the center of rotation
				glTranslatef(1.0, -1.0, 0.0);
				
				//draw the outline
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				drawCube();
				glDisable(GL_POLYGON_OFFSET_FILL);
			
			glPopMatrix();
			//END BACK LEG////
			
			
			//START BACK FEET
			glPushMatrix();
			
			
				glTranslatef(B_FOOT_X_LOCATION, B_FOOT_Y_LOCATION, B_FOOT_Z_LOCATION);
				
				glRotatef(joint_ui_data->getDOF(Keyframe::R_KNEE)*-1,0.0,0.0,1.0);
			
				glScalef(FOOT_SCALE_X,FOOT_SCALE_Y, FOOT_SCALE_Z);
				//draw the outline
				
				//translate the center of rotation of the feet
				
				
				//set the correct oriantation for the feet
				glRotatef(90,1.0,0.0,0.0);
				glRotatef(-90,0.0,0.0,1.0);
				
				glTranslatef(0.0, -1.0, 1.0);
				
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					drawTrip(POLY_OUT);
					
				}
				glColor3f(0.0, 1.0, 0.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				drawTrip();
				glDisable(GL_POLYGON_OFFSET_FILL);
			
			//END BACK FOOT
			
			glPopMatrix();
			
			
		glPopMatrix();
			
		//END BACK LEG+FOOT
		
		
		
		
		
	glPopMatrix();
	//END ROOT////////////

		

/*
	
	glPushMatrix();

		// setup transformation for body part
		glTranslatef(joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z));
		
		
		

		
		glRotatef(60.0, 0.0, 1.0, 0.0);
		//glRotatef(30.0, 1.0, 0.0, 0.0);
		glScalef(1.0,3.0,1.0);
		
		//draw the outline
		if (renderStyle==OUTLINED){
			glColor3f(0.0, 0.0, 0.0);
			drawCube(1);	
		}
		
		glColor3f(1.0, 1.0, 1.0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		// draw body part
		drawCube();
		glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();

	glPushMatrix();
		glRotatef(-60.0, 0.0, 1.0, 0.0);
		glRotatef(60.0, 1.0, 0.0, 0.0);
		glScalef(0.50,0.5,1.0);
		
		//draw the outline
		if (renderStyle==OUTLINED){
			glColor3f(0.0, 0.0, 0.0);
			drawCube(POLY_OUT);
			
		}
		glEnable(GL_POLYGON_OFFSET_FILL);
		glColor3f(1.0, 0.0, 0.0);
		drawCube();
		glDisable(GL_POLYGON_OFFSET_FILL);

		
	glPopMatrix();
	
*/
	

	
	
	//
	// SAMPLE CODE **********

    // Execute any GL functions that are in the queue just to be safe
    glFlush();

	// Dump frame to file, if requested
	if( frameToFile )
	{
		sprintf(filenameF, "frame%03d.ppm", frameNumber);
		writeFrame(filenameF, false, false);
	}

    // Now, show the frame buffer that we just drew into.
    // (this prevents flickering).
    glutSwapBuffers();
}


// Handles mouse button pressed / released events
void mouse(int button, int state, int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if( button == GLUT_RIGHT_BUTTON )
	{
		if( state == GLUT_DOWN )
		{
			lastX = x;
			lastY = y;
			updateCamZPos = true;
		}
		else
		{
			updateCamZPos = false;
		}
	}
	
	// If the LMB is pressed and dragged then rotate
	if( button == GLUT_LEFT_BUTTON )
	{
		if( state == GLUT_DOWN )
		{
			lastX = x;
			lastY = y;
			updateAngle = true;
		}
		else
		{
			updateAngle = false;
		}
	}
	

}


// Handles mouse motion events while a button is pressed
void motion(int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if( updateCamZPos )
	{
		// Update camera z position
		camZPos += (x - lastX) * ZOOM_SCALE;
		lastX = x;

		//add control for light location
		*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE) += (y - lastY);
		lastY = y;

		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
	// If the RMB is pressed and dragged then zoom in / out
	if( updateAngle )
	{
		// Update camera z position
		*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Y) += (x - lastX);
		lastX = x;
		
		*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_X) += (y - lastY);
		lastY = y;

		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
}






//Draw Body parts
void drawBody(int is_line)
{
	//int line? checks whether to draw line_loop vs gl_duad

	if (is_line==POLY_OUT){
		glBegin(GL_LINE_LOOP);
		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	else {
		glBegin(GL_QUADS);
	}
	
	
			//TODO: fix extra diagonal on the backface with line loop
		
			//specify normal for lighting
			glNormal3f(0,0,1);
			// draw front face
			glVertex3f(-1.0*TRAP_F, -1.0, 1.0);
			glVertex3f( 1.0*TRAP_F, -1.0, 1.0);
			glVertex3f( 1.0,  1.0, 1.0);
			glVertex3f(-1.0,  1.0, 1.0);

			//specify normal for lighting
			glNormal3f(0,0,-1);
			// draw back face
			glVertex3f( 1.0*TRAP_F, -1.0, -1.0);
			glVertex3f(-1.0*TRAP_F, -1.0, -1.0);
			glVertex3f(-1.0,  1.0, -1.0);
			glVertex3f( 1.0,  1.0, -1.0);



			//cross product calculated by hand
			glNormal3f(-8.0/(sqrt(8*8+16*(TRAP_F-1.0)*(TRAP_F-1.0))),(4.0*(TRAP_F-1.0))/(sqrt(8*8+16*(TRAP_F-1)*(TRAP_F-1))),0.0);
			// draw left face
			glVertex3f(-1.0*TRAP_F, -1.0, -1.0);
			glVertex3f(-1.0*TRAP_F, -1.0,  1.0);
			glVertex3f(-1.0,  1.0,  1.0);
			glVertex3f(-1.0,  1.0, -1.0);


			//cross product calculated by hand
			glNormal3f(8.0/(sqrt(8*8+16*(TRAP_F-1.0)*(TRAP_F-1.0))),(4.0*(TRAP_F-1.0))/(sqrt(8*8+16*(TRAP_F-1)*(TRAP_F-1))),0.0);
			// draw right face
			
			glVertex3f( 1.0*TRAP_F, -1.0,  1.0);
			glVertex3f( 1.0*TRAP_F, -1.0, -1.0);
			glVertex3f( 1.0,  1.0, -1.0);
			glVertex3f( 1.0,  1.0,  1.0);


			//specify normal for lighting
			glNormal3f(0,1,0);
			// draw top
			glVertex3f(-1.0,  1.0,  1.0);
			glVertex3f( 1.0,  1.0,  1.0);
			glVertex3f( 1.0,  1.0, -1.0);
			glVertex3f(-1.0,  1.0, -1.0);


			//specify normal for lighting
			glNormal3f(0,-1,0);
			// draw bottom
			glVertex3f(-1.0*TRAP_F, -1.0, -1.0);
			glVertex3f( 1.0*TRAP_F, -1.0, -1.0);
			glVertex3f( 1.0*TRAP_F, -1.0,  1.0);
			glVertex3f(-1.0*TRAP_F, -1.0,  1.0);
		glEnd();
}



// Draw a unit cube, centered at the current location
// README: Helper code for drawing a cube
void drawCube(int is_line)
{
	//int line? checks whether to draw line_loop vs gl_duad


	if (is_line==POLY_OUT){
		glBegin(GL_LINE_LOOP);
		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	else {
		glBegin(GL_QUADS);
	}
	

			//specify normal for lighting
			glNormal3f(0,0,1);
			// draw front face
			glVertex3f(-1.0, -1.0, 1.0);
			glVertex3f( 1.0, -1.0, 1.0);
			glVertex3f( 1.0,  1.0, 1.0);
			glVertex3f(-1.0,  1.0, 1.0);


			//specify normal for lighting
			glNormal3f(0,0,-1);
			// draw back face
			glVertex3f( 1.0, -1.0, -1.0);
			glVertex3f(-1.0, -1.0, -1.0);
			glVertex3f(-1.0,  1.0, -1.0);
			glVertex3f( 1.0,  1.0, -1.0);


			//specify normal for lighting
			glNormal3f(-1,0,0);
			// draw left face
			glVertex3f(-1.0, -1.0, -1.0);
			glVertex3f(-1.0, -1.0,  1.0);
			glVertex3f(-1.0,  1.0,  1.0);
			glVertex3f(-1.0,  1.0, -1.0);


			//specify normal for lighting
			glNormal3f(1,0,0);
			// draw right face
			glVertex3f( 1.0, -1.0,  1.0);
			glVertex3f( 1.0, -1.0, -1.0);
			glVertex3f( 1.0,  1.0, -1.0);
			glVertex3f( 1.0,  1.0,  1.0);



			//specify normal for lighting
			glNormal3f(0,1,0);
			// draw top
			glVertex3f(-1.0,  1.0,  1.0);
			glVertex3f( 1.0,  1.0,  1.0);
			glVertex3f( 1.0,  1.0, -1.0);
			glVertex3f(-1.0,  1.0, -1.0);


			//specify normal for lighting
			glNormal3f(0,-1,0);
			// draw bottom
			glVertex3f(-1.0, -1.0, -1.0);
			glVertex3f( 1.0, -1.0, -1.0);
			glVertex3f( 1.0, -1.0,  1.0);
			glVertex3f(-1.0, -1.0,  1.0);
		glEnd();
}



// Draw a unit triangular prism for the foot
// README: Helper code for drawing a cube
void drawTrip(int is_line)
{
	//int line? checks whether to draw line_loop vs gl_duad

	if (is_line==POLY_OUT){
		glBegin(GL_LINE_LOOP);
		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	else {
		glBegin(GL_TRIANGLES);
	}
	
		
			//specify normal for lighting
			glNormal3f(0,0,1);
			// draw front face
			glVertex3f(-1.0, -1.0, 1.0);
			glVertex3f( 1.0, -1.0, 1.0);
			glVertex3f( 0.0,  1.0, 1.0);
			//glVertex3f(-1.0,  1.0, 1.0);

			//specify normal for lighting
			glNormal3f(0,0,-1);
			// draw back face
			glVertex3f( -1.0, -1.0, -1.0);
			glVertex3f(1.0, -1.0, -1.0);
			glVertex3f(0.0,  1.0, -1.0);
			//glVertex3f( 1.0,  1.0, -1.0);
			glEnd();
			
			if (is_line==POLY_OUT){
				glBegin(GL_LINE_LOOP);
				//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			}
			else {
				glBegin(GL_QUADS);
			}
			
			//calculated normal by hand
			glNormal3f(-8/(sqrt(80)),4/(sqrt(80)),0);
			// draw left face
			glVertex3f(-1.0, -1.0, -1.0);
			glVertex3f(-1.0, -1.0,  1.0);
			glVertex3f(0.0,  1.0,  1.0);
			glVertex3f(0.0,  1.0, -1.0);


			//calculated normal by hand
			glNormal3f(8/(sqrt(80)),4/(sqrt(80)),0);
			// draw right face
			glVertex3f( 1.0, -1.0,  1.0);
			glVertex3f( 1.0, -1.0, -1.0);
			glVertex3f( 0.0,  1.0, -1.0);
			glVertex3f( 0.0,  1.0,  1.0);



			//specify normal for lighting
			glNormal3f(0,-1,0);
			//draw bottom
			glVertex3f(-1.0, -1.0, -1.0);
			glVertex3f( 1.0, -1.0, -1.0);
			glVertex3f( 1.0, -1.0,  1.0);
			glVertex3f(-1.0, -1.0,  1.0);
		glEnd();
}



///////////////////////////////////////////////////////////
//
// BELOW ARE FUNCTIONS FOR GENERATING IMAGE FILES (PPM/PGM)
//
///////////////////////////////////////////////////////////

void writePGM(char* filename, GLubyte* buffer, int width, int height, bool raw=true)
{
	FILE* fp = fopen(filename,"wt");

	if( fp == NULL )
	{
		printf("WARNING: Can't open output file %s\n",filename);
		return;
	}

	if( raw )
	{
		fprintf(fp,"P5\n%d %d\n%d\n",width,height,255);
		for(int y=height-1;y>=0;y--)
		{
			fwrite(&buffer[y*width],sizeof(GLubyte),width,fp);
			/*
			for(int x=0;x<width;x++)
			{
				fprintf(fp,"%c",int(buffer[x+y*width];
			}
			*/
		}
	}
	else
	{
		fprintf(fp,"P2\n%d %d\n%d\n",width,height,255);
		for(int y=height-1;y>=0;y--)
		{
			for(int x=0;x<width;x++)
			{
				fprintf(fp,"%d ",intptr_t(&buffer[x+y*width]));
			}
			fprintf(fp,"\n");
		}
	}

	fclose(fp);
}

#define RED_OFFSET   0
#define GREEN_OFFSET 1
#define BLUE_OFFSET  2

void writePPM(char* filename, GLubyte* buffer, int width, int height, bool raw=true)
{
	FILE* fp = fopen(filename,"wt");

	if( fp == NULL )
	{
		printf("WARNING: Can't open output file %s\n",filename);
		return;
	}

	if( raw )
	{
		fprintf(fp,"P6\n%d %d\n%d\n",width,height,255);
		for(int y=height-1;y>=0;y--)
		{
			for(int x=0;x<width;x++)
			{
				GLubyte* pix = &buffer[4*(x+y*width)];

				fprintf(fp,"%c%c%c",int(pix[RED_OFFSET]),
									int(pix[GREEN_OFFSET]),
									int(pix[BLUE_OFFSET]));
			}
		}
	}
	else
	{
		fprintf(fp,"P3\n%d %d\n%d\n",width,height,255);
		for(int y=height-1;y>=0;y--)
		{
			for(int x=0;x<width;x++)
			{
				GLubyte* pix = &buffer[4*(x+y*width)];

				fprintf(fp,"%d %d %d ",int(pix[RED_OFFSET]),
									   int(pix[GREEN_OFFSET]),
									   int(pix[BLUE_OFFSET]));
			}
			fprintf(fp,"\n");
		}
	}

	fclose(fp);
}

void writeFrame(char* filename, bool pgm, bool frontBuffer)
{
	static GLubyte* frameData = NULL;
	static int currentSize = -1;

	int size = (pgm ? 1 : 4);

	if( frameData == NULL || currentSize != size*Win[0]*Win[1] )
	{
		if (frameData != NULL)
			delete [] frameData;

		currentSize = size*Win[0]*Win[1];

		frameData = new GLubyte[currentSize];
	}

	glReadBuffer(frontBuffer ? GL_FRONT : GL_BACK);

	if( pgm )
	{
		glReadPixels(0, 0, Win[0], Win[1],
					 GL_LUMINANCE, GL_UNSIGNED_BYTE, frameData);
		writePGM(filename, frameData, Win[0], Win[1]);
	}
	else
	{
		glReadPixels(0, 0, Win[0], Win[1],
					 GL_RGBA, GL_UNSIGNED_BYTE, frameData);
		writePPM(filename, frameData, Win[0], Win[1]);
	}
}
