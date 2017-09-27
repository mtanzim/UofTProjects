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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "keyframe.h"
#include "timer.h"
#include "vector.h"
#include "colours.h"

// *************** GLOBAL VARIABLES *************************

const float PI = 3.14159;

const float SPINNER_SPEED = 0.1*20;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;				// Glut window ID (for display)
int Win[2];					// window (x,y) size

GLUI* glui_joints;			// Glui window with joint controls
GLUI* glui_keyframe;		// Glui window with keyframe controls
GLUI* glui_render;			// Glui window for render style

char msg[256];				// String used for status message
GLUI_StaticText* status;	// Status message ("Status: <msg>")
bool* keyStates = new bool[256]; //is key pressed or not

// ------------------- CUBE VARIABLES ------------------------
Vertex normal={0};
const int TOTAL_FACES_CUBE = 6;
const int TOTAL_FACES_TRIANGLE = 5;
int num_faces = 0;
int num_faces2 = 0;
enum { CUBE, TRAP, TRIANGLE, SPHERE, CYLINDER, CONE, SHARPCONE, CUBETEXTURE };	// the different polygon styles

Vertices facePlane = {-1.0,1.0,1.0,  1.0,1.0,1.0,   1.0,1.0,-1.0, -1.0,1.0,-1.0};

Vertices facesCube[TOTAL_FACES_CUBE] = { //6 faces of a cube
-1.0,-1.0,1.0, /**/1.0,-1.0,1.0,  /**/1.0,1.0,1.0,  /**/-1.0,1.0,1.0, //front
1.0,-1.0,-1.0, /**/-1.0,-1.0,-1.0,/**/-1.0,1.0,-1.0,/**/1.0,1.0,-1.0, //back
-1.0,-1.0,-1.0,/**/-1.0,-1.0,1.0, /**/-1.0,1.0,1.0, /**/-1.0,1.0,-1.0,//left
1.0,-1.0,1.0,  /**/1.0,-1.0,-1.0, /**/1.0,1.0,-1.0, /**/1.0,1.0,1.0,  //right
-1.0,1.0,1.0,  /**/1.0,1.0,1.0,   /**/1.0,1.0,-1.0, /**/-1.0,1.0,-1.0,//top
-1.0,-1.0,-1.0,/**/1.0,-1.0,-1.0, /**/1.0,-1.0,1.0, /**/-1.0,-1.0,1.0 //bottom
};

Vertices facesTrap[TOTAL_FACES_CUBE] = { //6 faces of a cube
-1.0,-1.0,1.0, /**/1.0,-1.0,1.0,  /**/0.7,1.0,0.7,  /**/-0.7,1.0,0.7, //front
1.0,-1.0,-1.0, /**/-1.0,-1.0,-1.0,/**/-0.7,1.0,-0.7,/**/0.7,1.0,-0.7, //back
-1.0,-1.0,-1.0,/**/-1.0,-1.0,1.0, /**/-0.7,1.0,0.7, /**/-0.7,1.0,-0.7,//left
1.0,-1.0,1.0,  /**/1.0,-1.0,-1.0, /**/0.7,1.0,-0.7, /**/0.7,1.0,0.7,  //right
-0.7,1.0,0.7,  /**/0.7,1.0,0.7,   /**/0.7,1.0,-0.7, /**/-0.7,1.0,-0.7,//top
-1.0,-1.0,-1.0,/**/1.0,-1.0,-1.0, /**/1.0,-1.0,1.0, /**/-1.0,-1.0,1.0 //bottom
};

Vertices facesTriangle[TOTAL_FACES_TRIANGLE] = { //5 faces of a triangular prism
-1.0,-1.0,1.0, /**/1.0,-1.0,1.0, /**/0.0,1.0,1.0, /*front*/ 0.0,0.0,0.0,
-1.0,-1.0,-1.0, /**/1.0,-1.0,-1.0,/**/0.0,1.0,-1.0,/*back*/ 0.0,0.0,0.0,
-1.0,-1.0,-1.0,/**/-1.0,-1.0,1.0, /**/0.0,1.0,1.0, /**/0.0,1.0,-1.0,//left
0.0,1.0,1.0,  /**/0.0,1.0,-1.0,   /**/1.0,-1.0,-1.0, /**/1.0,-1.0,1.0,//top
-1.0,-1.0,-1.0,/**/1.0,-1.0,-1.0, /**/1.0,-1.0,1.0, /**/-1.0,-1.0,1.0 //bottom
};

// ---------------- LIGHTING AND MATERIAL -------------------
float spec[4] = { 0.75, 0.75, 0.75, 1 };
float diff[4] = { 0.75, 0.75, 0.75, 1 };


//terrain material properties
float def_spec[4] = { 1.0,1.0, 1.0, 1 };
float def_dif[4] = { 1.0,1.0, 1.0, 1 };
float def_amb[4] = { 0.0,0.0, 0.0, 1 };
float ter_diff[4] = { 1.0f, 1.0f, 1.0f, -0.5f };

float goldspec[4] = { 0.628281,0.555802, 0.366065, 1 };
float golddiff[4] = { 0.75164, 0.60648, 0.22648, 1};
float goldambient[4] = {0.24725,0.1995,0.0745,1};
GLfloat goldshine[] = { 0.4*128.0 };

float cyanspec[4] = { 0.50196078,0.50196078, 0.50196078, 1 };
float cyandiff[4] = { 0.0, 0.50980392, 0.50980392, 1 };
float cyanambient[4] = {0.0,0.1,0.06,1};
GLfloat cyanshine[] = { 0.25*128.0 };

GLfloat mat_shininess[] = { 128.0 };

//-----------------HEIGHT MAPPING VARIABLES-------------------//
//height mapping from nehe
//http://nehe.gamedev.net/tutorial/beautiful_landscapes_by_means_of_height_mapping/16006/
#define		MAP_SIZE	  1024
#define		MAP_REPEAT	  2				//how many times to repeat the texture per axis
#define		STEP_SIZE	  32				/* Width And Height Of Each Quad */
#define		HEIGHT_RATIO  1.0f							/* Ratio That The Y Is Scaled According To The X And Z */
#define		BORDER_MAX    MAP_SIZE
#define		BORDER_MIN    0
#define	    FACE_SIZE	  MAP_SIZE/4

//terrain texture image in raw
const char ter_file[]= "terrain_data/sphere_terrain.raw";
const char ter1_file[]= "terrain_data/sphere_terrain1.raw";
//dimensions of sub faces of the plane to be folded into cubes (MAP_SIZE/4)				
//Holds The Height Map Data
unsigned char g_HeightMap[MAP_SIZE*MAP_SIZE];			
unsigned char g1_HeightMap[MAP_SIZE*MAP_SIZE];

//terrain scaling value
float ter_scaleValue = 1.0f;								
float normal_calc_step=1.0f;

//change this to swicth terrain wirefram from quads to triangles
//1=triangle
//0=quads
const int terrain_triangle=0;
//whether or not to conver to a sphere, or draw the original plane
//0-draw box
//1-draw sphere
//2-draw orginal plane
int is_sphere=2;

//whether or not to use the height map
const int is_height=1;
//whether or not to light the terrain
const int not_lit=0;
//whether or not to wireframe for debug
const int terrain_wire=0;
//flag to select which world to draw
int world_id=0;
//    4
//6 2 1 5
//    3  .
//       (origin)
//flag that tracks which face we're on
int face_id=1;

//factor by which height value is to be applied to the sphere
const float heightmap_factor=0.7f;

//store spherical terrain vertices
Vertices world_vert = {0};
Vertex world_vert_record [MAP_SIZE/STEP_SIZE][MAP_SIZE/STEP_SIZE] = {0};
//store spherical terrain normals
Vertices world_norm = {0};
Vertex world_norm_record [MAP_SIZE/STEP_SIZE][MAP_SIZE/STEP_SIZE] = {0};
int is_recorded = 0;
//store the vertices of the flat world for traversing the sphere
Vertices temp_orig={0}; 
//variable to store miscellaneous vertex information b/w sphere mapping functions
Vertex returner={0};

//how much to transfer the terrain after it has been created
const float TER_TRAN_X=0;//-MAP_SIZE/2;
const float TER_TRAN_Y=0;//-85+80.5-0.4;
const float TER_TRAN_Z=0;//-MAP_SIZE/4;
	
//-----------------TEXTURE VARIABLES-------------------//
//texture loader
 GLuint raw_texture_load(const char *filename, int width, int height, int is_back);
static GLuint texture;
float g_fContributionOfTex0 = 0.33f;
float g_fContributionOfTex1 = 0.33f;
float g_fContributionOfTex2 = 0.33f;
const char *tex0file="texture/mars_seamless.raw";
const char *tex0_lev1_file="texture/face_test.raw";

const char *tex1file="texture/mars_seamless.raw";
const char *tex1_lev1_file="texture/moon.raw";

//--------------------------------------------------------
///////////////////////SUN/MOON VARIABLES////////////////////////////////////
//tm added light position
const float LIGHT_RADIUS             = 1.5*(MAP_SIZE/2);
const float LIGHT_Z             = MAP_SIZE*1.3;
const float LAMP_SCALE=MAP_SIZE/10;
//const float LAMP_SCALE_Y=256;
//const float LAMP_SCALE_Z=256;
//sun texture
#define		MOON_SIZE	  450	
const char *moon_file="texture/moon.raw";
void drawMoonSphere();



//-----------------BACKGROUND VARIABLES-------------------//
#define		BACK_SIZE_X	  	800	
#define		BACK_SIZE_Y		600
//background algorith taken from:
//http://www.felixgers.de/teaching/jogl/textureAsBackground.html
//////////background//////
void drawBackground() ;
const char *backfile="background/stars.raw";
int is_back_drawn=0;
const float WORLD_SCALE_X=MAP_SIZE;
const float WORLD_SCALE_Y=MAP_SIZE;
const float WORLD_SCALE_Z=MAP_SIZE;
const float WORLD_SHIFT_Z=1.1*(MAP_SIZE/4);

/////////////////////////////////////////////////////////////////////////////////////////////////		
//--------------------------------------------------------
//TERRAIN function initializations
void LoadRawFile(const char* strName, int nSize,unsigned char *pHeightMap);
int Height(unsigned char *pHeightMap, int X, int Y);
void SetVertexColor(int height, int x, int y);
void texture_terrain (float x, float z);
void texture_sun (float x, float z);
void drawHeightMappedPlane(unsigned char pHeightMap[],const char *ter_tex, const char *lev1_tex );
void drawOldHeightMappedPlane(unsigned char *pHeightMap);
float* convert_cube_to_sphere (Vertex cube_vertices, Vertex orig_plane_vertex);
float* heightmap_sphere (Vertex sphere_vertices, Vertex normal_vertices, Vertex orig_plane_vertex);
void terrain_draw (GLuint tex0, GLuint tex1, Vertices temp, unsigned char pHeightMap[], int orient, Vertices temp_orig);
//---------------------------------------------------------------------------------------------------------------------------	
	
// ---------------- ANIMATION VARIABLES ---------------------
float BiInterpol (Vertices point, float x, float y);
void whatFaceIsCharOn(float X, float Y);
int whatFaceIsObjectOn(float X, float Y);
void newPositionOfChar (int currentFace, float* FuncStrafe, float* FuncPropel, float* turnAngle);
//Positional Global Variables
double MoveVertical = 0;
double MoveHorizontal = 0;
double Move3 = 0;
float lightAngle=0;
float unitsPerFrame = 2.5; //how much to send him forward per frame (based on keeping foot in same place)
float timePerFrame = 0;

float Propel=0;
float Strafe=0;
const float PropelAmount=4;//0.1307;//units/call;2.5 units/frame;8 frames/total;
const float StrafeAmount=4;// 0.1307;//same as propel but sideways
float gROOT_ROTATE_Y =  0.0; //character rotating horizontally (not strafing case)
const float gRotateAmount=10;
int strafeEnabled = 0;

Vertex charSpherePos={0}; //sphere position
float percentStepSizeTraversed=0;

// Camera settings
bool updateCamZPos = false;
bool updateAngleonLeftButton = false;
int  lastX = 0;
int  lastY = 0;
const float ZOOM_SCALE = 0.8;
//11,85,21
GLdouble camXPos = 0;//20;//11.0;
GLdouble camYPos =  0;//-180;//85;//-5.5;
GLdouble camXPosRotate =  0.0;
GLdouble camYPosRotate =  0.0;
GLdouble camZPos = -55.5;
GLdouble camZPosTemp = 0;

const GLdouble CAMERA_FOVY = 60.0;
const GLdouble NEAR_CLIP   = 0.1;
const GLdouble FAR_CLIP    = 9000.0;

// Render settings
enum { WIREFRAME, SOLID, OUTLINED, METALLIC, MATTE };	// README: the different render styles
int renderStyle = METALLIC;			// README: the selected render style

// Animation settings
int animate_mode = 0;			// 0 = no anim, 1 = animate
int animation_over=1; //animation is totally done, after finalDOF has happened
int animation_stopped=0; //first time key released, finalDOF is created and injected. this set to 1 after
int shipfly = 0; //ship flying once character gets close
unsigned char animation_dir = 'n';
// Keyframe settings
const char filenameKF[] = "keyframes.txt";	// file for loading / saving keyframes

Keyframe* keyframes;			// list of keyframes
Keyframe FinalDOF; //the keyframe that gets injected after animation stops

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
const float TIME_MAX = 10.0;	// README: specifies the max time of the animation
const float SEC_PER_FRAME = 1.0 / 120.0;

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
const float ROOT_TRANSLATE_X_MIN = -500.0;
const float ROOT_TRANSLATE_X_MAX =  500.0;
const float ROOT_TRANSLATE_Y_MIN = -500.0;
const float ROOT_TRANSLATE_Y_MAX =  500.0;
const float ROOT_TRANSLATE_Z_MIN = -500.0;
const float ROOT_TRANSLATE_Z_MAX =  500.0;
const float ROOT_ROTATE_X_MIN    = -180.0;
const float ROOT_ROTATE_X_MAX    =  180.0;
const float ROOT_ROTATE_Y_MIN    = -180.0;
const float ROOT_ROTATE_Y_MAX    =  180.0;
const float ROOT_ROTATE_Z_MIN    = -180.0;
const float ROOT_ROTATE_Z_MAX    =  180.0;
const float HEAD_MIN             = -60.0;
const float HEAD_MAX             =  60.0;
const float SHOULDER_PITCH_MIN   = -105.0;
const float SHOULDER_PITCH_MAX   =  105.0;
const float SHOULDER_YAW_MIN     = -105.0;
const float SHOULDER_YAW_MAX     =  105.0;
const float SHOULDER_ROLL_MIN    = -105.0;
const float SHOULDER_ROLL_MAX    =  105.0;
const float HIP_PITCH_MIN        = -105.0;
const float HIP_PITCH_MAX        =  105.0;
const float HIP_YAW_MIN          = -105.0;
const float HIP_YAW_MAX          =  105.0;
const float HIP_ROLL_MIN         = -105.0;
const float HIP_ROLL_MAX         =  105.0;
const float BEAK_MIN             =  0.0;
const float BEAK_MAX             =  1.0;
const float ELBOW_MIN            = -105.0;
const float ELBOW_MAX            = 105.0;
const float KNEE_MIN             = -105.0;
const float KNEE_MAX             = 105.0;


