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
#include "SOIL.h"

//for libpng
//#include <png.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//for multitexturing
//#include <GL/glaux.h>
#include <glext.h>


// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

const float SPINNER_SPEED = 0.1;


//-----------------TEXTURE VARIABLES-------------------//
//texture loader
GLuint raw_texture_load(const char *filename, int width, int height);
static GLuint texture;

float g_fContributionOfTex0 = 0.33f;
float g_fContributionOfTex1 = 0.33f;
float g_fContributionOfTex2 = 0.33f;

//--------------------------------------------------------

//-----------------HEIGH MAPPING VARIABLES-------------------//
//height mapping from nehe
//http://nehe.gamedev.net/tutorial/beautiful_landscapes_by_means_of_height_mapping/16006/
#define		MAP_SIZE	  1024							/* Size Of Our .RAW Height Map */
#define		STEP_SIZE	  16					/* Width And Height Of Each Quad */
#define		HEIGHT_RATIO  1.5f							/* Ratio That The Y Is Scaled According To The X And Z */


void LoadRawFile(const char* strName, int nSize,unsigned char *pHeightMap);
unsigned char g_HeightMap[MAP_SIZE*MAP_SIZE];			/* Holds The Height Map Data */

float ter_scaleValue = 0.15f;								/* Scale Value For The Terrain */


//--------------------------------------------------------


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
const float ZOOM_SCALE = 0.8;


//updated to see terrain
GLdouble camXPos =  0.0;
GLdouble camYPos =  -50.0;
GLdouble camZPos = -300.0;

const GLdouble CAMERA_FOVY = 60.0;
const GLdouble NEAR_CLIP   = 0.1;
const GLdouble FAR_CLIP    = 1000.0;

// Render settings
enum { WIREFRAME, SOLID, OUTLINED, METALLIC, MATTE, TEXTURED };	// README: the different render styles
enum { POLY_DEF, POLY_OUT };			// for the outlined render style while while drawing polygons
int renderStyle = SOLID;			// README: the selected render style

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

const float TER_TRAN_X=0;
const float TER_TRAN_Y=0;
const float TER_TRAN_Z=-25;



const float BOX_SCALE_X=25;
const float BOX_SCALE_Y=25;
const float BOX_SCALE_Z=25;

const float BOX_DISTANCE_X=3.5;
const float BOX_DISTANCE_Y=102.5;
const float BOX_DISTANCE_Z=100;

const float LAMP_SCALE_X=12;
const float LAMP_SCALE_Y=12;
const float LAMP_SCALE_Z=12;

const float LAMP_DISTANCE=200.0;

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

//a3b constants
const float SPHERE_SCALE=2.0;


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
void drawTextCube();
void drawBody(int is_line=0);
void drawTrip(int is_line=0);
void drawSphere(int is_line=0);
void drawHeightMappedPlane(unsigned char pHeightMap[]);
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

	//load terrain map
	LoadRawFile("terrain_data/Terrain.raw", MAP_SIZE * MAP_SIZE, g_HeightMap);

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
	glui_render->add_radiobutton_to_group(glui_radio_group, "Textured");
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
    
    //specify shader model
    glShadeModel(GL_SMOOTH);
    
    
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



//height mapping helper functions
//from: http://nehe.gamedev.net/tutorial/beautiful_landscapes_by_means_of_height_mapping/16006/
//Loads The .RAW File And Stores It In pHeightMap
void LoadRawFile(const char* strName, int nSize,unsigned char *pHeightMap)
{
	int result;
	FILE *pFile = NULL;

	/* Open The File In Read / Binary Mode. */
	pFile = fopen( strName, "rb" );

	/* Check To See If We Found The File And Could Open It */
	if ( pFile == NULL )
	{
		/* Display Error Message And Stop The Function */
		printf("Error can't Find The Height Map!\n");
		return;
	}

	fread( pHeightMap, 1, nSize, pFile );

	/* After We Read The Data, It's A Good Idea To Check If Everything Read Fine */
	result = ferror( pFile );

	/* Check If We Received An Error */
	if (result)
	{
		printf("Error failed To Get Data!\n");
	}

	/* Close The File. */
	fclose(pFile);
}