char screenmsg[128]; 
//**************SHIP CONSTS*******************************
const float SHIP_BODY_HEIGHT = 20.0;
const float SHIP_BODY_WIDTH =  8.0;
float ship_x =0;
float ship_y =0;
float ship_rot_y =0;
float ship_z =0;

//***************fuel constants*******************
const int total_fuel = 5;
int fuel_location[total_fuel][4] = {0};//x,y,z,fuel_obtained
float fuel_bounce = 0;
int fuel_rev = 0;
const float BOUNCE_FACTOR = 0.4;
int fuel_remaining = total_fuel;
bool at_fuel=0;
//***************enemy constants*******************
const int total_enemy = 3;
int enemy_location[total_enemy][4] = {0};//x,y,z,fuel_obtained
const float ENEMY_MOVE_FACTOR = 0.4;
float enemy_move=0;
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
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void keyboardSpecial(int key, int x, int y);
void motion(int x, int y);
void motionPassive(int x, int y);


// Functions to help draw the object
Vector getInterpolatedJointDOFS(float time);
void drawCubeOld();
void drawCubeV(Vertices face);
void drawCubeVOutline(Vertices face);
void drawShapeDispatcher(int shape, int type, GLfloat *fillColour, GLfloat *outlineColour);

void drawTriangleV(Vertices face);
void drawTriangleVOutline(Vertices face);
void drawTriangleDispatcher(int type, GLfloat *fillColour, GLfloat *outlineColour);

void drawSphere(double r, int lats, int longs);
void drawSphereOutline(double r, int lats, int longs);

void drawCylinder(float base, float top, float height, int slices, int stacks);
void drawCylinderOutline(float base, float top, float height, int slices, int stacks);

void print(float x, float y,int z, GLfloat *fillColour, char *string, void* font);

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
        printf("Using 800x600 window by default...\n");
        Win[0] = 800;
        Win[1] = 600;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }

	//load terrain map
	LoadRawFile(ter_file, MAP_SIZE * MAP_SIZE, g_HeightMap);
	LoadRawFile(ter1_file, MAP_SIZE * MAP_SIZE, g1_HeightMap);
	
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
	//glutPassiveMotionFunc(motionPassive);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(keyboardSpecial);
	glutIgnoreKeyRepeat(1);
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
	if (keyframeID>maxValidKeyframe)
	{
		maxValidKeyframe=keyframeID;
	}

	// Update the appropriate entry in the 'keyframes' array
	// with the 'joint_ui_data' data
	keyframes[keyframeID] = *joint_ui_data;

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
void animateButton()
{
	//printf("start button press animate %d animation over %d \n",animate_mode,animation_over);
  // synchronize variables that GLUT uses
  glui_keyframe->sync_live();
  // toggle animation mode and set idle function appropriately
  if( animate_mode == 0 )
  {
	// start animation
	frameRateTimer->reset();
	//animationTimer->reset();//so taht stopping, startig animaton wont reset animation
	animate_mode = 1;
	animation_over = 0;
	//printf("animation not over %d\n",animation_over);
	GLUI_Master.set_glutIdleFunc(animate);

	// Let the user know the animation is running
	sprintf(msg, "Status: Animating...");
	status->set_text(msg);
  }
  else if(!keyStates['w']&!keyStates['a']&!keyStates['s']&!keyStates['d'])
  {
	// stop animation
	animate_mode = 0;
	animation_stopped=0;
	//GLUI_Master.set_glutIdleFunc(NULL); //only stop after animation over, in display()
	//printf("animating mode stop\n");
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

    GLUI_Master.set_glutIdleFunc(animate);//changed to animate manually

/*
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
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "light angle:", GLUI_SPINNER_INT, joint_ui_data->getDOFPtr(Keyframe::LIGHTANGLE));
	glui_spinner->set_int_limits(-180, 180, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED/60);

	// Create controls to specify head rotation
	glui_panel = glui_joints->add_panel("Head");

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "head pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::HEAD_PITCH));
	glui_spinner->set_float_limits(HEAD_MIN, HEAD_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "head yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::HEAD_YAW));
	glui_spinner->set_float_limits(HEAD_MIN, HEAD_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "head roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::HEAD_ROLL));
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

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "R_ELBOW pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_ELBOW_PITCH));
	glui_spinner->set_float_limits(ELBOW_MIN, ELBOW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "R_ELBOW yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_ELBOW_YAW));
	glui_spinner->set_float_limits(ELBOW_MIN, ELBOW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "R_ELBOW roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_ELBOW_ROLL));
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

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "L_ELBOW pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_ELBOW_PITCH));
	glui_spinner->set_float_limits(ELBOW_MIN, ELBOW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "L_ELBOW yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_ELBOW_YAW));
	glui_spinner->set_float_limits(ELBOW_MIN, ELBOW_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "L_ELBOW roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_ELBOW_ROLL));
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

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "R_KNEE pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_KNEE_PITCH));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "R_KNEE yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_KNEE_YAW));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "R_KNEE roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_KNEE_ROLL));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "R_FOOT pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_FOOT_PITCH));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "R_FOOT yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_FOOT_YAW));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "R_FOOT roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::R_FOOT_ROLL));
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

	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "L_KNEE pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_KNEE_PITCH));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "L_KNEE yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_KNEE_YAW));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "L_KNEE roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_KNEE_ROLL));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "L_FOOT pitch:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_FOOT_PITCH));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "L_FOOT yaw:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_FOOT_YAW));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
	
	glui_spinner = glui_joints->add_spinner_to_panel(glui_panel, "L_FOOT roll:", GLUI_SPINNER_FLOAT, joint_ui_data->getDOFPtr(Keyframe::L_FOOT_ROLL));
	glui_spinner->set_float_limits(KNEE_MIN, KNEE_MAX, GLUI_LIMIT_CLAMP);
	glui_spinner->set_speed(SPINNER_SPEED);
*/	
	

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
	//glui_keyframe->add_button_to_panel(glui_panel, "Load Keyframe", 0, loadKeyframeButton);
	//glui_keyframe->add_button_to_panel(glui_panel, "Load Keyframes From File", 0, loadKeyframesFromFileButton);
	//glui_keyframe->add_button_to_panel(glui_panel, "Start / Stop Animation", 0, animateButton);
	//glui_keyframe->add_column_to_panel(glui_panel, false);
	//glui_keyframe->add_button_to_panel(glui_panel, "Update Keyframe", 0, updateKeyframeButton);
	//glui_keyframe->add_button_to_panel(glui_panel, "Save Keyframes To File", 0, saveKeyframesToFileButton);
	//glui_keyframe->add_button_to_panel(glui_panel, "Render Frames To File", 0, renderFramesToFileButton);

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
	glui_render->add_radiobutton_to_group(glui_radio_group, "Metallic");
	glui_render->add_radiobutton_to_group(glui_radio_group, "Matte");
	//
	// ***************************************************


	// Tell GLUI windows which window is main graphics window
	//glui_joints->set_main_gfx_window(windowID);
	glui_keyframe->set_main_gfx_window(windowID);
	glui_render->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
	loadKeyframesFromFileButton(MoveHorizontal);
	//dude initial position
	joint_ui_data->setDOF(Keyframe::R_SHOULDER_PITCH,-90);
	joint_ui_data->setDOF(Keyframe::L_SHOULDER_PITCH,-90);

	Propel = (FACE_SIZE/2)*3;
	Strafe = (FACE_SIZE/2)*3;
	joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_X,Propel);
	joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_Z,Strafe);
	joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_Y,0);
								//Height(g_HeightMap, 
								//joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X)-TER_TRAN_X, 
								//joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z)-TER_TRAN_Z));
	//ship initial position
	ship_x=-4.3*20-7;
	ship_z=17.8*20;
	ship_y=Height(g_HeightMap,ship_x-TER_TRAN_X,ship_z-TER_TRAN_Z);
	
	//fuel cell location
	
	for (int q = 0;q<fuel_remaining;q++)
	{
		fuel_location[q][0] = (rand() % MAP_SIZE)+TER_TRAN_X;//-20-q*10;
		printf("fuel %d x %d",q,fuel_location[q][0]);
		fuel_location[q][2] = (rand() % MAP_SIZE)+TER_TRAN_Z;//-20-q*10;
		printf(" z %d",fuel_location[q][2]);
		fuel_location[q][1] = Height(g_HeightMap,fuel_location[q][0]-TER_TRAN_X,fuel_location[q][2]-TER_TRAN_Z);
		printf(" y %d \n",fuel_location[q][1]);
	}
	
	for (int q = 0;q<total_enemy;q++)
	{
		enemy_location[q][0] = (rand() % MAP_SIZE)+TER_TRAN_X;//-20-q*10;
		//printf("Sentinel %d x %d",q,enemy_location[q][0]);
		enemy_location[q][2] = (rand() % MAP_SIZE)+TER_TRAN_Z;//-20-q*10;
		//printf(" z %d",enemy_location[q][2]);
		enemy_location[q][1] = Height(g_HeightMap,enemy_location[q][0]-TER_TRAN_X,enemy_location[q][2]-TER_TRAN_Z);
		//printf(" y %d \n",enemy_location[q][1]);
	}
	
	sprintf(screenmsg, "Fuel Cells Remaining: %d ", fuel_remaining);
	//enable depth buffer
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(1.0, 2);
		
	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE); 
	//specify shader model
    glShadeModel(GL_SMOOTH);
    //GLUI_Master.set_glutIdleFunc(NULL);manual comment
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
	//
	int i = 0;
	Vector p0;
	Vector p1;
	while( i <= maxValidKeyframe && keyframes[i].getTime() < time )
		i++;
		
	if (!animate_mode)//if regular animation, use regular i and stuff from file
	{
		//printf("animation offing i %d time %f  keyframetime %f\n",i,time,keyframes[i].getTime());
		if (!animation_stopped)
		{
			for( int j = 0; j < Keyframe::NUM_JOINT_ENUM; j++ )
				{//if animation just stopped, inject the standing keyframe
					if ((j==10)| (j==25))//if shoulders, set to -90 degrees
						FinalDOF.setDOF(j,-90);
					else
						FinalDOF.setDOF(j,0);
				}
			FinalDOF.setTime(keyframes[i].getTime());
			animation_stopped=1;
			keyframes[i].setDOFVector(FinalDOF.getDOFVector());
		}
		
		if ((time >= FinalDOF.getTime())|| //reached end of frame (added 0.01 to make final frame stop)
			((i == 0)&(time == 0)))//special case for last frame, sometimes the recieving time resets to zero
		{
			animation_over=1;
			//printf("animation over drawing i %d \n",i);
			//GLUI_Master.set_glutIdleFunc(NULL); //manual comment stop calling this function, stop animating
			return FinalDOF.getDOFVector();
		}
	}

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
	p0 = keyframes[i-1].getDOFVector();
	p1 = keyframes[i].getDOFVector();

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
    glMatrixMode(GL_PROJECTION)  ;
    glLoadIdentity();
	gluPerspective(CAMERA_FOVY, (GLdouble)Win[0]/(GLdouble)Win[1], NEAR_CLIP, FAR_CLIP);
}



////////////////DRAW AXIS///////////
#define RADPERDEG 0.0174533

void Arrow(GLdouble x1,GLdouble y1,GLdouble z1,GLdouble x2,GLdouble y2,GLdouble z2,GLdouble D)
{
  double x=x2-x1;
  double y=y2-y1;
  double z=z2-z1;
  double L=sqrt(x*x+y*y+z*z);

    GLUquadricObj *quadObj;

    glPushMatrix ();

      glTranslated(x1,y1,z1);

      if((x!=0.)||(y!=0.)) {
        glRotated(atan2(y,x)/RADPERDEG,0.,0.,1.);
        glRotated(atan2(sqrt(x*x+y*y),z)/RADPERDEG,0.,1.,0.);
      } else if (z<0){
        glRotated(180,1.,0.,0.);
      }

      glTranslatef(0,0,L-4*D);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluCylinder(quadObj, 2*D, 0.0, 4*D, 32, 1);
      gluDeleteQuadric(quadObj);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluDisk(quadObj, 0.0, 2*D, 32, 1);
      gluDeleteQuadric(quadObj);

      glTranslatef(0,0,-L+4*D);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluCylinder(quadObj, D, D, L-4*D, 32, 1);
      gluDeleteQuadric(quadObj);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluDisk(quadObj, 0.0, D, 32, 1);
      gluDeleteQuadric(quadObj);

    glPopMatrix ();

}
void drawAxes(GLdouble length)
{
    glPushMatrix();
    glTranslatef(-length,0,0);
    Arrow(0,0,0, 2*length,0,0, 0.2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,-length,0);
    Arrow(0,0,0, 0,2*length,0, 0.2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,-length);
    Arrow(0,0,0, 0,0,2*length, 0.2);
    glPopMatrix();
}

/////////////////////////////////////////////


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
	is_sphere=1+MoveHorizontal*10;
    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
	// Get the time for the current animation step, if necessary
	if(( animate_mode | !animation_over)&&(!shipfly))
	{
		float curTime = animationTimer->elapsed();
		//printf("animating mode %d over %d\n",animate_mode,animation_over);
		if( curTime >= keyframes[maxValidKeyframe].getTime() )
		{
			// Restart the animation
			animationTimer->reset();
			curTime = animationTimer->elapsed();
			//animation_over=1;
			printf("animation over %d!\n",animation_over);
		}
		
		if (animate_mode) //if still animating, animation not over
		{animation_over=0;}
		else if (animation_over) //if not animating and animation over, stop
		{}
		
		//assuming every frame has the same time interval, get the time per frame
		timePerFrame = keyframes[1].getTime()-keyframes[0].getTime();//not needed?
		
		// Get the interpolated joint DOFs if not only rotating
		if ((!strafeEnabled)&&(keyStates['a']||keyStates['d'])&&(!keyStates['w']&&!keyStates['s'])){
			joint_ui_data->setDOF(Keyframe::ROOT_ROTATE_Y,gROOT_ROTATE_Y);
			animation_over=1;
			}
		else
			joint_ui_data->setDOFVector( getInterpolatedJointDOFS(curTime) );
		
		/////movement////////
		if (strafeEnabled)
		{
			gROOT_ROTATE_Y = 0;
			if ((keyStates['w'])&(Propel-TER_TRAN_Z<=BORDER_MAX))
				Propel += PropelAmount;
			else if ((keyStates['s'])&(Propel-TER_TRAN_Z>=BORDER_MIN))
				Propel -= PropelAmount;
			if ((keyStates['a'])&(Strafe-TER_TRAN_X<=BORDER_MAX))
				Strafe += StrafeAmount;
			else if ((keyStates['d'])&(Strafe-TER_TRAN_X>=BORDER_MIN))
				Strafe -= StrafeAmount;
		}
		else //strafe not enabled
		{
			if ((keyStates['w'])&(Propel-TER_TRAN_Z<=BORDER_MAX))
			{
				Propel += PropelAmount*cos(gROOT_ROTATE_Y*(PI/180));
				Strafe += StrafeAmount*sin(gROOT_ROTATE_Y*(PI/180));
			}
			else if ((keyStates['s'])&(Propel-TER_TRAN_Z>=BORDER_MIN))
			{
				Propel -= PropelAmount*cos(gROOT_ROTATE_Y*(PI/180));
				Strafe -= StrafeAmount*sin(gROOT_ROTATE_Y*(PI/180));
			}
			if (keyStates['a'])
				gROOT_ROTATE_Y += gRotateAmount;
			else if (keyStates['d'])
				gROOT_ROTATE_Y -= gRotateAmount;
		}
		//wrap around visible portion of plane
		whatFaceIsCharOn(Strafe,Propel);
		//printf("face_id %d \n",face_id);
		newPositionOfChar(face_id,&Strafe,&Propel,&gROOT_ROTATE_Y);
		//printf("propel %f strafe %f \n",Propel, Strafe);
		joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_Z,Propel);
		joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_X,Strafe);
		joint_ui_data->setDOF(Keyframe::ROOT_ROTATE_Y,gROOT_ROTATE_Y);

		//printf("cos %f sin %f rotate %f\n",cos(gROOT_ROTATE_Y*(PI/180)),sin(gROOT_ROTATE_Y*(PI/180)),gROOT_ROTATE_Y);
		
		// Update user interface
		joint_ui_data->setTime(curTime);
		glui_keyframe->sync_live();
//****************************************************INTERACTIVE ELEMENTS*************************************************************
		//reached the ship!
		if ((abs(ship_x-joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X))<20)&&
			(abs(ship_z-joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z))<20))
		{
			if (!fuel_remaining)
			{
				printf("reached ship!\n");
				shipfly = 1;
				GLUI_Master.set_glutIdleFunc(animate); //permanently animate with shipfly on
			}
			else
			{
				sprintf(screenmsg, "Get The Fuel Cells First! ");
			}
		}
		else
		{
			at_fuel = 0;
			if (!fuel_remaining)
			{
				sprintf(screenmsg, "GET TO THE SHIP");
			}
			else
				sprintf(screenmsg, "Fuel Cells Remaining: %d ", fuel_remaining);
		}
		for (int q = 0;q<total_fuel;q++)
		{
			//reached the FUEL CELL!
			if ((abs(fuel_location[q][0]-joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X))<5)&&
				(abs(fuel_location[q][2]-joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z))<5)&&
				(!at_fuel)&&
				(!fuel_location[q][3])&&//fuel not obtained
				(fuel_remaining>0))
			{
				fuel_location[q][3]=1;
				at_fuel = 1;
				fuel_remaining--;
				sprintf(screenmsg, "Got The Fuel Cell! %d more to find! ",fuel_remaining);
			}
		}
		for (int q = 0;q<total_enemy;q++)
		{
			//Hit a sentinel!
			if ((abs(enemy_location[q][0]-joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X))<5)&&
				(abs(enemy_location[q][2]-joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z))<5))
			{
				enemy_location[q][3]=1;
				fuel_remaining=total_fuel;
				for (int q = 0;q<total_fuel;q++)
				{
					//turn on all the fuels again and reset locations
					fuel_location[q][3]=0;
					fuel_location[q][0] = (rand() % MAP_SIZE)+TER_TRAN_X;//-20-q*10;
					fuel_location[q][2] = (rand() % MAP_SIZE)+TER_TRAN_Z;//-20-q*10;
					fuel_location[q][1] = Height(g_HeightMap,fuel_location[q][0]-TER_TRAN_X,fuel_location[q][2]-TER_TRAN_Z);
				}
				sprintf(screenmsg, "You Hit a Sentinel! Fuel Cells Lost! %d more to find! ",fuel_remaining);
			}
		}