int Height(unsigned char *pHeightMap, int X, int Y)		/* This Returns The Height From A Height Map Index */
{
	int x = X % MAP_SIZE;								/* Error Check Our x Value */
	int y = Y % MAP_SIZE;								/* Error Check Our y Value */

	if(!pHeightMap) return 0;							/* Make Sure Our Data Is Valid */
	
	
	//heightmpa stores values ranging from 0 to 256
	return pHeightMap[x + (y * MAP_SIZE)];				/* Index Into Our Height Array And Return The Height */
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
																						
 GLuint raw_texture_load(const char *filename, int width, int height)
 {
     GLuint texture;
     unsigned char *data;
     FILE *file;
 
     // open texture data
     file = fopen(filename, "rb");
     if (file == NULL) return 0;
 
     // allocate buffer
     data = (unsigned char*) malloc(width * height * 4);
 
     // read texture data
     fread(data, width * height * 4, 1, file);
     fclose(file);
 
     // allocate a texture name
     glGenTextures(1, &texture);
 
     // select our current texture
     glBindTexture(GL_TEXTURE_2D, texture);
 
     // select modulate to mix texture with color for shading
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
 
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);
 
     // when texture area is small, bilinear filter the closest mipmap
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
     // when texture area is large, bilinear filter the first mipmap
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
     // texture should tile
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
 
     // build our texture mipmaps
     gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
 
     // free buffer
     free(data);
 
     return texture;
     
     
     
     
 }
 


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


	GLfloat global_amb[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
	
	GLfloat lt_amb_col[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lt_diff_col[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lt_spec_col[] = {1.0f, 1.0f, 1.0f, 1.0f};
	

	//material properties
	//material properties, make it a chrome hue gold, according to:
	//http://devernay.free.fr/cours/opengl/materials.html
	GLfloat met_amb_mat_col[] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat met_diff_mat_col[] = {0.5f, 0.0f, 0.0f, 1.0f};
	GLfloat met_spec_mat_col[] = {0.02f, 0.0f, 0.0f, 1.0f};
	GLfloat met_shine=0.05f;

	//material properties, make it a chrome hue gold, according to:
	//http://devernay.free.fr/cours/opengl/materials.html
	GLfloat mat_amb_mat_col[] = {0.0f, 0.0f, 1.0f, 1.0f};
	GLfloat mat_diff_mat_col[] = {0.0f, 0.0f, 0.5f, 1.0f};
	GLfloat mat_spec_mat_col[] = {0.0f, 0.0f, 0.02f, 1.0f};
	GLfloat mat_shine=0.01f;
	
	//light
	GLfloat lightpos []= {LAMP_DISTANCE*cos(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), LAMP_DISTANCE*sin(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), 0, 1};
	
	
	if (renderStyle==TEXTURED) {
		texture = raw_texture_load("texture/grass.raw", 256, 256);
		glEnable(GL_TEXTURE_2D);
		
		//disable coloring due for metallic and matte appearence
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		
		glLightfv(GL_FRONT, GL_AMBIENT, lt_amb_col);  
		glLightfv(GL_FRONT, GL_DIFFUSE, lt_diff_col); 
		glLightfv(GL_FRONT, GL_SPECULAR, lt_spec_col);

		
		glMaterialfv(GL_FRONT, GL_AMBIENT, met_amb_mat_col); 
		glMaterialfv(GL_FRONT, GL_DIFFUSE, met_diff_mat_col); 
		glMaterialfv(GL_FRONT, GL_SPECULAR, met_spec_mat_col); 
		glMaterialf(GL_FRONT, GL_SHININESS, met_shine*128.0);
		
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_amb);

		
		//glLightfv(GL_LIGHT0, GL_SPECULAR, met_spec_mat_col);
		//lightpos= {cos(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), sin(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), 0, 0.0001};
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	}
	
	else if (renderStyle==METALLIC) {
		glDisable(GL_TEXTURE_2D);
		//disable coloring due for metallic and matte appearence
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		

		//enable ligting
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		
		glLightfv(GL_FRONT, GL_AMBIENT, lt_amb_col);  
		glLightfv(GL_FRONT, GL_DIFFUSE, lt_diff_col); 
		glLightfv(GL_FRONT, GL_SPECULAR, lt_spec_col);

		
		glMaterialfv(GL_FRONT, GL_AMBIENT, met_amb_mat_col); 
		glMaterialfv(GL_FRONT, GL_DIFFUSE, met_diff_mat_col); 
		glMaterialfv(GL_FRONT, GL_SPECULAR, met_spec_mat_col); 
		glMaterialf(GL_FRONT, GL_SHININESS, met_shine*128.0);
		
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_amb);

		
		//glLightfv(GL_LIGHT0, GL_SPECULAR, met_spec_mat_col);
		//lightpos= {cos(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), sin(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), 0, 0.0001};
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	}

	else if (renderStyle==MATTE) {
		glDisable(GL_TEXTURE_2D);
		//disable coloring due for metallic and matte appearence
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

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
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	//for solid and solid w/ outlines
	//use for default renders
	else {
		//glDisable(GL_TEXTURE_2D);
		//glDisable(GL_LIGHTING);
		//glDisable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		//enable ligting
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glLightfv(GL_FRONT, GL_AMBIENT, lt_amb_col);  
		glLightfv(GL_FRONT, GL_DIFFUSE, lt_diff_col); 
		glLightfv(GL_FRONT, GL_SPECULAR, lt_spec_col);


		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb_mat_col); 
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff_mat_col); 
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec_mat_col); 
		glMaterialf(GL_FRONT, GL_SHININESS, mat_shine*128.0);
		
		//lightpos= {cos(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), sin(*joint_ui_data->getDOFPtr(Keyframe::LIGHT_ANGLE)*(PI/180)), 0, 0.0001};
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
		
		//for solid with outlines, without line dropouts
		//glPolygonOffset(0.5, 2);
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
	
	
	//a3 code starts here//////////////
	
	//height mapped plane test
	glPushMatrix();
		//from NEHE
		// setup transformation for body part
		
		glTranslatef(joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z));
					 
					 

		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_X),1.0,0.0,0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Y),0.0,1.0,0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Z),0.0,0.0,1.0);
		
		//move the plane for easier visibility
		
		glTranslatef(TER_TRAN_X, TER_TRAN_Y, TER_TRAN_Z);
		//glRotatef(166.0, 1.0, 0.0, 0.0);
		glRotatef(-45.0, 0.0, 1.0, 0.0);
		
		glScalef(ter_scaleValue, ter_scaleValue * HEIGHT_RATIO, ter_scaleValue);
		drawHeightMappedPlane(g_HeightMap);
		//drawTextCube();
		
		
		
	
	glPopMatrix();
	
	
	
	glPushMatrix();
		
		// setup transformation for body part
		glTranslatef(joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z));
					 
					 

		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_X),1.0,0.0,0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Y),0.0,1.0,0.0);
		glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Z),0.0,0.0,1.0);
		
		
		//draw a textured cube for practice
		glPushMatrix();
			
			glTranslatef(BOX_DISTANCE_X, BOX_DISTANCE_Y, BOX_DISTANCE_Z);
					 
			 //rotate for easier visibility
			glRotatef(-166.0, 1.0, 0.0, 0.0);
					 
			glScalef(BOX_SCALE_X,BOX_SCALE_Y,BOX_SCALE_Z);
			glColor3f(1.0, 0.0, 0.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			drawCube();
			glDisable(GL_TEXTURE_2D);
			drawSphere(POLY_OUT);
			glDisable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_TEXTURE_2D);
			
		glPopMatrix();
		
		
		
		
		//draw a sphere, use the light control  to move it around initially
		glPushMatrix();
		
			//sphere manual
			//https://www.opengl.org/resources/libraries/glut/spec3/node81.html
			//glTranslatef((LAMP_DISTANCE*(cos(joint_ui_data->getDOF(Keyframe::LIGHT_ANGLE)*(PI/180)))),
			//		 (LAMP_DISTANCE*(sin(joint_ui_data->getDOF(Keyframe::LIGHT_ANGLE)*(PI/180)))),
			//		 0);
					 
			 //rotate for easier visibility
			//glRotatef(0, 1.0, 1.0, 1.0);
					 
			glScalef(SPHERE_SCALE,SPHERE_SCALE,SPHERE_SCALE);
			//draw the outline
			if (renderStyle==OUTLINED){
				glColor3f(0.0, 0.0, 0.0);
				//drawSphere(POLY_OUT);
				
			}
			glColor3f(1.0, 0.0, 0.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			//drawSphere();
			glDisable(GL_POLYGON_OFFSET_FILL);
		
		glPopMatrix();
		
		 
		
		
	glPopMatrix();
	
	//a3 code ends here///////////////////////////////////////////////////////////
	
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
				drawCube(POLY_OUT);
				
			}
			glColor3f(1.0, 1.0, 1.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			drawCube();
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
				//drawBody(POLY_OUT);
				
			}
			glColor3f(1.0, 0.0, 0.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			//drawBody();
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
				//drawBody(POLY_OUT);
				
			}
			glColor3f(0.0, 1.0, 0.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			//drawBody();
			glDisable(GL_POLYGON_OFFSET_FILL);
			
			
			//DRAW BEAK TOP RELATIVE TO HEAD
			glPushMatrix();
			
				glTranslatef(BEAK_TOP_X_LOCATION, BEAK_TOP_Y_LOCATION, 0.0);
				glScalef(BEAK_SCALE_X,BEAK_SCALE_Y,BEAK_SCALE_Z);
				
					//draw the outline
				if (renderStyle==OUTLINED){
					glColor3f(0.0, 0.0, 0.0);
					//drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				//drawCube();
				glDisable(GL_POLYGON_OFFSET_FILL);
				
				//DRAW BEAK TOP RELATIVE TO BOTTOM
				glPushMatrix();
				
					glTranslatef(0.0, joint_ui_data->getDOF(Keyframe::BEAK) , 0.0);
					glTranslatef(BEAK_BOT_X_LOCATION, BEAK_BOT_Y_LOCATION, 0.0);
					//glScalef(BEAK_SCALE_X,BEAK_SCALE_Y,BEAK_SCALE_Z);
					
						//draw the outline
					if (renderStyle==OUTLINED){
						glColor3f(0.0, 0.0, 0.0);
						//drawCube(POLY_OUT);
						
					}
					glColor3f(0.0, 0.0, 1.0);
					glEnable(GL_POLYGON_OFFSET_FILL);
					//drawCube();
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
					//drawBody(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				//drawBody();
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
					//drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 1.0, 0.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				//drawCube();
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
					//drawBody(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				//drawBody();
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
					//drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 1.0, 0.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				//drawCube();
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
					//drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				//drawCube();
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
					//drawTrip(POLY_OUT);
					
				}
				glColor3f(0.0, 1.0, 0.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				//drawTrip();
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
					//drawCube(POLY_OUT);
					
				}
				glColor3f(0.0, 0.0, 1.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				//drawCube();
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
					//drawTrip(POLY_OUT);
					
				}
				glColor3f(0.0, 1.0, 0.0);
				glEnable(GL_POLYGON_OFFSET_FILL);
				//drawTrip();
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

void SetVertexColor(unsigned char *pHeightMap, int x, int y)	/* Sets The Color Value For A Particular Index, Depending On The Height Index */
{
	if(!pHeightMap)
		return;								/* Make Sure Our Height Data Is Valid */
	else
	{
		//float fColor = -0.15f + (Height(pHeightMap, x, y ) / 256.0f);
		/* Assign This Blue Shade To The Current Vertex */
		
		//glColor3f(fColor, fColor , fColor);
		
		
		//0=stone - high
		//1=grass - mid
		//2=water - low
		
			
		int low_mid_border=95;
		int mid_high_border=200;
		int high_transition_zone=30;
		int low_transition_zone=10;
		int height = Height(pHeightMap, x, y);
		
		
		//this formula gives the illusion of lighting, need to design actual lighting algorithm
		if (height < low_mid_border) {
			//glDisable(GL_TEXTURE_2D);
			//glColor3f(fColor, fColor , fColor);
			g_fContributionOfTex0=0.0;
			g_fContributionOfTex1=0.0;
			g_fContributionOfTex2=1.0;
		}
		else if (height > mid_high_border)  {
			//glColor3f(fColor, fColor , fColor );
			//glEnable(GL_TEXTURE_2D);
			g_fContributionOfTex0=1.0;
			g_fContributionOfTex1=0.0;
			g_fContributionOfTex2=0.0;
		}
		
		//mid region
		else {
			//glDisable(GL_TEXTURE_2D);
			//glColor3f(fColor,  fColor, fColor );
			//create a transition zone b/w med and high
			
			//transition to high border
			if (height > mid_high_border - high_transition_zone)  {
			
				float tex_mixer = (height - mid_high_border-high_transition_zone)/high_transition_zone;
				
				//mix b/w mid and high textures
				g_fContributionOfTex0=tex_mixer;
				g_fContributionOfTex1=1.0-tex_mixer;
				g_fContributionOfTex2=0.0;
			}
			
			//transition low to mid border
			
			else if (height < low_mid_border + low_transition_zone)  {
			
				float tex_mixer = (height - low_mid_border)/low_transition_zone;
				
				//mix b/w mid and high textures
				g_fContributionOfTex0=0.0;
				g_fContributionOfTex1=tex_mixer;
				g_fContributionOfTex2=1.0-tex_mixer;
			}
			
			else {
				g_fContributionOfTex0=0.0;
				g_fContributionOfTex1=1.0;
				g_fContributionOfTex2=0.0;
			}
				
		}
		

		//Calculate contribution of each of the textures
		//textures must add up to 1
		float rgbValue   = g_fContributionOfTex0 / (1.0f - g_fContributionOfTex2);
		float alphaValue = 1.0f - g_fContributionOfTex2;
		
		// Do some bounds checking...
		if( rgbValue < 0.0f )
			rgbValue = 0.0f;
		if( rgbValue > 1.0f )
			rgbValue = 1.0f;

		if( alphaValue < 0.0f )
			alphaValue = 0.0f;
		if( alphaValue > 1.0f )
			alphaValue = 1.0f;

		glColor4f( rgbValue, rgbValue, rgbValue, alphaValue );
    
		

		
	}
}


void texture_terrain (int x, int y, int height){
	

		glMultiTexCoord2fARB( GL_TEXTURE0_ARB, x, y );
		glMultiTexCoord2fARB( GL_TEXTURE1_ARB, x, y );
		glMultiTexCoord2fARB( GL_TEXTURE2_ARB, x, y );
		//glTexCoord2d(x, y);
	
}


//taken from NEHE
//multitexturing code taken from below:
//http://www.codesampler.com/oglsrc/oglsrc_4.htm
void drawHeightMappedPlane(unsigned char pHeightMap[]) {

	int X = 0, Y = 0;									/* Create Some Variables To Walk The Array With. */
	int x, y, z;										/* Create Some Variables For Readability */

	if(!pHeightMap) return;							/* Make Sure Our Height Data Is Valid */
	
	
	
	GLuint tex0 = raw_texture_load ("texture/snow.raw", 256, 256);
	GLuint tex1 = raw_texture_load ("texture/stone.raw", 256, 256);
	GLuint tex2 = raw_texture_load ("texture/grass.raw", 256, 256);
	
	//use the method described here
	//http://stackoverflow.com/questions/14592925/opengl-multitexturing-and-blending
	
// Simply output texture0 for stage 0.
    //

    glActiveTextureARB( GL_TEXTURE0_ARB );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, tex0);

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    //
    // Texture Stage 1
    //
    // Perform a linear interpolation between the output of stage 0 
    // (i.e texture0) and texture1 and use the RGB portion of the vertex's 
    // color to mix the two. 
    //

    glActiveTextureARB(GL_TEXTURE1_ARB );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, tex1 );

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_COLOR );

    //
    // Texture Stage 2
    //
    // Perform a linear interpolation between the output of stage 1 
    // (i.e texture0 mixed with texture1) and texture2 and use the ALPHA 
    // portion of the vertex's color to mix the two. 
    //

    glActiveTextureARB( GL_TEXTURE2_ARB );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, tex2);

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_ALPHA );
	
	

	
	
	glBegin( GL_QUADS );							
	
	for ( X = 0; X < MAP_SIZE; X += STEP_SIZE )
		for ( Y = 0; Y < MAP_SIZE; Y += STEP_SIZE )
		{
			/* Get The (X, Y, Z) Value For The Bottom Left Vertex */
			x = X;							
			y = Height(pHeightMap, X, Y );
			z = Y;

			/* Set The Color Value Of The Current Vertex */
			SetVertexColor(pHeightMap, x, z);

			texture_terrain(0, 0, y); 
			glVertex3i(x, y, z);						/* Send This Vertex To OpenGL To Be Rendered (Integer Points Are Faster) */

			/* Get The (X, Y, Z) Value For The Top Left Vertex */
			x = X;										
			y = Height(pHeightMap, X, Y + STEP_SIZE );
			z = Y + STEP_SIZE ;

			/* Set The Color Value Of The Current Vertex */
			SetVertexColor(pHeightMap, x, z);

			texture_terrain(0, 1, y);
			glVertex3i(x, y, z);						/* Send This Vertex To OpenGL To Be Rendered */

			/* Get The (X, Y, Z) Value For The Top Right Vertex */
			x = X + STEP_SIZE;
			y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE );
			z = Y + STEP_SIZE ;

			/* Set The Color Value Of The Current Vertex */
			SetVertexColor(pHeightMap, x, z);
			
			texture_terrain(1, 1, y); 
			glVertex3i(x, y, z);						/* Send This Vertex To OpenGL To Be Rendered */

			/* Get The (X, Y, Z) Value For The Bottom Right Vertex */
			x = X + STEP_SIZE;
			y = Height(pHeightMap, X + STEP_SIZE, Y ); 
			z = Y;

			/* Set The Color Value Of The Current Vertex */
			SetVertexColor(pHeightMap, x, z);

			texture_terrain(1, 0, y); 
			glVertex3i(x, y, z);						/* Send This Vertex To OpenGL To Be Rendered */
		}
	glEnd();
	
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);
	
	/*
	glDisable (GL_TEXTURE_2D);
	
	glDisable(GL_BLEND);
	glActiveTexture (GL_TEXTURE1);
	glDisable (GL_TEXTURE_2D);
	glActiveTexture (GL_TEXTURE0);
	glDisable (GL_TEXTURE_2D);

	*/

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);					/*Reset The Color */

	
}