//****************************************************FLY AWAY*************************************************************
	}//animate mode over
	else if (shipfly) //reached the ship, time to leave!
	{
		ship_y += 4;
		ship_rot_y += 4;
		joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_Z,ship_z);
		joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_Y,ship_y);
		joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_X,ship_x);
		
	}
		
	////////////////////////////////////////////////////LIGHTING//////////////////////////
	glEnable(GL_POLYGON_OFFSET_FILL);
	float modelTwoside[] = {GL_TRUE};

	if (renderStyle == METALLIC)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glMaterialfv(GL_FRONT, GL_SPECULAR, goldspec);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, golddiff);
		glMaterialfv(GL_FRONT, GL_AMBIENT, goldambient);
		glMaterialfv(GL_FRONT, GL_SHININESS, goldshine);
		
		glLightfv(GL_LIGHT0, GL_SPECULAR, goldspec);
		glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, modelTwoside);
	}
	else if (renderStyle == MATTE)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glMaterialfv(GL_FRONT, GL_SPECULAR, cyanspec);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cyandiff);
		glMaterialfv(GL_FRONT, GL_AMBIENT, cyanambient);
		glMaterialfv(GL_FRONT, GL_SHININESS, Colourblack);
		
		glLightfv(GL_LIGHT0, GL_SPECULAR, cyanspec);
		glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, modelTwoside);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	
	//////////////////////////////////////////////CAMERA////////////////////////////////////
    // Specify camera transformation
	
	if (!shipfly) //break camera from character once it flies
	{
		glTranslatef(camXPos, camYPos, camZPos);
		print(MoveHorizontal-0.35,MoveVertical-0.25,-camZPos-0.1,Colourwhite,screenmsg,GLUT_BITMAP_TIMES_ROMAN_24);
		glRotatef(camXPosRotate, 0.0,1.0,0.0);
		//alter which asix to rotate from, as the x rotation changes
		glRotatef(camYPosRotate, -cos(camXPosRotate*(PI/180)),0.0,-sin(camXPosRotate*(PI/180)));
		/*glTranslatef(-joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),//attach camera to character
						 -joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
						 -joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z));
		*/
		glTranslatef(-charSpherePos[0],-charSpherePos[1],-charSpherePos[2]);
	}
	else//initial char position, and look at ship flying away
	{	
		print(-0.35,-0.25,-camZPos-0.1,Colourwhite,"GET TO THE SHIP",GLUT_BITMAP_TIMES_ROMAN_24);
		gluLookAt(-TER_TRAN_X,Height(g_HeightMap,ship_x-TER_TRAN_X,ship_z-TER_TRAN_Z),-TER_TRAN_Z,
					joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),
					joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
					joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z),
					0,1,0);//up vector
	}
	
	///////////////////////AXES
	drawAxes(1024.0);

	//////////////////////////////////////LIGHT SOURCE//////////////////////////////////////
	//if (Move3!=0)
		lightAngle+=3;
		
	GLfloat lightPos0[] = {cos((*joint_ui_data->getDOFPtr(Keyframe::LIGHTANGLE)+lightAngle)*(PI/180)), 
						   sin((*joint_ui_data->getDOFPtr(Keyframe::LIGHTANGLE)+lightAngle)*(PI/180)), 
						   0.0f, 0.0f};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, Colourwhite);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	/////////////////////////////////////////draw background//////////////////////
	//drawBackground();
	glDisable(GL_LIGHTING);
	glPushMatrix();
		glTranslatef(0.0,0.0,WORLD_SHIFT_Z);
		glScalef(WORLD_SCALE_X,WORLD_SCALE_Y,WORLD_SCALE_Z);
		//drawShapeDispatcher(CUBETEXTURE,renderStyle,ColourOrange,Colourblack);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	
		//////////////////////////////////////LIGHT SOURCE INDICATOR (SUN/MOON)//////////////////////////////////////
	//draw the light source indicator
	glPushMatrix();
			glDisable(GL_LIGHTING);
				
			glTranslatef((LIGHT_RADIUS*(cos((*joint_ui_data->getDOFPtr(Keyframe::LIGHTANGLE)+lightAngle)*(PI/180)))),
					 (LIGHT_RADIUS*(sin((*joint_ui_data->getDOFPtr(Keyframe::LIGHTANGLE)+lightAngle)*(PI/180)))),
					 LIGHT_Z);
			//glRotatef(180.0,0.0, 1.0, 0.0);
			glPushMatrix();
				glScalef(LAMP_SCALE,LAMP_SCALE,LAMP_SCALE);
				//drawShapeDispatcher(SPHERE,renderStyle,ColourOrange,Colourblack);
				drawMoonSphere();
				
			glPopMatrix();
			
			glEnable(GL_LIGHTING);
	glPopMatrix();
	////////////////////////////////////////////////////WORLD//////////////////////////
	//a plane
	glPushMatrix();
	//glEnable(GL_COLOR_MATERIAL);
		if (not_lit==1){
			glDisable(GL_LIGHTING);
		}
		else {
			glEnable(GL_LIGHTING);
			
		}
		//from NEHE
		// setup transformation for body part
		//move the plane for easier visibility
		
		glTranslatef(TER_TRAN_X, TER_TRAN_Y, TER_TRAN_Z);
		//glRotatef(166.0, 1.0, 0.0, 0.0);
		//glRotatef(45.0, 1.0, 0.0, 0.0);
		
		glScalef(ter_scaleValue, ter_scaleValue * HEIGHT_RATIO, ter_scaleValue);
		//drawOldHeightMappedPlane(g_HeightMap);
		glTranslatef(TER_TRAN_X, TER_TRAN_Y, TER_TRAN_Z*2);
		
		if (world_id == 0) {
			drawHeightMappedPlane(g_HeightMap, tex0file, tex0_lev1_file);
		} 
		else if (world_id == 1) {
			drawHeightMappedPlane(g1_HeightMap, tex1file, tex1_lev1_file);
		}
		else {
			drawHeightMappedPlane(g_HeightMap, tex0file, tex0_lev1_file);
		}
		
			
		//glDisable(GL_COLOR_MATERIAL);
		//drawTextCube();
	glPopMatrix();
	///////////////////////////////////////////////SHIP///////////////////////////////////////	
	// draw main cylinder 
	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);
		glTranslatef(0,-4.6,0);
		if (!shipfly)
			ship_y=Height(g_HeightMap,ship_x-TER_TRAN_X,ship_z-TER_TRAN_Z);
		glTranslatef(ship_x, ship_y, ship_z);
		glRotatef(ship_rot_y,0,1,0);//rotate for effect, after launch
		glScalef(SHIP_BODY_WIDTH,SHIP_BODY_HEIGHT,SHIP_BODY_WIDTH);
		drawShapeDispatcher(CYLINDER,renderStyle,Colourdarkgreen,Colourblack);
		//draw top cone
		glPushMatrix();
			glTranslatef(0,1,0);
			glScalef(1,7/SHIP_BODY_HEIGHT,1);
			drawShapeDispatcher(SHARPCONE,renderStyle,Colourdarkgreen,Colourblack);
		glPopMatrix();
		//draw door
		glPushMatrix();
			glTranslatef(0,4.7/SHIP_BODY_HEIGHT,(7.8)/SHIP_BODY_WIDTH);
			glScalef(2.6/SHIP_BODY_WIDTH,4.7/SHIP_BODY_HEIGHT,1/SHIP_BODY_WIDTH);
			drawShapeDispatcher(CUBE,renderStyle,ColourDarkGray,Colourblack);
		glPopMatrix();
		//draw surrounding cylinders
		glPushMatrix();
			glRotatef(45,0,1,0);
			glTranslatef(12.4/SHIP_BODY_WIDTH,0,0);//center the origin to edge of cylinder
			glScalef(5/SHIP_BODY_WIDTH,12/SHIP_BODY_HEIGHT,5/SHIP_BODY_WIDTH);
			drawShapeDispatcher(CYLINDER,renderStyle,ColourLightGray,Colourblack);
		glPopMatrix();
		glPushMatrix();
			glRotatef(45,0,1,0);
			glTranslatef(-12.4/SHIP_BODY_WIDTH,0,0);
			glScalef(5/SHIP_BODY_WIDTH,12/SHIP_BODY_HEIGHT,5/SHIP_BODY_WIDTH);
			drawShapeDispatcher(CYLINDER,renderStyle,ColourLightGray,Colourblack);
		glPopMatrix();
		glPushMatrix();
			glRotatef(45,0,1,0);
			glTranslatef(0,0,12.4/SHIP_BODY_WIDTH);
			glScalef(5/SHIP_BODY_WIDTH,12/SHIP_BODY_HEIGHT,5/SHIP_BODY_WIDTH);
			drawShapeDispatcher(CYLINDER,renderStyle,ColourLightGray,Colourblack);
		glPopMatrix();
		glPushMatrix();
			glRotatef(45,0,1,0);
			glTranslatef(0,0,-12.4/SHIP_BODY_WIDTH);
			glScalef(5/SHIP_BODY_WIDTH,12/SHIP_BODY_HEIGHT,5/SHIP_BODY_WIDTH);
			drawShapeDispatcher(CYLINDER,renderStyle,ColourLightGray,Colourblack);
		glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();
	/////////////////////////////////////collectibles/////////////////////////////////////////
	//fuel cell
	if (fuel_rev) //timing of hte bouncing is outside the for loop else sporadic bouncing
		fuel_bounce-=BOUNCE_FACTOR;
	else
		fuel_bounce+=BOUNCE_FACTOR;

	if (fuel_bounce >= 2)
		fuel_rev = 1;
	else if (fuel_bounce <= -3)
		fuel_rev = 0;
	for (int q = 0;q<total_fuel;q++)
	{ //draw only if dude hasn't reached it yet
		if (!fuel_location[q][3])
		{
			glPushMatrix();//three cylinders
				glEnable(GL_COLOR_MATERIAL);
				glTranslatef(fuel_location[q][0],fuel_location[q][1],fuel_location[q][2]);
				glTranslatef(0,fuel_bounce,0);
				glPushMatrix();
					glScalef(0.8*2,0.8,0.8*2);
					drawShapeDispatcher(CYLINDER,renderStyle,ColourNeonBlue,Colourblack);
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0,0.8,0);
					glScalef(0.8*2,0.8,0.8*2);
					drawShapeDispatcher(CYLINDER,renderStyle,ColourLightGray,Colourblack);
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0,-0.8,0);
					glScalef(0.8*2,0.8,0.8*2);
					drawShapeDispatcher(CYLINDER,renderStyle,ColourLightGray,Colourblack);
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0,0.8*2,0);
					glScalef(0.8*2,0.8,0.8*2);
					drawShapeDispatcher(CYLINDER,renderStyle,ColourVioletRed,Colourblack);
				glPopMatrix();
				glDisable(GL_COLOR_MATERIAL);
			glPopMatrix();
		}
	}
	//Sentinels
	for (int q = 0;q<total_enemy;q++)
	{ 
		//they gotta move too
		if ((enemy_location[q][0]-TER_TRAN_X<BORDER_MAX)&&(enemy_location[q][0]-TER_TRAN_X>BORDER_MIN))
			enemy_location[q][0] += ((rand()%3)-1)*ENEMY_MOVE_FACTOR;
		if ((enemy_location[q][0]-TER_TRAN_Z<BORDER_MAX)&&(enemy_location[q][0]-TER_TRAN_Z>BORDER_MIN))
			enemy_location[q][2] += ((rand()%3)-1)*ENEMY_MOVE_FACTOR;
		
		//printf("Sentinel %d x %d y %d z %d\n",q,enemy_location[q][0],enemy_location[q][1],enemy_location[q][2]);
		glPushMatrix();//just an evil cube
			glEnable(GL_COLOR_MATERIAL);
			glTranslatef(enemy_location[q][0],enemy_location[q][1]+2,enemy_location[q][2]);
			glPushMatrix();
				glScalef(3,3,3);
				drawShapeDispatcher(CUBE,renderStyle,ColourSilver,Colourblack);
			glPopMatrix();
		glPopMatrix();
	}
	///////////////////////////////////////////////BODY///////////////////////////////////////	    
	
	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);
		
		/*glTranslatef(joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
					 joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z)); 
		*/
		glTranslatef(charSpherePos[0],charSpherePos[1],charSpherePos[2]);
		glRotatef(*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_X),
                  1.0, 0.0, 0.0);
		glRotatef(*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Y),
                  0.0, 1.0, 0.0);
		glRotatef(*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Z),
                  0.0, 0.0, 1.0);   
                  
                  //set up transformation for body
		
		glPushMatrix();//only torso cylinder transformations
			glTranslatef(0.0,-0.8,0.0);
			//scale to make larger
			glScalef(0.9,2.2,0.8);
			// draw body 
			drawShapeDispatcher(CYLINDER,renderStyle,ColourDimGray,Colourblack);
		glPopMatrix();
		//draw head-----------------------------------------------------------------------------------
		glPushMatrix();
			//Translating head to above body		  
			glTranslatef(0.0,1.5,0.0);
			//head rotation, nodding
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::HEAD_ROLL),
					  1.0, 0.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::HEAD_YAW),
					  0.0, 1.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::HEAD_PITCH),
					  0.0, 0.0, 1.0);
			//translate origin (rotation joint) to bottom of head
			glTranslatef(0.0,0.7,0.0);
			
			//draw top head
			glPushMatrix();
				glTranslatef(0.0,0.4,0.0);
				glScalef(0.6,0.2,0.6);
				drawShapeDispatcher(TRAP,renderStyle,ColourNeonBlue,Colourblack);
			glPopMatrix();
			//draw visor
			glPushMatrix();	  
				glTranslatef(0.0,0.1,0.1);
				glScalef(0.5,0.5,0.5);
				drawShapeDispatcher(SPHERE,renderStyle,ColourOrange,Colourblack);
			glPopMatrix();
			//draw bottom head
			glPushMatrix();
				glTranslatef(0.0,-0.3,0.0);
				glScalef(0.6,-0.3,0.6);
				drawShapeDispatcher(TRAP,renderStyle,ColourNeonBlue,Colourblack);
			glPopMatrix();
			//draw back head
			glPushMatrix();	  
				glTranslatef(0.0,0.1,-0.4);
				glScalef(0.6,0.1,0.2);
				drawShapeDispatcher(CUBE,renderStyle,ColourNeonBlue,Colourblack);
			glPopMatrix();
			//draw left side head
			glPushMatrix();	  
				glTranslatef(-0.5,0.1,0.0);
				glScalef(0.1,0.1,0.6);
				drawShapeDispatcher(CUBE,renderStyle,ColourNeonBlue,Colourblack);
			glPopMatrix();
			//draw right side head
			glPushMatrix();	  
				glTranslatef(0.5,0.1,0.0);
				glScalef(0.1,0.1,0.6);
				drawShapeDispatcher(CUBE,renderStyle,ColourNeonBlue,Colourblack);
			glPopMatrix();
			
			//draw neck (rotation joint)
			glPushMatrix();
				glTranslatef(0.0,-0.7,0.0);
				glScalef(0.2,0.2,0.2);
				drawShapeDispatcher(SPHERE,renderStyle,ColourGold,Colourblack);
			glPopMatrix();
				
		glPopMatrix();
		
		//draw left arm-----------------------------------------------------------------------------------
		glPushMatrix();
			
			//Translating left arm to beside body		  
			glTranslatef(-1.1,1.2,0.0);
			//rotating the hip joint
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_SHOULDER_ROLL),
					  1.0, 0.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_SHOULDER_YAW),
					  0.0, 1.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_SHOULDER_PITCH),
					  0.0, 0.0, -1.0);
			//translate origin (rotation joint) to top of arm (shoulder)
			glTranslatef(-0.9,0.0,0.0);
			glPushMatrix();
			    //scale to right proportions
				glScalef(0.5,0.3,0.3);
				drawShapeDispatcher(CUBE,renderStyle,ColourFirebrick,Colourblack);
			glPopMatrix();
			
			//draw shoulder
			glPushMatrix();
				//translate to proper position (gotta subtract the origin movement from 
				//x now since the rotations messed up the axes
				glTranslatef(0.9,0.0,0.0);
				glScalef(0.5,0.5,0.5);
				drawShapeDispatcher(SPHERE,renderStyle,ColourGold,Colourblack);
			glPopMatrix();
			
			//draw left forearm
			glPushMatrix();
				//translate to proper position (gotta subtract the origin movement from 
				//x now since the rotations messed up the axes
				glTranslatef(-0.7,0.0,0.0);
				//left forearm rotation
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_ELBOW_ROLL),
						  -1.0, 0.0, 0.0);
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_ELBOW_YAW),
						  0.0, -1.0, 0.0);
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_ELBOW_PITCH),
						  0.0, 0.0, -1.0);
				//translate origin to correct position
				glTranslatef(-0.7,-0.0,0.0);
				glPushMatrix();
					glScalef(0.5,0.2,0.2);
					drawShapeDispatcher(CUBE,renderStyle,ColourFirebrick,Colourblack);
				glPopMatrix();
				//draw elbow
				glPushMatrix();
					glTranslatef(0.7,0.0,0.0);
					glScalef(0.3,0.3,0.3);
					drawShapeDispatcher(SPHERE,renderStyle,ColourGold,Colourblack);
				glPopMatrix();
			glPopMatrix();
			
		glPopMatrix();
		
		//draw right arm-----------------------------------------------------------------------------------
		glPushMatrix();
			
			//Translating right arm to beside body		  
			glTranslatef(1.1,1.2,0.0);
			//rotating the hip joint
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_ROLL),
					  1.0, 0.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_YAW),
					  0.0, 1.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_PITCH),
					  0.0, 0.0, 1.0);
			//translate origin (rotation joint) to top of arm (shoulder)
			glTranslatef(-0.9,0.0,0.0);
			glPushMatrix();
			    //scale to right proportions
				glScalef(0.5,0.3,0.3);
				glTranslatef(3.6,0.0,0.0);//moved the arm from left side to right
				drawShapeDispatcher(CUBE,renderStyle,ColourFirebrick,Colourblack);
			glPopMatrix();
			
			//draw shoulder
			glPushMatrix();
				//translate to proper position (gotta subtract the origin movement from 
				//x now since the rotations messed up the axes
				glTranslatef(0.9,0.0,0.0);
				glScalef(0.5,0.5,0.5);
				drawShapeDispatcher(SPHERE,renderStyle,ColourGold,Colourblack);
			glPopMatrix();
			
			//draw right forearm
			glPushMatrix();
				//translate to proper position (gotta subtract the origin movement from 
				//x now since the rotations messed up the axes
				glTranslatef(2.5,0.0,0.0);
				//right forearm rotation
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_ELBOW_ROLL),
						  1.0, 0.0, 0.0);
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_ELBOW_YAW),
						  0.0, 1.0, 0.0);
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_ELBOW_PITCH),
						  0.0, 0.0, 1.0);
				//translate origin to correct position
				glTranslatef(0.7,-0.0,0.0);
				glPushMatrix();
					glScalef(0.5,0.2,0.2);
					drawShapeDispatcher(CUBE,renderStyle,ColourFirebrick,Colourblack);
				glPopMatrix();
				//draw elbow
				glPushMatrix();
					glTranslatef(-0.7,0.0,0.0);
					glScalef(0.3,0.3,0.3);
					drawShapeDispatcher(SPHERE,renderStyle,ColourGold,Colourblack);
				glPopMatrix();
			glPopMatrix();
			
		glPopMatrix();
				
		//draw left leg-----------------------------------------------------------------------------------
		glPushMatrix();
			//Translating leg to bottom of body		  
			glTranslatef(-0.4,-1.1,0.0);
			//rotating the hip joint
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_HIP_ROLL),
					  1.0, 0.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_HIP_YAW),
					  0.0, 1.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_HIP_PITCH),
					  0.0, 0.0, 1.0);
			//translate origin (rotation joint) to top of leg (hip)
			glTranslatef(0.0,-1.4,0.0);
			glPushMatrix();
			    //scale to right proportions
				glScalef(0.4,1.1,0.4);
				drawShapeDispatcher(CYLINDER,renderStyle,Colourdarkgreen,Colourblack);
			glPopMatrix();
			
			//draw hip
			glPushMatrix();
				//translate to proper position (gotta subtract the origin movement from 
				//x now since the rotations messed up the axes
				glTranslatef(0.0,1.4,0.0);
				glScalef(0.4,0.4,0.4);
				drawShapeDispatcher(SPHERE,renderStyle,ColourGold,Colourblack);
			glPopMatrix();
		
		//draw left shin
			glPushMatrix();
				//translate to proper position (gotta subtract the origin movement from 
				//x now since the rotations messed up the axes
				glTranslatef(0.0,-0.3,0.0);
				//knee rotation
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_KNEE_ROLL),
						  1.0, 0.0, 0.0);
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_KNEE_YAW),
						  0.0, 1.0, 0.0);
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_KNEE_PITCH),
						  0.0, 0.0, 1.0);
				//translate origin to correct position
				glTranslatef(0.0,-1.5,0.0);
				glPushMatrix();
					glScalef(0.4,1.2,0.4);
					drawShapeDispatcher(CONE,renderStyle,ColourNewMidnightBlue,Colourblack);
				glPopMatrix();
				//draw knee sphere
				glPushMatrix();
					glTranslatef(0.0,1.5,0.0);
					glScalef(0.4,0.4,0.4);
					drawShapeDispatcher(SPHERE,renderStyle,ColourGold,Colourblack);
				glPopMatrix();
				
				//draw left foot
				glPushMatrix();
					//left foot rotation
					glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_FOOT_ROLL),
							  1.0, 0.0, 0.0);
					glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_FOOT_YAW),
							  0.0, 1.0, 0.0);
					glRotatef(*joint_ui_data->getDOFPtr(Keyframe::L_FOOT_PITCH),
							  0.0, 0.0, 1.0);
					glTranslatef(0.3,-0.3,0.0);
					//foot cylinder
					glPushMatrix();
						glRotatef(90,0.0,0.0,1.0);
						glScalef(0.3,0.6,0.3);
						drawShapeDispatcher(CYLINDER,renderStyle,ColourSienna,Colourblack);
					glPopMatrix();
					//foot triangle
					glPushMatrix();
						glTranslatef(-0.3,0.0,0.1);
						glRotatef(90,0.0,1.0,0.0);
						glScalef(0.6,0.3,0.3);
						drawTriangleDispatcher(renderStyle,ColourSienna,Colourblack);
					glPopMatrix();
				glPopMatrix();
				
			glPopMatrix();
			
		glPopMatrix();
		
		//draw right leg-----------------------------------------------------------------------------------
		glPushMatrix();
			//Translating leg to bottom of body		  
			glTranslatef(0.4,-1.1,0.0);
			//rotating the hip joint
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_HIP_ROLL),
					  1.0, 0.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_HIP_YAW),
					  0.0, 1.0, 0.0);
			glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_HIP_PITCH),
					  0.0, 0.0, 1.0);
			//translate origin (rotation joint) to top of leg (hip)
			glTranslatef(0.0,-1.4,0.0);
			glPushMatrix();
			    //scale to right proportions
				glScalef(0.4,1.1,0.4);
				drawShapeDispatcher(CYLINDER,renderStyle,Colourdarkgreen,Colourblack);
			glPopMatrix();
			
			//draw hip
			glPushMatrix();
				//translate to proper position (gotta subtract the origin movement from 
				//x now since the rotations messed up the axes
				glTranslatef(0.0,1.4,0.0);
				glScalef(0.4,0.4,0.4);
				drawShapeDispatcher(SPHERE,renderStyle,ColourGold,Colourblack);
			glPopMatrix();
		
		//draw right shin
			glPushMatrix();
				//translate to proper position (gotta subtract the origin movement from 
				//x now since the rotations messed up the axes
				glTranslatef(0.0,-0.3,0.0);
				//knee rotation
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_KNEE_ROLL),
						  1.0, 0.0, 0.0);
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_KNEE_YAW),
						  0.0, 1.0, 0.0);
				glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_KNEE_PITCH),
						  0.0, 0.0, 1.0);
				//translate origin to correct position
				glTranslatef(0.0,-1.5,0.0);
				glPushMatrix();
					glScalef(0.4,1.2,0.4);
					drawShapeDispatcher(CONE,renderStyle,ColourNewMidnightBlue,Colourblack);
				glPopMatrix();
				//draw knee sphere
				glPushMatrix();
					glTranslatef(0.0,1.5,0.0);
					glScalef(0.4,0.4,0.4);
					drawShapeDispatcher(SPHERE,renderStyle,ColourGold,Colourblack);
				glPopMatrix();
				
				//draw right foot
				glPushMatrix();
					//right foot rotation
					glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_FOOT_ROLL),
							  1.0, 0.0, 0.0);
					glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_FOOT_YAW),
							  0.0, 1.0, 0.0);
					glRotatef(*joint_ui_data->getDOFPtr(Keyframe::R_FOOT_PITCH),
							  0.0, 0.0, 1.0);
					glTranslatef(0.3,-0.3,0.0);		  
					//foot cylinder
					glPushMatrix();
						glRotatef(90,0.0,0.0,1.0);
						glScalef(0.3,0.6,0.3);
						drawShapeDispatcher(CYLINDER,renderStyle,ColourSienna,Colourblack);
					glPopMatrix();
					//foot triangle
					glPushMatrix();
						glTranslatef(-0.3,0.0,0.1);
						glRotatef(90,0.0,1.0,0.0);
						glScalef(0.6,0.3,0.3);
						drawTriangleDispatcher(renderStyle,ColourSienna,Colourblack);
					glPopMatrix();
				glPopMatrix();
				
			glPopMatrix();
		glDisable(GL_COLOR_MATERIAL);	
		glPopMatrix();
		
	glPopMatrix();
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
	else if (button == GLUT_LEFT_BUTTON)
	{
		if( state == GLUT_DOWN )
		{
			lastX = x;
			lastY = y;
			updateAngleonLeftButton = true;
		}
		else
		{
			updateAngleonLeftButton = false;
		}
	}
}

void keyboard(unsigned char key, int x, int y)
{
	animation_dir = key;
	keyStates[key] = true;
	switch (key){
		case 'w':
		case 's':
		case 'a': 
		case 'd': 
			loadKeyframesFromFileButton(MoveHorizontal);
				animateButton();  
			break;
		case 'i':
			MoveVertical +=0.1;
			break;
		case 'k':
			MoveVertical -=0.1;
			break;
		case 'j':
			MoveHorizontal-=0.1;
			break;
		case 'l':
			MoveHorizontal+=0.1;
			break;
		case 'z':
			Move3=-1;
			loadKeyframesFromFileButton(MoveHorizontal);
			animateButton(); 
			break;
		case 'x':
			Move3=1;
			loadKeyframesFromFileButton(MoveHorizontal);
			animateButton(); 
			break;
		case 'r':
			if (strafeEnabled == 1)
				strafeEnabled =0;
			else
				strafeEnabled =1;
 
		default:
			//animate_mode=0;
         break;
	}
	glutSetWindow(windowID);
		glutPostRedisplay();
	//printf("Horizontal %f\n",MoveHorizontal);
	//printf("MoveVertical %f\n",MoveVertical);
	//printf("camZPos %f\n",camZPos);
	//printf("animation dir %c\n",animation_dir);
	//printf("Move3 %f\n",Move3);
}

void keyboardUp(unsigned char key, int x, int y)
{
	animation_dir = key;
	keyStates[key] = false;
	switch (key){
		case 'w':
		case 's':
		case 'a': 
		case 'd': 
			loadKeyframesFromFileButton(MoveHorizontal);
			animateButton(); 
			break;
		case 'z':
		case 'x':
			Move3=0;
			loadKeyframesFromFileButton(MoveHorizontal);
			animateButton(); 
			break;
 
		default:
         break;
	}
	glutSetWindow(windowID);
		glutPostRedisplay();
	//printf("animation dir stop %c\n",animation_dir);
	//printf("Vertical %f\n",MoveVertical);
	//printf("Horizontal %f\n",MoveHorizontal);
	//printf("Move3 %f\n",Move3);
}

void keyboardSpecial(int key, int x, int y)
{
	switch (key){
		case GLUT_KEY_LEFT:
			camXPos -= 10*ZOOM_SCALE;
			break;
		case GLUT_KEY_RIGHT:
			camXPos += 10*ZOOM_SCALE;
			break;
		case GLUT_KEY_DOWN:
			camYPos -= 10*ZOOM_SCALE;
			break;
		case GLUT_KEY_UP:
			camYPos += 10*ZOOM_SCALE;
			break;
 
		default:
         break;
	}
	// Redraw the scene from updated camera position
	glutSetWindow(windowID);
	glutPostRedisplay();
	printf("camXPos %f\n",camXPos);
	printf("camYPos %f\n",camYPos);
}