void drawSphere(int is_line)
{
	if (is_line==POLY_OUT){
		glBegin(GL_LINE_LOOP);
		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	else {
		glBegin(GL_QUADS);
	}
	
		glutSolidSphere(1,50,50);	

	glEnd();	


}
void drawTextCube()
{

		glBegin(GL_QUADS);


			//specify normal for lighting
			glNormal3f(0,0,1);
			// draw front face
			glTexCoord2d(1, 1);glVertex3f(-1.0, -1.0, 1.0);
			glTexCoord2d(1, 0);glVertex3f( 1.0, -1.0, 1.0);
			glTexCoord2d(0, 0);glVertex3f( 1.0,  1.0, 1.0);
			glTexCoord2d(0, 1);glVertex3f(-1.0,  1.0, 1.0);


			//specify normal for lighting
			glNormal3f(0,0,-1);
			// draw back face
			glTexCoord2d(1, 1);glVertex3f( 1.0, -1.0, -1.0);
			glTexCoord2d(1, 0);glVertex3f(-1.0, -1.0, -1.0);
			glTexCoord2d(0, 0);glVertex3f(-1.0,  1.0, -1.0);
			glTexCoord2d(0, 1);glVertex3f( 1.0,  1.0, -1.0);


			//specify normal for lighting
			glNormal3f(-1,0,0);
			// draw left face
			glTexCoord2d(1, 1);glVertex3f(-1.0, -1.0, -1.0);
			glTexCoord2d(1, 0);glVertex3f(-1.0, -1.0,  1.0);
			glTexCoord2d(0, 0);glVertex3f(-1.0,  1.0,  1.0);
			glTexCoord2d(0, 1);glVertex3f(-1.0,  1.0, -1.0);


			//specify normal for lighting
			glNormal3f(1,0,0);
			// draw right face
			glTexCoord2d(1, 1);glVertex3f( 1.0, -1.0,  1.0);
			glTexCoord2d(1, 0);glVertex3f( 1.0, -1.0, -1.0);
			glTexCoord2d(0, 0);glVertex3f( 1.0,  1.0, -1.0);
			glTexCoord2d(0, 1);glVertex3f( 1.0,  1.0,  1.0);



			//specify normal for lighting
			glNormal3f(0,1,0);
			// draw top
			glTexCoord2d(1, 1);glVertex3f(-1.0,  1.0,  1.0);
			glTexCoord2d(1, 0);glVertex3f( 1.0,  1.0,  1.0);
			glTexCoord2d(0, 0);glVertex3f( 1.0,  1.0, -1.0);
			glTexCoord2d(0, 1);glVertex3f(-1.0,  1.0, -1.0);


			//specify normal for lighting
			glNormal3f(0,-1,0);
			// draw bottom
			glTexCoord2d(1, 1);glVertex3f(-1.0, -1.0, -1.0);
			glTexCoord2d(1, 0);glVertex3f( 1.0, -1.0, -1.0);
			glTexCoord2d(0, 0);glVertex3f( 1.0, -1.0,  1.0);
			glTexCoord2d(0, 1);glVertex3f(-1.0, -1.0,  1.0);
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