// Handles mouse motion events while a button is pressed
void motion(int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if( updateCamZPos )
	{
		// Update camera z position
		camZPosTemp =camZPos + (x - lastX) * ZOOM_SCALE;
		printf("Horizontal %f\n",MoveHorizontal);
	printf("MoveVertical %f\n",MoveVertical);
	printf("camZPos %f\n",camZPos);
		if (/*(camZPosTemp >-100)&&*/(camZPosTemp<-7))//clamp the camera
			camZPos = camZPosTemp;
		//*joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_PITCH)
		//*joint_ui_data->getDOFPtr(Keyframe::LIGHTANGLE)+= (x - lastX);
		lastX = x;
		//*joint_ui_data->getDOFPtr(Keyframe::R_SHOULDER_ROLL)
		num_faces2+= (y - lastY);
		lastY = y;

		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
	if ( updateAngleonLeftButton )
	{
		//*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Y)+= (x - lastX);
		camXPosRotate += (x - lastX) ;
		//gcamXPosRotateAngle = x;
		lastX = x;
		//*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_X)+= (y - lastY);
		camYPosRotate += (y - lastY) ;
		lastY = y;
		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
}

void motionPassive(int x, int y)
{
		//*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Y)+= (x - lastX);
		camXPosRotate += (x - lastX) ;
		lastX = x;
		//*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_X)+= (y - lastY);
		camYPosRotate += (y - lastY) ;
		lastY = y;
		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
}

///////////////////////////////////////////////////////////
//
// DRAWING FUNCTIONS
//
///////////////////////////////////////////////////////////

//taken from
//http://stackoverflow.com/questions/13983189/opengl-how-to-calculate-normals-in-a-terrain-height-grid

float* normalize_per_vertex (unsigned char pHeightMap[], Vertex current_pt) {
	
 // # P.xy store the position for which we want to calculate the normals
  // # height() here is a function that return the height at a point in the terrain
	
	int x=0;
	//float y=0.0;
	int z=0;

	//Vertex vectorL={0};
	//Vertex vectorR={0};
	//Vertex vectorD={0};
	//Vertex vectorU={0};
	//Vertex vectorOff={0};
	
	//vectorOff[0] = STEP_SIZE;
	//vectorOff[1] = 0.0f;
	//vectorOff[2] = STEP_SIZE;
	//vectorOff[0] = 0.0f;
	
	x = current_pt[0];
	z = current_pt[2];
	
	//x=(int)current_pt[0];
	//z=(int)current_pt[1];										
	//y = Height(pHeightMap, x, z );
	
	//printf ("y is %f \n", y);
	
	float hL=Height(pHeightMap, x-normal_calc_step, z);
	float hR=Height(pHeightMap, x+normal_calc_step, z);
	float hD=Height(pHeightMap, x, z-normal_calc_step);
	float hU=Height(pHeightMap, x, z+normal_calc_step);
	
	normal [0] = hL-hR;
	normal [1] = 2.0f;
	normal [2] = hD-hU;

	
	float length = sqrt(normal[0]*normal[0]+
						normal[1]*normal[1]+
						normal[2]*normal[2]);
	
	if (length>0)
	{
		normal[0] /= length;
		normal[1] /= length;
		normal[2] /= length;
	}
	
	
	//printf ("x is %f \n", hL);
	//printf ("y is %f \n", normal[1]);
	//printf ("z is %f \n", normal[2]);
	
	return normal;
	
	//return vectorOff;
	
	/*
  // read neightbor heights using an arbitrary small offset
  vec3 off = vec3(1.0, 1.0, 0.0);
  float hL = height(P.xy - off.xz);
  float hR = height(P.xy + off.xz);
  float hD = height(P.xy - off.zy);
  float hU = height(P.xy + off.zy);

  // deduce terrain normal
  N.x = hL - hR;
  N.y = hD - hU;
  N.z = 2.0;
  N = normalize(N);
  
  return N;
  * */
	
	
}




float* normalize (Vertices face)
{
	Vertex vector1={0};
	Vertex vector2={0};
	//0=x 1=y 2=z
	vector1[0]=face[1][0]-face[0][0];//B.x-A.x
	vector1[1]=face[1][1]-face[0][1];//B.y-A.y
	vector1[2]=face[1][2]-face[0][2];//B.z-A.z
	vector2[0]=face[2][0]-face[0][0];//C.x-A.x
	vector2[1]=face[2][1]-face[0][1];//C.y-A.y
	vector2[2]=face[2][2]-face[0][2];//C.z-A.z
	
	normal[0] = vector1[1]*vector2[2] - vector1[2]*vector2[1];
	normal[1] = vector2[0]*vector1[2] - vector2[2]*vector1[0];
	normal[2] = vector1[0]*vector2[1] - vector1[1]*vector2[0];
	
	float length = sqrt(normal[0]*normal[0]+
						normal[1]*normal[1]+
						normal[2]*normal[2]);
	
	if (length>0)
	{
		normal[0] /= length;
		normal[1] /= length;
		normal[2] /= length;
	}
	
	return normal;
};

void drawCubeV(Vertices face)
{
	glBegin(GL_QUADS);
		glNormal3fv(normalize(face));
		glVertex3fv(face[0]);
		glVertex3fv(face[1]);
		glVertex3fv(face[2]);
		glVertex3fv(face[3]);
    glEnd();
}

void drawCubeVTexture(Vertices face)
{
	GLuint tex0 = raw_texture_load (backfile, MAP_SIZE, MAP_SIZE, 1);
	glActiveTexture(GL_TEXTURE0);
    glEnable( GL_TEXTURE_2D );		
	glBindTexture( GL_TEXTURE_2D, tex0);
	glBegin(GL_QUADS);
		glNormal3fv(normalize(face));
		glTexCoord2i( 0, 0);
		glVertex3fv(face[0]);
		glTexCoord2i( 0, 1);
		glVertex3fv(face[1]);
		glTexCoord2i( 1, 1);
		glVertex3fv(face[2]);
		glTexCoord2i( 1, 0);
		glVertex3fv(face[3]);
    glEnd();
    glActiveTexture (GL_TEXTURE0);
	glDisable (GL_TEXTURE_2D);
	glDeleteTextures(1, &tex0);
}

void drawTriangleV(Vertices face)
{
	glBegin(GL_TRIANGLES);
	    glNormal3fv(normalize(face));
		glVertex3fv(face[0]);
		glVertex3fv(face[1]);
		glVertex3fv(face[2]);
    glEnd();
}

void drawCubeVOutline(Vertices face)
{
	glBegin(GL_LINE_LOOP);
		glNormal3fv(normalize(face));
		glVertex3fv(face[0]);
		glVertex3fv(face[1]);
		glVertex3fv(face[2]);
		glVertex3fv(face[3]);
    glEnd();
}

void drawTriangleVOutline(Vertices face)
{
	glBegin(GL_LINE_LOOP);
		glVertex3fv(face[0]);
		glVertex3fv(face[1]);
		glVertex3fv(face[2]);
    glEnd();
}



void drawMoonSphere(){
	GLUquadric *quad;
	quad = gluNewQuadric();
	GLuint tex0 = raw_texture_load (moon_file, MOON_SIZE, MOON_SIZE, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glEnable( GL_TEXTURE_2D );	
	glBindTexture( GL_TEXTURE_2D, tex0);

	gluQuadricTexture(quad,1);
	glBegin(GL_QUADS);
	//glutSolidSphere(quad, 1,50,50);	
	gluSphere(quad, 1,50,50);
	glEnd();	
	
	glActiveTexture (GL_TEXTURE0);
	glDisable (GL_TEXTURE_2D);
	glDeleteTextures(1, &tex0);

}

void drawSphere(double r, int lats, int longs) {
    int i, j;
    for(i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0  = sin(lat0);
        double zr0 =  cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

		/*
		GLuint tex0 = raw_texture_load ("texture/mars.raw", MAP_SIZE, MAP_SIZE);
		glActiveTexture(GL_TEXTURE0);
		glEnable( GL_TEXTURE_2D );	

		*/

        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double) (j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);
			
			//texture_sun(0, 1); 
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            //texture_sun(1, 0); 
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
        /*
		glActiveTexture (GL_TEXTURE0);
		glDisable (GL_TEXTURE_2D);
		glDeleteTextures(1, &tex0);
		*/
    }
}
void drawSphereOutline(double r, int lats, int longs) {
    int i, j;
    for(i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0  = sin(lat0);
        double zr0 =  cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_LINE_LOOP);
        for(j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double) (j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
}

void drawCylinder(float base, float top, float height, int slices, int stacks)
{
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	gluQuadricDrawStyle(quadratic, GLU_FILL);
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	glPushMatrix();
		glRotatef(-90,1.0,0.0,0.0);
		gluCylinder(quadratic,base,top,height,slices,stacks);
		gluDisk(quadratic,0,base,slices,stacks);
		glPushMatrix();
			glTranslatef(0.0,0.0,1.0);
			gluDisk(quadratic,0,top,slices,stacks);
		glPopMatrix();
	glPopMatrix();
	
}

void drawCylinderOutline(float base, float top, float height, int slices, int stacks)
{
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	gluQuadricDrawStyle(quadratic, GLU_LINE);
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	glPushMatrix();
		glRotatef(-90,1.0,0.0,0.0);
		gluCylinder(quadratic,base,top,height,slices,stacks);
	glPopMatrix();
}

void drawShapeDispatcher(int shape, int type, GLfloat *fillColour, GLfloat *outlineColour)
{
	int i= 0;
	if (type == WIREFRAME)
	{
		glColor3fv(outlineColour);
		 if (shape == SPHERE)
			drawSphereOutline(1.0, 10, 10);
		else if (shape == CYLINDER)
			drawCylinderOutline(1.0, 1.0, 1.0, 10, 1);
		else if (shape == CONE)
			drawCylinderOutline(0.3, 1.0, 1.0, 10, 1);
		else if (shape == SHARPCONE)
			drawCylinderOutline(1.0, 0.0, 1.0, 10, 1);
		else
		{
			for (i=0;i<TOTAL_FACES_CUBE;i++)
				{
					if (shape == CUBE)
						drawCubeVOutline(facesCube[i]);
					else if (shape == TRAP)
						drawCubeVOutline(facesTrap[i]);
				}
		}
	}
	else if (type == SOLID)
	{
		glColor3fv(fillColour);
		if (shape == SPHERE)
					drawSphere(1.0, 10, 10);
		else if (shape == CYLINDER)
			drawCylinder(1.0, 1.0, 1.0, 10, 1);
		else if (shape == CONE)
			drawCylinder(0.3, 1.0, 1.0, 10, 1);
		else if (shape == SHARPCONE)
			drawCylinder(1.0, 0.0, 1.0, 10, 1);
		else
		{			
			for (i=0;i<TOTAL_FACES_CUBE;i++)
				{
					if (shape == CUBE)
						drawCubeV(facesCube[i]);
					if (shape == CUBETEXTURE)
						drawCubeVTexture(facesCube[i]);
					else if (shape == TRAP)
						drawCubeV(facesTrap[i]);
				}
		}
	}
	else if ((type == OUTLINED)||(type==METALLIC)||(type==MATTE))
	{
		drawShapeDispatcher(shape, SOLID, fillColour, outlineColour);
		//drawShapeDispatcher(shape, WIREFRAME, fillColour, outlineColour);
	}
}

void drawTriangleVHelper()
{
	drawTriangleV(facesTriangle[0]);
	drawTriangleV(facesTriangle[1]);
	drawCubeV(facesTriangle[2]);
	drawCubeV(facesTriangle[3]);
	drawCubeV(facesTriangle[4]);
}

void drawTriangleVOutlineHelper()
{
	drawTriangleVOutline(facesTriangle[0]);
	drawTriangleVOutline(facesTriangle[1]);
	drawCubeVOutline(facesTriangle[2]);
	drawCubeVOutline(facesTriangle[3]);
	drawCubeVOutline(facesTriangle[4]);
}

void drawTriangleDispatcher(int type, GLfloat *fillColour, GLfloat *outlineColour)
{
	if (type == WIREFRAME)
	{
				glColor3fv(outlineColour);
				drawTriangleVOutlineHelper();
	}
	else if (type == SOLID)
	{
				glColor3fv(fillColour);
				drawTriangleVHelper();

	}
	else if ((type == OUTLINED)||(type==METALLIC)||(type==MATTE))
	{
				glColor3fv(fillColour);
				drawTriangleVHelper();
				//glColor3fv(outlineColour);
				//drawTriangleVOutlineHelper();
	}
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

void print(float x, float y,int z, GLfloat *fillColour, char *string, void* font)
{
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glTranslatef(x,y, z);
		glColor3fv(fillColour);
		//set the position of the text in the window using the x and y coordinates
		glRasterPos3f(0,0,0);
		//get the length of the string to display
		int len = (int) strlen(string);
		//loop to display character by character
		for (int i = 0; i < len; i++)
		{
			glutBitmapCharacter(font,string[i]);
		}
		glEnable(GL_LIGHTING);
	glPopMatrix();
};

//////////////////////height mapping helper functions/////////////////////////////////////////////
////////////////
///////////////
////////////////
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

	//fread( pHeightMap, 1, nSize, pFile );
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
	
	
	//heightmap stores values ranging from 0 to 256
	//height map must contain 8 bit values
	return pHeightMap[x + (y * MAP_SIZE)];				
}

 GLuint raw_texture_load(const char *filename, int width, int height, int is_back)
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
 
	if (is_back==1) {
		//for background box
		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	} 
	else {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);
		     // when texture area is small, bilinear filter the closest mipmap
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first mipmap
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
	}
	
	//if (is_back==2) {
		//for sphere cube mapping
		//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//} else {
		// texture should tile
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//}
	
     // build our texture mipmaps
     gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
 
     // free buffer
     free(data);
 
     return texture;
 }

void SetVertexColor(int height, int x, int y)	/* Sets The Color Value For A Particular Index, Depending On The Height Index */
{
	if(!height)
		return;								/* Make Sure Our Height Data Is Valid */
	else
	{
		//float fColor = -0.15f + (Height(pHeightMap, x, y ) / 256.0f);
		/* Assign This Blue Shade To The Current Vertex */
		
		//glColor3f(fColor, fColor , fColor);
		
		//0=stone - high
		//1=grass - mid
		//2=water - low
		
		/*	
		int low_mid_border=0;
		int mid_high_border=0;
		int high_transition_zone=0;
		int low_transition_zone=0;
		//int height = Height(pHeightMap, x, y);
		
		
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
				g_fContributionOfTex1=0.0;
				g_fContributionOfTex2=1.0;
			}
				
		}
		* */
		
		g_fContributionOfTex0=0.0;
		g_fContributionOfTex1=0.5;
		g_fContributionOfTex2=0.0;
		
		
		//Calculate contribution of each of the textures
		//textures must add up to 1
		//float rgbValue   = g_fContributionOfTex0 / (1.0f - g_fContributionOfTex2);
		float rgbValue   = 1.0f;
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


void texture_sun (float x, float z){
			
		glTexCoord2f(x, z);
	
}



 void texture_terrain (float x, float z){
			
		//glTexCoord2f(x/(MAP_SIZE/MAP_REPEAT), z/(MAP_SIZE/MAP_REPEAT));
		glTexCoord2f(x/(FACE_SIZE/MAP_REPEAT), z/(FACE_SIZE/MAP_REPEAT));
		//glTexCoord2f(x, z);
		
		//glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
		//glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
		//glColor4f( 1.0f, 1.0f, 1.0f, 0.2f);
		//glMultiTexCoord2f(GL_TEXTURE0, x/(FACE_SIZE/MAP_REPEAT), z/(FACE_SIZE/MAP_REPEAT));
		//glColor4f( 1.0f, 1.0f, 1.0f, 1.0f);
		//glMultiTexCoord2f(GL_TEXTURE1, x/(FACE_SIZE/MAP_REPEAT), z/(FACE_SIZE/MAP_REPEAT));
		
		
		//glMultiTexCoord2fARB( GL_TEXTURE0_ARB,x/(FACE_SIZE/MAP_REPEAT), z/(FACE_SIZE/MAP_REPEAT) );
		//glMultiTexCoord2fARB( GL_TEXTURE1_ARB, x/(FACE_SIZE/MAP_REPEAT), z/(FACE_SIZE/MAP_REPEAT));
		//glMultiTexCoord2fARB( GL_TEXTURE2_ARB, x/(FACE_SIZE/MAP_REPEAT), z/(FACE_SIZE/MAP_REPEAT) );
	
}

//draw background
void drawBackground() 
{
	
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(0, 1, 0, 1, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	// No depth buffer writes for background.
	glDepthMask( false );



	GLuint tex0 = raw_texture_load (backfile, BACK_SIZE_X, BACK_SIZE_Y, 1);
	
	glActiveTexture(GL_TEXTURE0);
	glEnable( GL_TEXTURE_2D );
	//glDisable(GL_LIGHTING);	
	
	glBindTexture( GL_TEXTURE_2D, tex0);
	
	glBegin(GL_QUADS); 
	  glTexCoord2i( 0, 0);
	  glVertex2i( 0, 0 );
	  glTexCoord2i( 0, 1 );
	  glVertex2i( 0, 1 );
	  glTexCoord2i( 1, 1 );
	  glVertex2i( 1, 1 );
	  glTexCoord2i( 1, 0 );
	  glVertex2i( 1, 0 );
	 
	glEnd();

	glActiveTexture (GL_TEXTURE0);
	glDisable (GL_TEXTURE_2D);
	glDeleteTextures(1, &tex0);
	glDepthMask( true );

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}



//taken from NEHE
//multitexturing code taken from below:
//http://www.codesampler.com/oglsrc/oglsrc_4.htm
void drawHeightMappedPlane(unsigned char pHeightMap[],const char *ter_tex, const char *lev1_tex ){

	int X = 0, Y = 0;									
	float x, y=0.0f, z;		
	//temp variables to hold cube mapping 				
	float x_c, y_c, z_c;
	if(!pHeightMap) return;							
	
	
	//borders to switch b/w textures
	//float low_mid_border=0.3f;
	float mid_high_border=0.3f;
	float high_border=0.5f;
	
	//int orient=0;// value to be passed onto texture mapping so that different faces are textured accordingly
	
	/*
	if (world_id == 0) {
		GLuint tex0 = raw_texture_load (tex0file, MAP_SIZE, MAP_SIZE, 0);
	} else if (world_id = 1) {
		GLuint tex0 = raw_texture_load (tex1file, MAP_SIZE, MAP_SIZE, 0);
	} else {
		GLuint tex0 = raw_texture_load (tex0file, MAP_SIZE, MAP_SIZE, 0);
	}
	*/
	

	GLuint tex0 = raw_texture_load (ter_tex, MAP_SIZE, MAP_SIZE, 0);
	GLuint tex1 = raw_texture_load (lev1_tex, MAP_SIZE, MAP_SIZE, 0);
	GLuint tex2 = raw_texture_load (lev1_tex, MAP_SIZE, MAP_SIZE, 0);


	//terrain lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, goldspec);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, ter_diff);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, goldambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, goldshine);
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	glLightfv(GL_LIGHT0, GL_SPECULAR, def_spec);
	glLightfv(GL_LIGHT0, GL_AMBIENT, def_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, def_dif);

	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	
	
	for ( X = 0; X < MAP_SIZE; X += STEP_SIZE )
	{
		for ( Y = 0; Y < FACE_SIZE*3; Y += STEP_SIZE )
		{
			
			//make array to hold vertices of the plane
			Vertices temp = {0};  //hold current vertices
			Vertices temp_c={0}; //for cube mapping
			Vertices temp_t={0}; //trasnlation
			
			
			/* Get The (X, Y, Z) Value For The Bottom Left Vertex */
			x = X;							
			if (is_height==1) {y = Height(pHeightMap, X, Y );}
			z = Y;

			//store into vertices array
			temp [0][0]= x;
			temp [0][1]= y;
			temp [0][2]= z;
			//SetVertexColor(y, x, z);

			/* Get The (X, Y, Z) Value For The Top Left Vertex */
			x = X;										
			if (is_height==1) {y = Height(pHeightMap, X, Y + STEP_SIZE );}
			z = Y + STEP_SIZE ;
			
			//store into vertices array
			temp [1][0]= x;
			temp [1][1]= y;
			temp [1][2]= z;
			//SetVertexColor(y, x, z);		

			/* Get The (X, Y, Z) Value For The Top Right Vertex */
			x = X + STEP_SIZE;
			if (is_height==1) {y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE );}
			z = Y + STEP_SIZE ;
			
			//store into vertices array
			temp [2][0]= x;
			temp [2][1]= y;
			temp [2][2]= z;
			//SetVertexColor(y, x, z);							
			
			/* Get The (X, Y, Z) Value For The Bottom Right Vertex */
			x = X + STEP_SIZE;
			if (is_height==1) {y = Height(pHeightMap, X + STEP_SIZE, Y );} 
			z = Y;
			
			//store into vertices array
			temp [3][0]= x;
			temp [3][1]= y;
			temp [3][2]= z;
			//SetVertexColor(y, x, z);	
			

/*
////////////////////////////TEXTURE ATTEMPT 2////////////////////////////
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
    
    
    ////////////////////////////////////////////////////////////////////////////////////	
 */
    		//basic 1 texture
		
			glActiveTexture (GL_TEXTURE0);
			glEnable (GL_TEXTURE_2D);
			glBindTexture( GL_TEXTURE_2D, tex0);
			
			
			//note: trasnforms need to changer according to step size
			//faces:
			//x,3,x,x
			//5,1,2,6
			//x,4,x,x
			//turn plane to cube
			
			//store the original temp
			for (int i=0; i <4; i ++){
				for (int k=0; k<3; k++){
	
					temp_orig[i][k] = temp[i][k];
					
				}
			}
			
			//center the terrain cube
	
			for (int i=0; i <4; i ++){
				for (int k=0; k<3; k++){
					if (k == 0) {
						temp[i][k] -= (FACE_SIZE/2)*3;
						//temp[i][k] -= 0;
					}
					else if (k == 1) {
						temp[i][k] += (FACE_SIZE/2);
					}
					else if (k == 2) {
						temp[i][k] -= (FACE_SIZE/2)*3;

					}
					
				}
			}

			//face 1, top face
			if ( X >= FACE_SIZE && X<=FACE_SIZE*2-1 && Y >=FACE_SIZE && Y<=FACE_SIZE*2-1) {
				terrain_draw (tex0, tex1, temp, pHeightMap,0, temp_orig);
			}
			//face6, bottom face
			else if ( X >= FACE_SIZE*3 && X<=MAP_SIZE-1 && Y >=FACE_SIZE  && Y<=FACE_SIZE*2-1) {
				for (int i=0; i<4; i++){
					//now rotate around z axis by 180
					//x
					temp_c[i][0]=cos(M_PI)*temp[i][0]-sin(M_PI)*temp[i][1];
					//temp_c[i][0]=temp[i][0];
					//y 
					//temp_c[i][1]=cos(M_PI)*temp[i][1]-sin(M_PI)*temp[i][2];
					temp_c[i][1]=sin(M_PI)*temp[i][0]+cos(M_PI)*temp[i][1];
					//z
					//temp_c[i][2]=sin(M_PI)*temp[i][1]+cos(M_PI)*temp[i][2];
					temp_c[i][2]=temp[i][2];
					
					//translate first
					//x, protrudes
					temp_t[i][0]=temp_c[i][0]+FACE_SIZE*2;
					//y
					temp_t[i][1]=temp_c[i][1];
					//z, protrudes
					temp_t[i][2]=temp_c[i][2];
				}
				terrain_draw (tex0, tex1,temp_t, pHeightMap,0,temp_orig);
			}
			//face2, left
			else if ( X >= FACE_SIZE*2 && X<=FACE_SIZE*3-1 && Y >=FACE_SIZE  && Y<=FACE_SIZE*2-1) {
				for (int i=0; i<4; i++){
					//now rotate 90 deg about z axis
					//x becomes y
					temp_c[i][0]=cos(-M_PI/2)*temp[i][0]-sin(-M_PI/2)*temp[i][1];
					//y 
					temp_c[i][1]=sin(-M_PI/2)*temp[i][0]+cos(-M_PI/2)*temp[i][1];
					//z
					temp_c[i][2]=temp[i][2];
					
					//translate first
					//x, protrudes
					temp_t[i][0]=temp_c[i][0];
					//y
					temp_t[i][1]=temp_c[i][1]+FACE_SIZE;
					//z, protrudes
					temp_t[i][2]=temp_c[i][2];
				}
				terrain_draw (tex0, tex1,temp_t, pHeightMap,1,temp_orig); 
			}
			//face5, right
			else if ( X >= 0 && X<=FACE_SIZE && Y >=FACE_SIZE  && Y<=FACE_SIZE*2-1) {
				for (int i=0; i<4; i++){
					//now rotate
					//
					temp_c[i][0]=cos(M_PI/2)*temp[i][0]-sin(M_PI/2)*temp[i][1];
					//y 
					temp_c[i][1]=sin(M_PI/2)*temp[i][0]+cos(M_PI/2)*temp[i][1];
					//z
					temp_c[i][2]=temp[i][2];
					
					//translate first
					//x, protrudes
					temp_t[i][0]=temp_c[i][0];
					//y
					temp_t[i][1]=temp_c[i][1]+FACE_SIZE;
					//z, protrudes
					temp_t[i][2]=temp_c[i][2];
				}
				terrain_draw (tex0, tex1,temp_t, pHeightMap,1, temp_orig);
			}
			//face4, back
			else if ( X >= FACE_SIZE && X<=FACE_SIZE*2-1 && Y >=0  && Y<=FACE_SIZE-1) {
				for (int i=0; i<4; i++){
					//now rotate about x axis
					//x becomes y
					temp_c[i][0]=temp[i][0];
					//y 
					temp_c[i][1]=cos(-M_PI/2)*temp[i][1]-sin(-M_PI/2)*temp[i][2];
					//z
					temp_c[i][2]=sin(-M_PI/2)*temp[i][1]+cos(-M_PI/2)*temp[i][2];;
					
					//translate first
					//x, protrudes
					temp_t[i][0]=temp_c[i][0];
					//y
					temp_t[i][1]=temp_c[i][1]+FACE_SIZE;
					//z
					temp_t[i][2]=temp_c[i][2];
				}
				terrain_draw (tex0, tex1,temp_t, pHeightMap,2,temp_orig); 
			}
			//face3, front
			else if ( X >= FACE_SIZE && X<=FACE_SIZE*2-1 && Y >=FACE_SIZE*2  && Y<=FACE_SIZE*3-1) {
				for (int i=0; i<4; i++){
					//now rotate about x axis
					//x becomes y
					temp_c[i][0]=temp[i][0];
					//y 
					temp_c[i][1]=cos(M_PI/2)*temp[i][1]-sin(M_PI/2)*temp[i][2];
					//z
					temp_c[i][2]=sin(M_PI/2)*temp[i][1]+cos(M_PI/2)*temp[i][2];;
					
					//translate first
					//x, protrudes
					temp_t[i][0]=temp_c[i][0];
					//y
					temp_t[i][1]=temp_c[i][1]+FACE_SIZE;
					//z
					temp_t[i][2]=temp_c[i][2];
				}
				terrain_draw (tex0, tex1,temp_t, pHeightMap,2,temp_orig); 
			}
			
			
			//keep record of all of the world vertices and normals for each bot_left point
			for (int i=0; i <3; i ++){
					world_vert_record[(X/STEP_SIZE)][(Y/STEP_SIZE)][i] =  world_vert[0][i];
					world_norm_record[(X/STEP_SIZE)][(Y/STEP_SIZE)][i] =  world_norm[0][i];
				}
			
			
		}
	}
	
	
	glDisable( GL_BLEND );
	glActiveTexture (GL_TEXTURE0);
	glDisable (GL_TEXTURE_2D);
	glActiveTexture (GL_TEXTURE1);
	glDisable (GL_TEXTURE_2D);
	glActiveTexture (GL_TEXTURE2);
	glDisable (GL_TEXTURE_2D);
	glDeleteTextures(1, &tex0);
	glDeleteTextures(1, &tex1);
	glDeleteTextures(1, &tex2);
	//reset the color to white
	
	
	/*
	glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &tex0);
	glDeleteTextures(1, &tex1);
	glDeleteTextures(1, &tex2);
	*/
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	
	glDisable (GL_COLOR_MATERIAL);

	
}

float* heightmap_sphere (Vertex sphere_vertices, Vertex normal_vertices, Vertex orig_plane_vertex){

	float height=0.0f;
	float x=(sphere_vertices[0]);
	float y=(sphere_vertices[1]);
	float z=(sphere_vertices[2]);
	
	float x_n=(normal_vertices[0]);
	float y_n=(normal_vertices[1]);
	float z_n=(normal_vertices[2]);
	
	int X = (int)orig_plane_vertex[0];
	int Y = (int)orig_plane_vertex[2];
	
	
	//apply the heightmap factor now
	
	//customize heightmap usage here
	height = orig_plane_vertex[1]*heightmap_factor;
	
	/*
	if ( X >= FACE_SIZE*3 && X<=MAP_SIZE-1 && Y >=FACE_SIZE  && Y<=FACE_SIZE*2-1){
		returner[0]=x-x_n*height;
		returner[1]=y-y_n*height;
		returner[2]=z-z_n*height;
	}
	else if ( X >= FACE_SIZE && X<=FACE_SIZE*2-1 && Y >=0  && Y<=FACE_SIZE-1) {
		returner[0]=x-x_n*height;
		returner[1]=y-y_n*height;
		returner[2]=z-z_n*height;
	}
	else {
		returner[0]=x+x_n*height;
		returner[1]=y+y_n*height;
		returner[2]=z+z_n*height;
	}
	*/

	returner[0]=x+x_n*height;
	returner[1]=y+y_n*height;
	returner[2]=z+z_n*height;
	
	return returner;
	
	
	
}



//http://www.cs.cmu.edu/~ajw/s2007/0251-SphericalWorlds.pdf
float* convert_cube_to_sphere (Vertex cube_vertices, Vertex orig_plane_vertex){
	
	/*
	float x=(cube_vertices[0]-384)/(MAP_SIZE/8);
	float y=(cube_vertices[1]+128)/(MAP_SIZE/8);
	float z=(cube_vertices[2]-640)/(MAP_SIZE/8);
	*/
	
	
	float x=(cube_vertices[0]);
	float y=(cube_vertices[1]);
	float z=(cube_vertices[2]);
	
	//from the height map
	
	
	/*
	float dx = x * sqrtf(1.0 - (y*y/2.0) - (z*z/2.0) + (y*y*z*z/3.0));
	float dy = y * sqrtf(1.0 - (z*z/2.0) - (x*x/2.0) + (z*z*x*x/3.0));
	float dz = z * sqrtf(1.0 - (x*x/2.0) - (y*y/2.0) + (x*x*y*y/3.0));
	*/
	
	//use normalizing method instead; from here (don't need to be from -1 to 1)
	//http://answers.unity3d.com/questions/357993/mapping-a-cube-to-a-sphere.html
	
	float length = sqrtf(x*x + y*y + z*z);
	float sx = x / length;
	float sy = y / length;
	float sz = z / length;
	
	
	//return the spheical coordinate basics, as well as the normals
	returner[0]=sx;
	returner[1]=sy;
	returner[2]=sz;
	
	
	return returner;
}

//temp orig contains vertices for the original plane height map
void terrain_draw (GLuint tex0, GLuint tex1, Vertices temp, unsigned char pHeightMap[], int orient, Vertices temp_orig) {
	
	orient=0;
	
	int tex_xi=0;
	int tex_yi=0;
	//face1 and face6 texturing
	if (orient ==0 ){
		tex_xi=0;
		tex_yi=2;
	}
	//face 5, face 2
	if (orient ==1 ){
		tex_xi=1;
		tex_yi=2;
	}
	//face 3, face 4
	if (orient ==2 ){
		tex_xi=0;
		tex_yi=1;
	}
	
	
	//convert cube to a sphere;
	Vertices sphere_vertices = {0};
	Vertices sphere_vertices_normals = {0};
	Vertices heightmapped_sphere_vertices={0};
	for (int i=0; i <4; i ++){
		for (int k=0; k<3; k++){
			sphere_vertices_normals[i][k]=convert_cube_to_sphere(temp[i],temp_orig[i])[k];
			sphere_vertices[i][k] = FACE_SIZE*convert_cube_to_sphere(temp[i],temp_orig[i])[k];
			heightmapped_sphere_vertices[i][k] = heightmap_sphere(sphere_vertices[i],sphere_vertices_normals[i],temp_orig[i])[k];
			
			//copy over to global sphere terrain and normals variables
			world_norm[i][k]=sphere_vertices_normals[i][k];
			world_vert[i][k]=heightmapped_sphere_vertices[i][k];
			
		}
	}

	//copy over to temp_dec to either make a height mapped sphere or a flat cube
	
	Vertices temp_dec = {0};
	for (int i=0; i <4; i ++){
		for (int k=0; k<3; k++){
			if (is_sphere == 1){
				//temp_dec[i][k] = sphere_vertices[i][k];
				temp_dec[i][k] = world_vert[i][k];
			}
			else if (is_sphere == 2) {
				//draw the unaltered plane
				temp_dec[i][k] = temp_orig[i][k];
			}
			else {
				temp_dec[i][k] = temp[i][k];
			}
			
		}
	}
	
	//glEnable (GL_TEXTURE_2D);

	
	//normals for shading will be wrong for cube
	if (terrain_triangle == 0)
	{

		
		if (terrain_wire == 1)
		{	
			glBegin( GL_LINES);
		} else {
			glBegin( GL_QUADS );
		}
		

			//glNormal3fv(normalize(temp_dec));
			
			//calculate which part of the texture map to use
			//texture_terrain (texture_x, texture_z)
			//texture_terrain(temp_dec[0][tex_xi], temp_dec[0][tex_yi]);
			texture_terrain(temp_orig[0][tex_xi], temp_orig[0][tex_yi]);  
			glNormal3fv(sphere_vertices_normals[0]);
			glVertex3fv(temp_dec[0]);

			//texture_terrain(temp_dec[1][tex_xi], temp_dec[1][tex_yi]); 
			texture_terrain(temp_orig[1][tex_xi], temp_orig[1][tex_yi]); 
			glNormal3fv(sphere_vertices_normals[1]);
			glVertex3fv(temp_dec[1]);
			
			//texture_terrain(temp_dec[2][tex_xi], temp_dec[2][tex_yi]);
			texture_terrain(temp_orig[2][tex_xi], temp_orig[2][tex_yi]); 
			glNormal3fv(sphere_vertices_normals[2]);
			glVertex3fv(temp_dec[2]);

			
			//texture_terrain(temp_dec[3][tex_xi], temp_dec[3][tex_yi]);
			texture_terrain(temp_orig[3][tex_xi], temp_orig[3][tex_yi]); 
			glNormal3fv(sphere_vertices_normals[3]);
			glVertex3fv(temp_dec[3]);

		glEnd();
		

		
		///////////////
		
		
		/*
		glBindTexture(GL_TEXTURE_2D, texture1);
		glColor4f(1.0f, 1.0f, 1.0f, partOf1 );
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( -1.0f, -1.0f, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( -1.0f, 1.0f, 0.0);
		glEnd();
		 
		glBindTexture(GL_TEXTURE_2D, texture2);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f - partOf1);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( -1.0f, -1.0f, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( -1.0f, 1.0f, 0.0);
		glEnd();
		*/
		
		
		
		
		
		/////////////
		
		
		
		
		
		
	} else {
	
		//triangle section not done yet
		//convert to triangles
		
		//draw vertices 0,1,2
		//temp indices are as follows: 0,1,2,3 = bot_left, top_left, top_right, bottom_right
		//glBegin( GL_TRIANGLES);
		if (terrain_wire == 1)
		{	
			glBegin( GL_LINES);
		} else {
			glBegin( GL_TRIANGLES );
		}
		
		
			//calculate which part of the texture map to use
			//texture_terrain (texture_x, texture_z)
			//texture_terrain(temp_dec[0][tex_xi], temp_dec[0][tex_yi]);
			texture_terrain(temp_orig[0][tex_xi], temp_orig[0][tex_yi]);  
			glNormal3fv(sphere_vertices_normals[0]);
			glVertex3fv(temp_dec[0]);

			//texture_terrain(temp_dec[1][tex_xi], temp_dec[1][tex_yi]); 
			texture_terrain(temp_orig[1][tex_xi], temp_orig[1][tex_yi]); 
			glNormal3fv(sphere_vertices_normals[1]);
			glVertex3fv(temp_dec[1]);
			
			//texture_terrain(temp_dec[2][tex_xi], temp_dec[2][tex_yi]);
			texture_terrain(temp_orig[2][tex_xi], temp_orig[2][tex_yi]); 
			glNormal3fv(sphere_vertices_normals[2]);
			glVertex3fv(temp_dec[2]);
		
		glEnd();
		
		if (terrain_wire == 1)
		{	
			glBegin( GL_LINES);
		} else {
			glBegin( GL_TRIANGLES );
		}
		
		
		//draw vertices 0,2,3
			texture_terrain(temp_orig[0][tex_xi], temp_orig[0][tex_yi]);  
			glNormal3fv(sphere_vertices_normals[0]);
			glVertex3fv(temp_dec[0]);
		
			//texture_terrain(temp_dec[2][tex_xi], temp_dec[2][tex_yi]);
			texture_terrain(temp_orig[2][tex_xi], temp_orig[2][tex_yi]); 
			glNormal3fv(sphere_vertices_normals[2]);
			glVertex3fv(temp_dec[2]);

			//texture_terrain(temp_dec[2][tex_xi], temp_dec[2][tex_yi]);
			texture_terrain(temp_orig[3][tex_xi], temp_orig[3][tex_yi]); 
			glNormal3fv(sphere_vertices_normals[3]);
			glVertex3fv(temp_dec[3]);
		
		glEnd();
	}
	//move character on top of the sphere, if he is within the box
	//vertex order is: enum{bot_left,top_left, top_right, bot_right} 
	if ((*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X) >= temp_orig[0][0]) &&
	   (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z) >= temp_orig[0][2]) &&
	   (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X) < (temp_orig[0][0]+STEP_SIZE)) &&
	   (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z) < (temp_orig[0][2]+STEP_SIZE)) &&
	   (animate))
	{
		//in X
		percentStepSizeTraversed = (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X) - temp_orig[0][0])/STEP_SIZE;
		charSpherePos[0] = temp_dec[0][0]-((temp_dec[0][0]-temp_dec[3][0])*percentStepSizeTraversed);
		//in Z
		percentStepSizeTraversed = (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z) - temp_orig[0][2])/STEP_SIZE;
		charSpherePos[2] = temp_dec[0][2]-((temp_dec[0][2]-temp_dec[1][2])*percentStepSizeTraversed);
		//in Y
		charSpherePos[1] = BiInterpol(temp_dec,charSpherePos[0],charSpherePos[2])+4.9;
		/*printf("root %f %f %f \n",*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X), *joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Y), *joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z));
		printf("temp_orig %f %f %f \n",temp_orig[0][0], temp_orig[0][1], temp_orig[0][2]);
		printf("world_vert %f ",world_vert[0][0], world_vert[0][1], world_vert[0][2]);
		printf("world_vertnew %f ",world_vert[3][0], world_vert[0][1], world_vert[0][2]);
		printf("root %f ",(*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X)));
		printf("tempOrig %f ",(temp_orig[0][0]));
		printf("perc %f ",percentStepSizeTraversed);
		printf("charSpherePosY %f \n",charSpherePos[1], charSpherePos[1], charSpherePos[2]);*/
	}
	

}


void drawOldHeightMappedPlane(unsigned char pHeightMap[]) {

	int X = 0, Y = 0;									/* Create Some Variables To Walk The Array With. */
	float x, y, z;										/* Create Some Variables For Readability */

	if(!pHeightMap) return;							/* Make Sure Our Height Data Is Valid */
	
	
	//borders to switch b/w textures
	int low_mid_border=95;
	int mid_high_border=190;
	
	
	GLuint tex0 = raw_texture_load (tex0file, MAP_SIZE, MAP_SIZE, 2);
	GLuint tex1 = raw_texture_load (tex0_lev1_file, MAP_SIZE, MAP_SIZE, 2);
	
	//GLuint tex0 = raw_texture_load ("texture/hi_res_stone.raw", MAP_SIZE, MAP_SIZE, 0);
	//GLuint tex1 = raw_texture_load ("texture/grass.raw", 256, 256,0);
	//GLuint tex2 = raw_texture_load ("texture/snow.raw", 256, 256,0);

	
	glActiveTexture(GL_TEXTURE0);
    glEnable( GL_TEXTURE_2D );				
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for ( X = 0; X < FACE_SIZE*4; X += STEP_SIZE )
		for ( Y = 0; Y < FACE_SIZE*3; Y += STEP_SIZE )
		{
			
			//make array to hold vertices of the plane
			Vertices temp = {0};
			
			/* Get The (X, Y, Z) Value For The Bottom Left Vertex */
			x = X;							
			y = Height(pHeightMap, X, Y );
			z = Y;

			//store into vertices array
			temp [0][0]= x;
			temp [0][1]= y;
			temp [0][2]= z;			

			/* Get The (X, Y, Z) Value For The Top Left Vertex */
			x = X;										
			y = Height(pHeightMap, X, Y + STEP_SIZE );
			z = Y + STEP_SIZE ;
			
			//store into vertices array
			temp [1][0]= x;
			temp [1][1]= y;
			temp [1][2]= z;		

			/* Get The (X, Y, Z) Value For The Top Right Vertex */
			x = X + STEP_SIZE;
			y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE );
			z = Y + STEP_SIZE ;
			
			//store into vertices array
			temp [2][0]= x;
			temp [2][1]= y;
			temp [2][2]= z;							
			
			/* Get The (X, Y, Z) Value For The Bottom Right Vertex */
			x = X + STEP_SIZE;
			y = Height(pHeightMap, X + STEP_SIZE, Y ); 
			z = Y;
			
			//store into vertices array
			temp [3][0]= x;
			temp [3][1]= y;
			temp [3][2]= z;	
			
	
				
			glBindTexture( GL_TEXTURE_2D, tex0);

			//draw the square plane
			
			if (terrain_triangle == 0)
			{
			
				glBegin( GL_QUADS );	
				

				//calculate which part of the texture map to use
				//texture_terrain (texture_x, texture_z)
				texture_terrain(temp[0][0], temp[0][2]); 
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[0]));
				glVertex3fv(temp[0]);
				
				texture_terrain(temp[1][0], temp[1][2]); 
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[1]));
				glVertex3fv(temp[1]);
				
				texture_terrain(temp[2][0], temp[2][2]);
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[2]));
				glVertex3fv(temp[2]);
				
				texture_terrain(temp[3][0], temp[3][2]);
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[3]));
				glVertex3fv(temp[3]);
				glEnd();
			} else {
			

				//convert to triangles
				
				//draw vertices 0,1,2
				//temp indices are as follows: 0,1,2,3 = bot_left, top_left, top_right, bottom_right
				glBegin( GL_TRIANGLES);
				
				
				texture_terrain(temp[0][0], temp[0][2]);
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[0])); 
				glVertex3fv(temp[0]);	
				
				texture_terrain(temp[1][0], temp[1][2]);
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[1]));  
				glVertex3fv(temp[1]);
				
				texture_terrain(temp[2][0], temp[2][2]);
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[2])); 
				glVertex3fv(temp[2]);
				
				glEnd();
				
				glBegin(GL_TRIANGLES);
				
				
				//draw vertices 0,2,3
				
				texture_terrain(temp[0][0], temp[0][2]);
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[0]));  
				glVertex3fv(temp[0]);
				
				texture_terrain(temp[2][0], temp[2][2]);
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[2]));  
				glVertex3fv(temp[2]);	

				texture_terrain(temp[3][0], temp[3][2]);
				glNormal3fv(normalize_per_vertex(pHeightMap,temp[3])); 
				glVertex3fv(temp[3]);
				
				glEnd();
			}
			
			
			
			
			
			
		}
	glActiveTexture (GL_TEXTURE0);
	glDisable (GL_TEXTURE_2D);
	glDeleteTextures(1, &tex0);
	//reset the color to white
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	
}

float BiInterpol (Vertices point, float x, float z)
{
	float xInter0,xInter1,yInter0,yInter1,fxy1,fxy2,fxy;
	xInter0 = (point[3][0]-x)/(point[3][0]-point[0][0]);
	xInter1 = (x-point[0][0])/(point[3][0]-point[0][0]);
	
	yInter0 = (point[1][2]-z)/(point[1][2]-point[0][2]);
	yInter1 = (z-point[0][2])/(point[1][2]-point[0][2]);
	
	fxy1 = xInter0*point[0][1]+xInter1*point[3][1];
	fxy2 = xInter0*point[1][1]+xInter1*point[2][1];
	
	return (yInter0*fxy1+yInter1*fxy2);
}

void whatFaceIsCharOn(float X, float Y)
{
	int whichFaceInternal=0;
	//face1, top face
	if ( X >= FACE_SIZE && X<FACE_SIZE*2 && Y >=FACE_SIZE && Y<FACE_SIZE*2)
		face_id = 1;
	//face6, bottom face
	else if ( X >= FACE_SIZE*3 && X<=MAP_SIZE && Y >=FACE_SIZE  && Y<FACE_SIZE*2)
		face_id = 6;
	//face2, left
	else if ( X >= FACE_SIZE*2 && X<FACE_SIZE*3 && Y >=FACE_SIZE  && Y<FACE_SIZE*2) 
		face_id = 2;
	//face5, right
	else if ( X >= 0 && X<=FACE_SIZE && Y >=FACE_SIZE  && Y<FACE_SIZE*2) 
		face_id = 5;
	//face4, back
	else if ( X >= FACE_SIZE && X<FACE_SIZE*2 && Y >=FACE_SIZE*2  && Y<FACE_SIZE*3) 
		face_id = 4;
	//face3, front
	else if ( X >= FACE_SIZE && X<FACE_SIZE*2 && Y >=0  && Y<FACE_SIZE*3) 
		face_id = 3;
}

int whatFaceIsObjectOn(float X, float Y)
{
	int whichFaceInternal=0;
	//face1, top face
	if ( X >= FACE_SIZE && X<FACE_SIZE*2 && Y >=FACE_SIZE && Y<FACE_SIZE*2)
		whichFaceInternal = 1;
	//face6, bottom face
	else if ( X >= FACE_SIZE*3 && X<=MAP_SIZE && Y >=FACE_SIZE  && Y<FACE_SIZE*2)
		whichFaceInternal = 6;
	//face2, left
	else if ( X >= FACE_SIZE*2 && X<FACE_SIZE*3 && Y >=FACE_SIZE  && Y<FACE_SIZE*2) 
		whichFaceInternal = 2;
	//face5, right
	else if ( X >= 0 && X<=FACE_SIZE && Y >=FACE_SIZE  && Y<FACE_SIZE*2) 
		whichFaceInternal = 5;
	//face4, back
	else if ( X >= FACE_SIZE && X<FACE_SIZE*2 && Y >=FACE_SIZE*2  && Y<FACE_SIZE*3) 
		whichFaceInternal = 4;
	//face3, front
	else if ( X >= FACE_SIZE && X<FACE_SIZE*2 && Y >=0  && Y<FACE_SIZE*3) 
		whichFaceInternal = 3;
	return whichFaceInternal;
}

void newPositionOfChar (int currentFace, float* FuncStrafe, float* FuncPropel, float* turnAngle)
{
	//printf("FuncPropel %f FuncStrafe %f currentface %d \n",*FuncPropel, *FuncStrafe,currentFace);
	//right to bottom
		if (*FuncStrafe <= BORDER_MIN){
			*FuncStrafe  = BORDER_MAX-0.01;printf("right to bottom \n");}
		else if (*FuncStrafe >= BORDER_MAX)
			*FuncStrafe = BORDER_MIN+0.01;
		//back to right
		else if ((currentFace==4)&&(*FuncStrafe <= FACE_SIZE))
		{
			*FuncStrafe = *FuncStrafe-(*FuncPropel-FACE_SIZE*2);
			*FuncPropel = FACE_SIZE*2;
			*turnAngle-=90;
			printf("back to right \n");
		}
		//right to back
		else if ((currentFace==5)&&(*FuncPropel >= FACE_SIZE*2))
		{
			*FuncPropel = *FuncPropel+(FACE_SIZE-*FuncStrafe);
			*FuncStrafe = FACE_SIZE;
			*turnAngle+=90;
			printf("right to back\n");
		}
		//right to front
		else if ((currentFace==5)&&(*FuncPropel <= FACE_SIZE))
		{
			*FuncPropel = *FuncPropel-(FACE_SIZE-*FuncStrafe);
			*FuncStrafe = FACE_SIZE;
			*turnAngle-=90;
			printf("right to front\n");
		}
		//front to right
		else if ((currentFace==3)&&(*FuncStrafe <= FACE_SIZE))
		{
			*FuncStrafe = *FuncStrafe-(FACE_SIZE-*FuncPropel);
			*FuncPropel = FACE_SIZE;
			*turnAngle+=90;
			printf("front to right\n");
		}
		//front to left
		else if ((currentFace==3)&&(*FuncStrafe >= FACE_SIZE*2))
		{
			*FuncStrafe = *FuncStrafe+(FACE_SIZE-*FuncPropel);
			*FuncPropel = FACE_SIZE;
			*turnAngle-=90;
			printf("front to left \n");
		}
		//left to front
		else if ((currentFace==2)&&(*FuncPropel <= FACE_SIZE))
		{
			*FuncPropel = *FuncPropel-(*FuncStrafe - FACE_SIZE*2);
			*FuncStrafe = FACE_SIZE*2;
			*turnAngle+=90;
			printf("left to front \n");
		}
		//left to back
		else if ((currentFace==2)&&(*FuncPropel >= FACE_SIZE*2))
		{
			*FuncPropel = *FuncPropel+(*FuncStrafe - FACE_SIZE*2);
			*FuncStrafe = FACE_SIZE*2;
			*turnAngle-=90;
			printf("left to back \n");
		}
		//back to left
		else if ((currentFace==4)&&(*FuncStrafe >= FACE_SIZE*2))
		{
			*FuncStrafe = *FuncStrafe+(*FuncPropel - FACE_SIZE*2);
			*FuncPropel = FACE_SIZE*2;
			*turnAngle+=90;
			printf("back to left \n");
		}
		//back to bottom (new if group b/c unrelated to above traversal
		if ((currentFace==4)&&(*FuncPropel >= FACE_SIZE*3))
		{
			*FuncStrafe = FACE_SIZE*3+(FACE_SIZE*2-*FuncStrafe);
			*FuncPropel = FACE_SIZE*2;
			*turnAngle-=180;
			printf("back to bottom\n");
		}
		//bottom to back
		else if ((currentFace==6)&&(*FuncPropel >= FACE_SIZE*2))
		{
			*FuncStrafe = FACE_SIZE*2-(*FuncStrafe - FACE_SIZE*3);
			*FuncPropel = FACE_SIZE*3;
			*turnAngle+=180;
			printf("bottom to back \n");
		}
		//bottom to front
		else if ((currentFace==6)&&(*FuncPropel <= FACE_SIZE))
		{
			*FuncStrafe = FACE_SIZE*2-(*FuncStrafe - FACE_SIZE*3);
			*FuncPropel = 0;
			*turnAngle-=180;
			printf("bottom to front \n");
		}
		//front to bottom
		else if ((currentFace==3)&&(*FuncPropel <= 0))
		{
			*FuncStrafe = FACE_SIZE*3+(FACE_SIZE*2-*FuncStrafe);
			*FuncPropel = FACE_SIZE;
			*turnAngle+=180;
			printf("front to bottom \n");
		}
}
