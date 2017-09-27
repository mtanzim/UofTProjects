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
float golddiff[4] = { 0.75164, 0.60648, 0.22648, 1 };
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
#define		MAP_SIZE	  		2048
#define		TEX_SIZE	  		256
#define		MAP_REPEAT	  		8				//how many times to repeat the texture per axis
#define		STEP_SIZE	 	 	16				/* Width And Height Of Each Quad */
#define		HEIGHT_RATIO  		1.0f							/* Ratio That The Y Is Scaled According To The X And Z */
#define		BORDER_MAX    		MAP_SIZE
#define		BORDER_MIN    		0
#define	    FACE_SIZE	  		MAP_SIZE/4
#define     SPHEREANGLE         360
#define     WINDOW_SIZE_X       800
#define     WINDOW_SIZE_Y       600

//terrain height map raw files
const char ter_file[]= "terrain_data/sphere_terrain_large.raw";
const char ter1_file[]= "terrain_data/sphere_terrain1_large.raw";
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
const int terrain_triangle=1;
//whether or not to conver to a sphere, or draw the original plane
//0-draw box
//1-draw sphere
//2-draw orginal plane
int is_sphere=2;

//whether or not to use the height map
int is_height=1;
//whether or not to light the terrain
const int not_lit=0;
//whether or not to wireframe for debug
const int terrain_wire=0;
//flag to select which world to draw
int world_id=1;
//    4
//6 2 1 5
//    3  .
//       (origin)
//flag that tracks which face we're on
int face_id=1;

//factor by which height value is to be applied to the sphere
const float heightmap_factor=1.0f;
//return sphere to heightmap
float sphere_height (float xy, float yz);

//store spherical terrain vertices
bool record_keeping = false;
Vertices world_vert = {0};
Vertex world_vert_record [MAP_SIZE/STEP_SIZE+1][MAP_SIZE/STEP_SIZE+1] = {0};
//store spherical terrain normals
Vertices world_norm = {0};
Vertex world_norm_record [MAP_SIZE/STEP_SIZE+1][MAP_SIZE/STEP_SIZE+1] = {0};
int is_recorded = 0;
//store the vertices of the flat world for traversing the sphere
Vertices temp_orig={0};
Vertex temp_orig_record [MAP_SIZE/STEP_SIZE+1][MAP_SIZE/STEP_SIZE+1] = {0}; 
//variable to store miscellaneous vertex information b/w sphere mapping functions
Vertex returner={0};

//how much to transfer the terrain after it has been created
const float TER_TRAN_X=0;//-MAP_SIZE/2;
const float TER_TRAN_Y=0;//-85+80.5-0.4;
const float TER_TRAN_Z=0;//-MAP_SIZE/4;
	
//track the sphere position on the plane
int sphere_map_X= 0;
int sphere_map_Y= 0;

int char_height_X= 0;
int char_height_Y= 0;
int char_height_Z= 0;
	
	
//-----------------TEXTURE VARIABLES-------------------//
//texture loader
GLuint raw_texture_load(const char *filename, int width, int height, int is_back);
static GLuint texture;
float g_fContributionOfTex0 = 0.33f;
float g_fContributionOfTex1 = 0.33f;
float g_fContributionOfTex2 = 0.33f;
const char *tex0file="texture/mars_alt.raw";
const char *tex0_lev1_file="texture/moon.raw";

const char *tex1file="texture/mars_small.raw";
const char *tex1_lev1_file="texture/moon.raw";

//--------------------------------------------------------
///////////////////////SUN/MOON VARIABLES////////////////////////////////////
//tm added light position
const float LIGHT_RADIUS             = 3*(FACE_SIZE);
const float LIGHT_Z             = FACE_SIZE*3;
const float LAMP_SCALE=MAP_SIZE/10;
//const float LAMP_SCALE_Y=256;
//const float LAMP_SCALE_Z=256;
//sun texture
#define		MOON_SIZE	  450	
const char *moon_file="texture/moon.raw";
void drawMoonSphere();
void drawBackSphere();



//-----------------BACKGROUND VARIABLES-------------------//
#define		BACK_SIZE_X	  	80	
#define		BACK_SIZE_Y		60
//background algorith taken from:
//http://www.felixgers.de/teaching/jogl/textureAsBackground.html
//////////background//////
void drawBackground() ;
const char *backfile="background/stars.raw";
int back_tex_size_x=1920;
int back_tex_size_y=1080;

int is_back_there=1;

int is_back_drawn=0;
const float WORLD_SCALE_X=MAP_SIZE*2;
const float WORLD_SCALE_Y=MAP_SIZE*2;
const float WORLD_SCALE_Z=MAP_SIZE*2;
const float WORLD_SHIFT_Z=0;//1.1*(MAP_SIZE/4);

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
void PlaneToSpherePointConvert (float X, float Z, float* internalCharSpherePosX,float* internalCharSpherePosY,float* internalCharSpherePosZ);
float VectorAngle(float x, float y, float x1, float y1);
float VectorAngleCos(float x, float y, float x1, float y1);
float VectorAngleTan(float x, float y, float x1, float y1);
//Positional Global Variables
double MoveVertical = 0;
double MoveHorizontal = 0;
double Move3 = 0;
float lightAngle=0;
float unitsPerFrame = 2.5; //how much to send him forward per frame (based on keeping foot in same place)
float timePerFrame = 0;

float Propel=0;
float Strafe=0;
float CameraPropel=0;
float CameraStrafe=0;
const float PropelAmount=1;//
const float StrafeAmount=1;//same as propel but sideways
float gROOT_ROTATE_Y =  0.0; //character rotating horizontally (not strafing case)
const float gRotateAmount=10;
int strafeEnabled = 1;

Vertex charSpherePos={0}; //sphere position
float charAngle[2] = {0};
Vertex charNorm={0};
float percentStepSizeTraversed=0;
float percentStepSizeTraversedX=0;
float percentStepSizeTraversedZ=0;

// Camera settings
int updateCamZPos = 0;
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
int blast_off = 0;
int blast_countdown = 3;
bool is_intro = 1;
int intro_x = 0;
int intro_y = 0;
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
const int total_fuel = 100;
int fuel_location[total_fuel][4] = {0};//x,y,z,fuel_obtained
float fuel_bounce = 0;
int fuel_rev = 0;
const float BOUNCE_FACTOR = 0.4;
int fuel_remaining = total_fuel;
int fuel_needed = 4;
bool at_fuel=0;
//***************enemy constants*******************
const int total_enemy = 30;
int enemy_location[total_enemy][4] = {0};//x,y,z,fuel_obtained
const float ENEMY_MOVE_FACTOR = 1;
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
void strokePrint(float x, float y,int z, float scale, GLfloat *fillColour, char *string, void* font);

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
        Win[0] = WINDOW_SIZE_X;
        Win[1] = WINDOW_SIZE_Y;
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

	// Add status line
	glui_panel = glui_keyframe->add_panel("");
	status = glui_keyframe->add_statictext_to_panel(glui_panel, "Status: Ready");

	// Add button to quit
	glui_panel = glui_keyframe->add_panel("", GLUI_PANEL_NONE);
	glui_keyframe->add_button_to_panel(glui_panel, "Quit", 0, quitButton);
	//
	// ***************************************************

}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    //glClearColor(0.7f,0.7f,0.9f,1.0f);
	
	glClearColor(.0f,.0f,.0f,.0f);
	loadKeyframesFromFileButton(MoveHorizontal);
	//dude initial position
	joint_ui_data->setDOF(Keyframe::R_SHOULDER_PITCH,-90);
	joint_ui_data->setDOF(Keyframe::L_SHOULDER_PITCH,-90);

	Propel = 0;//(FACE_SIZE/2)*3;
	Strafe = 0;//(FACE_SIZE/2)*3;
	joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_X,Propel);
	joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_Z,Strafe);
	joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_Y,0);
	//ship initial position
	
	//while (whatFaceIsObjectOn(ship_x, ship_z)==0)
	{
		ship_x = (rand() % SPHEREANGLE)+TER_TRAN_X;//-20-q*10;
		printf("ship x %d",ship_x);
		ship_z = (rand() % SPHEREANGLE)+TER_TRAN_Z;//-20-q*10;
		printf(" z %d",ship_z);
		ship_y= FACE_SIZE;//Height(g_HeightMap,ship_x-TER_TRAN_X,ship_z-TER_TRAN_Z);
		printf(" y %d \n",ship_y);
	}	
	//fuel cell location
	
	for (int q = 0;q<fuel_remaining;q++)
	{
		{
			fuel_location[q][0] = (rand() % (SPHEREANGLE-1))+1+TER_TRAN_X;//-20-q*10;
			printf("fuel %d x %d",q,fuel_location[q][0]);
			fuel_location[q][2] = (rand() % (SPHEREANGLE-1))+1+TER_TRAN_Z;//-20-q*10;
			printf(" z %d",fuel_location[q][2]);
			fuel_location[q][1] = FACE_SIZE+3;
			printf(" y %d \n",fuel_location[q][1]);
		}
	}
	
	for (int q = 0;q<total_enemy;q++)
	{
		{
			enemy_location[q][0] = (rand() % (SPHEREANGLE-1))+1+TER_TRAN_X;//-20-q*10;
			//printf("Sentinel %d x %d",q,enemy_location[q][0]);
			enemy_location[q][2] = (rand() % (SPHEREANGLE-1))+1+TER_TRAN_Z;//-20-q*10;
			//printf(" z %d",enemy_location[q][2]);
			enemy_location[q][1] = FACE_SIZE+6;
			//printf(" y %d \n",enemy_location[q][1]);
		}
	}
	int printfuel = fuel_needed-(total_fuel-fuel_remaining);
	if (printfuel<0)
		printfuel=0;
	//fuel cells remaining
	sprintf(screenmsg, "%d / %d",(total_fuel-fuel_remaining),fuel_needed);
	//enable depth buffer
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(1.0, 2);
		
	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE); 
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
    if(0)//!shipfly)
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
			//printf("animation over %d!\n",animation_over);
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
			if (keyStates['w'])
				Propel += PropelAmount;
			else if (keyStates['s'])
				Propel -= PropelAmount;
			if (keyStates['a'])
				Strafe += StrafeAmount;
			else if (keyStates['d'])
				Strafe -= StrafeAmount;
		}
		else //strafe not enabled
		{
			if (keyStates['w'])
			{
				CameraPropel += PropelAmount;
				Propel += PropelAmount*cos(gROOT_ROTATE_Y*(PI/180));
				Strafe += StrafeAmount*sin(gROOT_ROTATE_Y*(PI/180));
			}
			else if (keyStates['s'])
			{
				CameraPropel -= PropelAmount;
				Propel -= PropelAmount*cos(gROOT_ROTATE_Y*(PI/180));
				CameraStrafe -= StrafeAmount;
				Strafe -= StrafeAmount*sin(gROOT_ROTATE_Y*(PI/180));
			}
			if (keyStates['a'])
			{
				gROOT_ROTATE_Y += gRotateAmount;
				CameraStrafe += StrafeAmount;
			}
			else if (keyStates['d'])
			{
				gROOT_ROTATE_Y -= gRotateAmount;
				CameraStrafe -= StrafeAmount;
			}
		}
		//printf("propel %f strafe %f groot %f \n",Propel, Strafe,gROOT_ROTATE_Y);
		 
		joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_Z,Propel);
		joint_ui_data->setDOF(Keyframe::ROOT_TRANSLATE_X,Strafe);
		joint_ui_data->setDOF(Keyframe::ROOT_ROTATE_Y,gROOT_ROTATE_Y);
		
		// Update user interface
		joint_ui_data->setTime(curTime);
		glui_keyframe->sync_live();
//****************************************************INTERACTIVE ELEMENTS*************************************************************
		//angle conversions
		float localPropel=fmod(Propel,360);
		if (localPropel<0)
			localPropel += 360;
		float localStrafe=fmod(Strafe,360);
		if (localStrafe<0)
			localStrafe += 360;
		float mirrorPropel = 180-localPropel;
		if (mirrorPropel <0)
			mirrorPropel += 360;
		float mirrorStrafe = localStrafe+180;
		if (mirrorStrafe >360)
			mirrorStrafe -= 360;
		
		//reached the ship!
		int printfuel = fuel_needed-(total_fuel-fuel_remaining);
		if (((fabs(localPropel-ship_x)<5)&&
				(fabs(localStrafe-ship_z)<5))
				||
				((fabs(mirrorPropel-ship_x)<5)&&
				(fabs(mirrorStrafe-ship_z)<5)))
		{
			if (total_fuel-fuel_remaining>=fuel_needed)
			{
				printf("reached ship!\n");
				shipfly = 1;
				blast_off=1;
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
			if (total_fuel-fuel_remaining>=fuel_needed)
			{
				sprintf(screenmsg, "GET TO THE SHIP");
			}
			else
			{
				if (printfuel<0)
					printfuel=0;
				sprintf(screenmsg, "%d / %d",(total_fuel-fuel_remaining),fuel_needed);
				//sprintf(screenmsg, "Fuel Cells Remaining: %d ", fuel_remaining);
			}
		}
				
		for (int q = 0;q<total_fuel;q++)
		{
			//reached the FUEL CELL!
			if ((((fabs(localPropel-fuel_location[q][0])<2)&&
				(fabs(localStrafe-fuel_location[q][2])<2))
				||
				((fabs(mirrorPropel-fuel_location[q][0])<2)&&
				(fabs(mirrorStrafe-fuel_location[q][2])<2)))
				&&(!at_fuel)&&
				(!fuel_location[q][3])&&//fuel not obtained
				(fuel_remaining>0))
			{
				fuel_location[q][3]=1;
				at_fuel = 1;
				fuel_remaining--;
				//sprintf(screenmsg, "Got The Fuel Cell! %d more to find! ",fuel_remaining);
			}
		}
		for (int q = 0;q<total_enemy;q++)
		{
			//Hit a sentinel!
			if (((fabs(localPropel-enemy_location[q][0])<2)&&
				(fabs(localStrafe-enemy_location[q][2])<2))
				||
				((fabs(mirrorPropel-enemy_location[q][0])<2)&&
				(fabs(mirrorStrafe-enemy_location[q][2])<2)))
			{
				enemy_location[q][3]=1;
				fuel_remaining=total_fuel;
				for (int q = 0;q<total_fuel;q++)
				{
					//turn on all the fuels again and reset locations
					fuel_location[q][3]=0;
					{
						fuel_location[q][0] = (rand() % (SPHEREANGLE-1))+1+TER_TRAN_X;
						fuel_location[q][2] = (rand() % (SPHEREANGLE-1))+1+TER_TRAN_Z;
						fuel_location[q][1] = FACE_SIZE+3;
					}
				}
				sprintf(screenmsg, "You Hit a Sentinel! Fuel Cells Lost! %d more to find! ",fuel_remaining);
			}
		}
//****************************************************FLY AWAY*************************************************************
	}//animate mode over
	else if (shipfly) //reached the ship, time to leave!
	{
		if (blast_off>0)//if countdown still happening, increase blastoff
			blast_off++;
		
		if (blast_off==12){//once blast_off hits max val, decrease countdown and restart blastoff
			blast_countdown--;
			blast_off=1;
		}
		if (blast_countdown==0)//once countdown over, blastoff goes to 0 to allow ship to leave
			blast_off=0;
		
		if (blast_off == 0)
			ship_y += 4;
		
		ship_rot_y += 4;
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
	//get the right charSpherePos position before camera starts, so that camera isn't one step behind char on movement
		
	if (!shipfly) //break camera from character once it flies
	{	
		if (is_intro)
		{
			intro_x++;
			intro_y=-45;
			glTranslatef(camXPos,camYPos,-1000);
			sprintf(screenmsg, "UNCHARTED SPACE");
			strokePrint(-640,210,-camZPos-0.1,1,Colourwhite,screenmsg,GLUT_STROKE_ROMAN);
			sprintf(screenmsg, "MOVEMENT:WASD");
			print(-0.6+MoveHorizontal,0.1,1000-0.1,Colourwhite,screenmsg,GLUT_BITMAP_9_BY_15);
			if (total_fuel>1)
				sprintf(screenmsg, "OBJECTIVE ALPHA:  Collect All %d Fuel Cells",total_fuel);
			else
				sprintf(screenmsg, "OBJECTIVE ALPHA:  Collect The Fuel Cell");
			print(-0.6,0.0,1000-0.1,Colourwhite,screenmsg,GLUT_BITMAP_9_BY_15);
			sprintf(screenmsg, "OBJECTIVE BETA:   Return To Your Spacecraft");
			print(-0.6,-0.1,1000-0.1,Colourwhite,screenmsg,GLUT_BITMAP_9_BY_15);
			if (total_enemy>1)
				sprintf(screenmsg, "CONSTRAINT ALPHA: Avoid The %d Sentinels",total_enemy);
			else
				sprintf(screenmsg, "CONSTRAINT ALPHA: Avoid The Sentinel");
			print(-0.6,-0.2,1000-0.1,Colourwhite,screenmsg,GLUT_BITMAP_9_BY_15);
			sprintf(screenmsg, "Press Any Key To Begin");
			print(-0.2,-0.3,1000-0.1,Colourwhite,screenmsg,GLUT_BITMAP_9_BY_15);
			glRotatef(intro_x, 0.0,1.0,0.0);
			glRotatef(intro_y, -cos(intro_x*(PI/180)),0.0,-sin(intro_x*(PI/180)));	
		}
		else //in game follow dude 
		{
			glTranslatef(camXPos,camYPos,camZPos);
			print(MoveHorizontal/*-0.05*/,MoveVertical/*-0.25*/,-camZPos-0.1,Colourwhite,screenmsg,GLUT_BITMAP_9_BY_15);
			glRotatef(camXPosRotate, 0.0,1.0,0.0);
			glRotatef(camYPosRotate, -cos(camXPosRotate*(PI/180)),0.0,-sin(camXPosRotate*(PI/180)));
			//glRotatef(-gROOT_ROTATE_Y, 0.0,1.0,0.0);
			///////////////////////AXES
			drawAxes(1024.0);
		}
		if (is_height)
			glTranslatef(0,-sphere_height(Propel, Strafe), 0);
		else
			glTranslatef(0,-FACE_SIZE-4.9,0);

		glRotatef(-Propel, 1.0, 0.0, 0.0);
		
		glRotatef(-Strafe, 0,0,1);
	}
	else //look at shipflying away
	{	
		glTranslatef(camXPos,camYPos,camZPos);
		//once hit ship, start countdown 3 2 1 then ship will go up
		if (blast_off>0)	
		{
			sprintf(screenmsg, "%d",blast_countdown);
			strokePrint(-35,-10,-camZPos-0.2,blast_off*0.04,Colourwhite,screenmsg,GLUT_STROKE_ROMAN);
		}
		else
		{
			sprintf(screenmsg, "COMPLETE",blast_countdown);
			print(0,0,-camZPos-0.1,Colourwhite,screenmsg,GLUT_BITMAP_9_BY_15);
		}
		if (ship_y>1000)
			quitButton(0);//KILL THE PROGRAM
		
		glRotatef(camXPosRotate, 0.0,1.0,0.0);
		glRotatef(camYPosRotate, -cos(camXPosRotate*(PI/180)),0.0,-sin(camXPosRotate*(PI/180)));
		if (is_height)
			glTranslatef(0,-sphere_height(Propel, Strafe),0);
		else
			glTranslatef(0,-FACE_SIZE-4.9,0);
		glRotatef(-Propel,
				1.0, 0.0, 0.0);
		glRotatef(-Strafe,
				0,0,1);
	}

	//////////////////////////////////////LIGHT SOURCE//////////////////////////////////////
	//if (Move3!=0)
		lightAngle+=3;
		
	GLfloat lightPos0[] = {cos((*joint_ui_data->getDOFPtr(Keyframe::LIGHTANGLE)+lightAngle)*(PI/180)), 
						   sin((*joint_ui_data->getDOFPtr(Keyframe::LIGHTANGLE)+lightAngle)*(PI/180)), 
						   0.0f, 0.0f};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, Colourwhite);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	/////////////////////////////////////////draw background//////////////////////
	
	glDisable(GL_LIGHTING);
	glPushMatrix();
		glTranslatef(0.0,0.0,WORLD_SHIFT_Z);
		glRotatef(90,1,0,0);
		glScalef(WORLD_SCALE_X,WORLD_SCALE_Y,WORLD_SCALE_Z);
		if (is_back_there == 1) {
			drawBackSphere();
		}
		//drawBackground();
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
			glPushMatrix();
				glScalef(LAMP_SCALE,LAMP_SCALE,LAMP_SCALE);
				drawMoonSphere();
				
			glPopMatrix();
			
			glEnable(GL_LIGHTING);
	glPopMatrix();
	
	
	///////////////////////////////////////////////SHIP///////////////////////////////////////	
	// draw main cylinder 
	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);
		glTranslatef(0,-4.6,0);
		if (!shipfly)
			ship_y=FACE_SIZE;
		
		glRotatef(ship_x,1,0,0);
		glRotatef(ship_z,
		//0,0,1); //below is the correct position
		0.0, 1.0*sin(ship_x*(PI/180)),1.0*cos(ship_x*(PI/180)));		  
		glTranslatef(0,ship_y,0);
		
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
			glPushMatrix();//four cylinders
				glEnable(GL_COLOR_MATERIAL);
				glRotatef(fuel_location[q][0],1,0,0);
				glRotatef(fuel_location[q][2],
				//0,0,1); //below is the correct position
				0.0, 1.0*sin(fuel_location[q][0]*(PI/180)),1.0*cos(fuel_location[q][0]*(PI/180)));		  
				glTranslatef(0,fuel_location[q][1],0);
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
			enemy_location[q][0] += ((rand()%3)-1)*ENEMY_MOVE_FACTOR;
			enemy_location[q][2] += ((rand()%3)-1)*ENEMY_MOVE_FACTOR;
		
		glPushMatrix();//just an evil cube
			glEnable(GL_COLOR_MATERIAL);
			glRotatef(enemy_location[q][0],1,0,0);
			glRotatef(enemy_location[q][2],
			0.0, 1.0*sin(enemy_location[q][0]*(PI/180)),1.0*cos(enemy_location[q][0]*(PI/180)));	
			glTranslatef(0,enemy_location[q][1],0);
			glPushMatrix();
				glScalef(5,5,5);
				drawShapeDispatcher(CUBE,renderStyle,ColourSilver,Colourblack);
			glPopMatrix();
		glPopMatrix();
	}
	///////////////////////////////////////////////BODY///////////////////////////////////////	    
	
	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);

		glRotatef(Propel,
					1,0,0);
		glRotatef(Strafe,
                  0.0, 1.0*sin(Propel*(PI/180)),1.0*cos(Propel*(PI/180)));
		
		if (shipfly)
			glTranslatef(0,0,0);//make him go to center of earth when ship leaves
		else if (is_height)
			glTranslatef(0,sphere_height(Propel, Strafe),0);
		else
			glTranslatef(0,FACE_SIZE+4.9,0);
		
		
		glRotatef(*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_X),
                  1.0, 0.0, 0.0);
		glRotatef(*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Z),
                  0.0, 0.0, 1.0); 
		glRotatef(*joint_ui_data->getDOFPtr(Keyframe::ROOT_ROTATE_Y),
                  0.0, 1.0, 0.0); 
				  //set up transformation for body
		///////////////////////AXES
	drawAxes(1024.0);
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
	//find char position at end to allow sync
	//figure out the sphere coordinates of the character
}


// Handles mouse button pressed / released events
void mouse(int button, int state, int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if(( button == GLUT_RIGHT_BUTTON )||(button == 3)||(button == 4)||(button == 5))
	{
		if( state == GLUT_DOWN )
		{
			lastX = x;
			lastY = y;
			//printf("button %d\n",button);
			updateCamZPos = button;
			if (updateCamZPos == 3)//scroll wheel up
			camZPosTemp = camZPos +5;
			else if (updateCamZPos == 4)//scroll wheel down
			camZPosTemp = camZPos -5;
		}
		else
		{
			updateCamZPos = 0;
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
	
	//shipfly=1;
	//blast_off=1;
	switch (key){
		case 'w':
		case 's':
		case 'a': 
		case 'd': 
			loadKeyframesFromFileButton(MoveHorizontal);
				is_intro = 0;
				animateButton();  
			break;
		case 'i':
			//MoveVertical +=0.1;
			break;
		case 'k':
			//MoveVertical -=0.1;
			break;
		case 'j':
			if (!world_id)
				world_id=1;
			else
				world_id =0;
			break;
		case 'l':
			if (!is_height)
				is_height=1;
			else
				is_height =0;
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
		if (updateCamZPos == GLUT_RIGHT_BUTTON)
			camZPosTemp =camZPos + (x - lastX) * ZOOM_SCALE;

		//printf("camZPos %f\n",camZPos);
		if (/*(camZPosTemp >-100)&&*/(camZPosTemp<-7))//clamp the camera
			camZPos = camZPosTemp;
		lastX = x;
		num_faces2+= (y - lastY);
		lastY = y;

		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
	if ( updateAngleonLeftButton )
	{
		camXPosRotate += (x - lastX) ;
		//gcamXPosRotateAngle = x;
		lastX = x;
		camYPosRotate += (y - lastY) ;
		lastY = y;
		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
}

void motionPassive(int x, int y)
{
		camXPosRotate += (x - lastX) ;
		lastX = x;
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
	
	x = current_pt[0];
	z = current_pt[2];

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
	return normal;
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
	GLuint tex0 = raw_texture_load (backfile, back_tex_size_x, back_tex_size_y, 1);
	glActiveTexture(GL_TEXTURE0);
    glEnable( GL_TEXTURE_2D );		
	glBindTexture( GL_TEXTURE_2D, tex0);
	glBegin(GL_QUADS);
		glNormal3fv(normalize(face));
		//glTexCoord2i( 0, 0);
		glVertex3fv(face[0]);
		//glTexCoord2i( 0, 1);
		glVertex3fv(face[1]);
		//glTexCoord2i( 1, 1);
		glVertex3fv(face[2]);
		//glTexCoord2i( 1, 0);
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

void drawBackSphere(){
	GLUquadric *quad;
	quad = gluNewQuadric();
	GLuint tex0 = raw_texture_load (backfile, back_tex_size_x, back_tex_size_y, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glEnable( GL_TEXTURE_2D );	
	glBindTexture(GL_TEXTURE_2D, tex0);

	gluQuadricTexture(quad,1);
	glBegin(GL_QUADS);
	//glutSolidSphere(quad, 1,50,50);	
	gluSphere(quad, 1,50,50);
	glEnd();	
	
	glActiveTexture (GL_TEXTURE0);
	glDisable (GL_TEXTURE_2D);
	glDeleteTextures(1, &tex0);
}

void drawMoonSphere(){
	GLUquadric *quad;
	quad = gluNewQuadric();
	GLuint tex0 = raw_texture_load (moon_file, MOON_SIZE, MOON_SIZE, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glEnable( GL_TEXTURE_2D );	
	glBindTexture(GL_TEXTURE_2D, tex0);

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

void strokePrint(float x, float y,int z, float scale, GLfloat *fillColour, char *string, void* font)
{
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glScalef(scale,scale,scale);
		glTranslatef(x,y, z);
		glColor3fv(fillColour);
		//set the position of the text in the window using the x and y coordinates
		glRasterPos3f(0,0,0);
		//get the length of the string to display
		int len = (int) strlen(string);
		//loop to display character by character
		//glScalef(scale,scale,scale);
		for (int i = 0; i < len; i++)
		{
			glutStrokeCharacter(font,string[i]);
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
		     // when texture area is small, bilinear filter the closest mipmap
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first mipmap
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
	}
	
		// texture should tile
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
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
			
		int low_mid_border=95;
		int mid_high_border=200;
		int high_transition_zone=30;
		int low_transition_zone=10;

		//this formula gives the illusion of lighting, need to design actual lighting algorithm
		if (height < low_mid_border) {
			//glDisable(GL_TEXTURE_2D);
			//glColor3f(fColor, fColor , fColor);
			g_fContributionOfTex0=0.0;
			g_fContributionOfTex1=0.0;
			g_fContributionOfTex2=1.0;
		}
		else if (height > mid_high_border)  {
			g_fContributionOfTex0=1.0;
			g_fContributionOfTex1=0.0;
			g_fContributionOfTex2=0.0;
		}
		
		//mid region
		else {
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


void texture_sun (float x, float z){
			
		glTexCoord2f(x, z);
	
}



 void texture_terrain (float x, float z){
		glTexCoord2f(x/(FACE_SIZE/MAP_REPEAT), z/(FACE_SIZE/MAP_REPEAT));
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
	
	glEnable(GL_COLOR_MATERIAL);
	glActiveTexture(GL_TEXTURE0);
	glEnable( GL_TEXTURE_2D );
	//glDisable(GL_LIGHTING);	
	
	glBindTexture( GL_TEXTURE_2D, tex0);
	
	glBegin(GL_QUADS); 
	  glTexCoord2i( 0, 0);
	  glVertex2i( 0, 0 );
	  glTexCoord2i( 0,1  );
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
	
	glDisable(GL_COLOR_MATERIAL);
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
	
	//GLuint tex0 = raw_texture_load (ter_tex, MAP_SIZE, MAP_SIZE, 0);
	GLuint tex0 = raw_texture_load (ter_tex, TEX_SIZE, TEX_SIZE, 0);
	GLuint tex1 = raw_texture_load (lev1_tex, TEX_SIZE, TEX_SIZE, 0);
	
	if (not_lit==0) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
		
		glMaterialfv(GL_FRONT, GL_SPECULAR, goldspec);
		glMaterialfv(GL_FRONT, GL_SHININESS, goldshine);
		glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		
		glLightfv(GL_LIGHT0, GL_SPECULAR, def_spec);
		glLightfv(GL_LIGHT0, GL_AMBIENT, def_amb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, def_dif);
	}	
	
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
			/* Get The (X, Y, Z) Value For The Bottom Left Vertex */
			x = X;							
			if (is_height==1) {y = Height(pHeightMap, X, Y );}
			z = Y;
			
			if (X==sphere_map_X && Y==sphere_map_Y) {
				//printf ("X_plane %d Y_plane %d height is:\t\t %f\n ", X, Y, y);
			}
			
			

			//store into vertices array
			temp [0][0]= x;
			temp [0][1]= y;
			temp [0][2]= z;			

			/* Get The (X, Y, Z) Value For The Top Left Vertex */
			x = X;										
			if (is_height==1) {y = Height(pHeightMap, X, Y + STEP_SIZE );}
			z = Y + STEP_SIZE ;
			
			//store into vertices array
			temp [1][0]= x;
			temp [1][1]= y;
			temp [1][2]= z;		

			/* Get The (X, Y, Z) Value For The Top Right Vertex */
			x = X + STEP_SIZE;
			if (is_height==1) {y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE );}
			z = Y + STEP_SIZE ;
			
			//store into vertices array
			temp [2][0]= x;
			temp [2][1]= y;
			temp [2][2]= z;							
			
			/* Get The (X, Y, Z) Value For The Bottom Right Vertex */
			x = X + STEP_SIZE;
			if (is_height==1) {y = Height(pHeightMap, X + STEP_SIZE, Y );} 
			z = Y;
			
			//store into vertices array
			temp [3][0]= x;
			temp [3][1]= y;
			temp [3][2]= z;	

			//basic texturing
			glDisable( GL_BLEND );
			glActiveTexture (GL_TEXTURE0);
			glEnable (GL_TEXTURE_2D);
			glBindTexture( GL_TEXTURE_2D, tex0);
			glActiveTexture (GL_TEXTURE1);
			glDisable (GL_TEXTURE_2D);
		
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
					//y 
					temp_c[i][1]=sin(M_PI)*temp[i][0]+cos(M_PI)*temp[i][1];
					//z
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
			//PlaneToSpherePointConvert enable this when using eth function
			if (0)//!animation_over)
			{
				record_keeping = true;
				for (int i=0; i <3; i ++){
					world_vert_record[(X/STEP_SIZE)][(Y/STEP_SIZE)][i] =  world_vert[0][i];
					world_norm_record[(X/STEP_SIZE)][(Y/STEP_SIZE)][i] =  world_norm[0][i];
					temp_orig_record[(X/STEP_SIZE)][(Y/STEP_SIZE)][i] =  temp_orig[0][i];

					world_vert_record[(X/STEP_SIZE)+1][(Y/STEP_SIZE)][i] =  world_vert[3][i];
					world_norm_record[(X/STEP_SIZE)+1][(Y/STEP_SIZE)][i] =  world_norm[3][i];
					temp_orig_record[(X/STEP_SIZE)+1][(Y/STEP_SIZE)][i] =  temp_orig[3][i];

					world_vert_record[(X/STEP_SIZE)][(Y/STEP_SIZE)+1][i] =  world_vert[1][i];
					world_norm_record[(X/STEP_SIZE)][(Y/STEP_SIZE)+1][i] =  world_norm[1][i];
					temp_orig_record[(X/STEP_SIZE)][(Y/STEP_SIZE)+1][i] =  temp_orig[1][i];

					world_vert_record[(X/STEP_SIZE)+1][(Y/STEP_SIZE)+1][i] =  world_vert[2][i];
					world_norm_record[(X/STEP_SIZE)+1][(Y/STEP_SIZE)+1][i] =  world_norm[2][i];
					temp_orig_record[(X/STEP_SIZE)+1][(Y/STEP_SIZE)+1][i] =  temp_orig[2][i];
					
					//printf(" world_norm[i][k] %f i %d\n",world_norm[0][i],i);
								
					if 	(0)//(Y == 496)&&(i==2))
					{
						printf(" world_norm_record[i][k] %f i %d step %d\n",world_norm_record[(X/STEP_SIZE)][(Y/STEP_SIZE)][i],i,X/STEP_SIZE);
						printf(" world_norm_record[i][k] %f i %d \n",world_norm_record[(X/STEP_SIZE)][(Y/STEP_SIZE)+1][i],i);
						printf(" world_norm_record[i][k] %f i %d \n",world_norm_record[(X/STEP_SIZE)+1][(Y/STEP_SIZE)+1][i],i);
						printf(" world_norm_record[i][k] %f i %d step %d\n",world_norm_record[(X/STEP_SIZE)+1][(Y/STEP_SIZE)][i],i,(X/STEP_SIZE)+1);
						//printf(" world_vert[i][k] %f i %d k %d\n",world_vert[i][k],i,k);
					}
					record_keeping = false;
				}
			}
		}
	}
	glDisable( GL_BLEND );
	glActiveTexture (GL_TEXTURE0);
	glDisable (GL_TEXTURE_2D);
	glActiveTexture (GL_TEXTURE1);
	glDisable (GL_TEXTURE_2D);
	glDeleteTextures(1, &tex0);
	glDeleteTextures(1, &tex1);
	//reset the color to white
	//glColor3f(1.0f, 1.0f, 1.0f);
	glDisable (GL_COLOR_MATERIAL);

}
//propel, strafe
float sphere_height (float yz, float xy) {

	//printf("xy %f yz %f \n", xy, yz);
	Vertex char_sphere_pos={0};

	//(FACE_SIZE/2)*3 is the center of the plane
	//trying to map the propel, strafe angles to the original heightmap
	float map_x=(FACE_SIZE/2)*tan(yz*(M_PI/180)) + (FACE_SIZE/2)*3;
	float map_y=(FACE_SIZE/2)*tan(xy*(M_PI/180)) +  (FACE_SIZE/2)*3;

	sphere_map_X= (int)map_x;
	sphere_map_Y= (int)map_y;
	

	int fsphere_height = 0;
	int fsphere_height_normalized = 0;
	
	if (world_id == 0) {
		
		fsphere_height = Height(g_HeightMap,sphere_map_X,sphere_map_Y);
	} else if (world_id == 1) {
		fsphere_height = Height(g1_HeightMap,sphere_map_X,sphere_map_Y);
	}
	
	
	//got the heightmap, now reconstruct the cube, sphere, heightmapped sphere
	float cube_plane_x=map_x-(FACE_SIZE/2)*3;
	float cube_plane_y=fsphere_height+(FACE_SIZE/2);
	float cube_plane_z=map_y-(FACE_SIZE/2)*3;

	float length = sqrtf(cube_plane_x*cube_plane_x + cube_plane_y*cube_plane_y + cube_plane_z*cube_plane_z);
	//make the unit sphere, and normals
	float sx_n = cube_plane_x / length;
	float sy_n = cube_plane_y / length;
	float sz_n = cube_plane_z / length;
	//correct the radius of unit sphere
	float sx = (FACE_SIZE)*sx_n;
	float sy = (FACE_SIZE)*sy_n;
	float sz = (FACE_SIZE)*sz_n;
	//apply the hightmap
	float sx_sphere = sx+sx_n*fsphere_height;
	float sy_sphere = sy+sy_n*fsphere_height;
	float sz_sphere = sz+sz_n*fsphere_height;
	
	float spherelength = sqrtf(sx_sphere*sx_sphere + sy_sphere*sy_sphere + sz_sphere*sz_sphere)+4.9;

	return (float)spherelength;
	
	
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

	returner[0]=x+x_n*height;
	returner[1]=y+y_n*height;
	returner[2]=z+z_n*height;
	
	return returner;
	
	
	
}



//http://www.cs.cmu.edu/~ajw/s2007/0251-SphericalWorlds.pdf
float* convert_cube_to_sphere (Vertex cube_vertices, Vertex orig_plane_vertex){

	float x=(cube_vertices[0]);
	float y=(cube_vertices[1]);
	float z=(cube_vertices[2]);
	
	//from the height map
	
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
	Vertices temp_dec = {0};
	for (int i=0; i <4; i ++){
		for (int k=0; k<3; k++){
			sphere_vertices_normals[i][k]=convert_cube_to_sphere(temp[i],temp_orig[i])[k];
			//multply by radius of the sphere
			sphere_vertices[i][k] = (FACE_SIZE)*convert_cube_to_sphere(temp[i],temp_orig[i])[k];
			heightmapped_sphere_vertices[i][k] = heightmap_sphere(sphere_vertices[i],sphere_vertices_normals[i],temp_orig[i])[k];

			//copy over to temp_dec to either make a height mapped sphere or a flat cube
			if (is_sphere == 1){
				temp_dec[i][k] = heightmapped_sphere_vertices[i][k];
				//temp_dec[i][k] = world_vert[i][k];
			}
			else if (is_sphere == 2) {
				//draw the unaltered plane
				temp_dec[i][k] = temp_orig[i][k];
			}
			else {
				temp_dec[i][k] = temp[i][k];
			}
				
			//copy over to global sphere terrain and normals variables
			world_norm[i][k]=sphere_vertices_normals[i][k];
			world_vert[i][k]=temp_dec[i][k];
		}
	}
	
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
	//PlaneToSpherePointConvert INSIDE TERRAIN DRAW
	//move character on top of the sphere, if he is within the box
	//vertex order is: enum{bot_left,top_left, top_right, bot_right} 
	if ((*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X) >= temp_orig[0][0]) &&
	   (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z) >= temp_orig[0][2]) &&
	   (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X) < (temp_orig[0][0]+STEP_SIZE)) &&
	   (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z) < (temp_orig[0][2]+STEP_SIZE)) && (0)
	   )
	{
		//in X
		percentStepSizeTraversedX = (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X) - temp_orig[0][0])/STEP_SIZE;
		charSpherePos[0] = temp_dec[0][0]-((temp_dec[0][0]-temp_dec[3][0])*percentStepSizeTraversedX);
		//in Z
		percentStepSizeTraversedZ = (*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z) - temp_orig[0][2])/STEP_SIZE;
		charSpherePos[2] = temp_dec[0][2]-((temp_dec[0][2]-temp_dec[1][2])*percentStepSizeTraversedZ);
		//in Y
		charSpherePos[1] = BiInterpol(temp_dec,charSpherePos[0],charSpherePos[2]);//+4.9;
		//printf("root %f %f %f \n",*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X), *joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Y), *joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_Z));
		//printf("temp_orig %f %f %f \n",temp_orig[0][0], temp_orig[0][1], temp_orig[0][2]);
		printf("world_vert %f %f %f \n",world_vert[0][0], world_vert[0][1], world_vert[0][2]);
		//printf("world_vertnew %f ",world_vert[3][0], world_vert[0][1], world_vert[0][2]);
		//printf("root %f ",(*joint_ui_data->getDOFPtr(Keyframe::ROOT_TRANSLATE_X)));
		//printf("tempOrig %f ",(temp_orig[0][0]));
		//printf("perc %f ",percentStepSizeTraversed);
		printf("charSpherePos %f %f %f\n",charSpherePos[0], charSpherePos[1], charSpherePos[2]);
		//NORMALS//////////////////////////////////////////////////////////////
		float xyangle = VectorAngleTan (0+0.000001,1+0.000001,world_norm[0][0]+0.000001,world_norm[0][1]+0.000001);
		float yzangle = VectorAngleTan (0+0.000001,1+0.000001,world_norm[0][2]+0.000001,world_norm[0][1]+0.000001);
		float xyangleNext = VectorAngleTan (0+0.000001,1+0.000001,world_norm[3][0]+0.000001,world_norm[3][1]+0.000001);
		float yzangleNext = VectorAngleTan (0+0.000001,1+0.000001,world_norm[1][2]+0.000001,world_norm[1][1]+0.000001);
		printf("world_norm %f %f %f ",world_norm[0][0], world_norm[0][1], world_norm[0][2]);
		//charAngle[0] = xyangle/(PI/180);
		//charAngle[1] = yzangle/(PI/180);
		charAngle[0] = xyangle/(PI/180)+percentStepSizeTraversedX*((xyangleNext/(PI/180))-(xyangle/(PI/180)));
		charAngle[1] = yzangle/(PI/180)+percentStepSizeTraversedZ*((yzangleNext/(PI/180))-(yzangle/(PI/180)));;
		charNorm[0] = world_norm[0][0];
		charNorm[1] = world_norm[0][1];
		charNorm[2] = world_norm[0][2];
		printf ("xyangle %f yzangle %f \n",charAngle[0],charAngle[1]);
		//joint_ui_data->setDOF(Keyframe::ROOT_ROTATE_X,-yzangle/(PI/360));
		//joint_ui_data->setDOF(Keyframe::ROOT_ROTATE_Z,xyangle/(PI/360)+360);
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
	GLuint tex1 = raw_texture_load ("texture/mars.raw", MAP_SIZE, MAP_SIZE, 2);

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
	printf("whichFaceInternal %d \n",whichFaceInternal);
	return whichFaceInternal;
}

void PlaneToSpherePointConvert (float X, float Z, float* internalCharSpherePosX,float* internalCharSpherePosY,float* internalCharSpherePosZ)
{////for good debug prints, uncomment all printf's with 4 backslashes
	Vertex internalPosition ={0};
	Vertices internalStepBox = {0};
	Vertices normalStepBox = {0};
	//temps since we use this function for Y position only now
	float internalX;
	float internalZ;
	int nextxMap = 0; //for that wierd corner case
	int nextzMap = 0; //for that wierd corner case
	int xMapRevert = 0;
	int zMapRevert = 0;
	//printf("pointconvert enter \n");
	//while (record_keeping){};
	for (int xMap = 0;xMap<MAP_SIZE/STEP_SIZE; xMap++){
		for (int zMap = 0;zMap<MAP_SIZE/STEP_SIZE; zMap++){
			//move character on top of the sphere, if he is within the box
			//vertex order is: enum{bot_left,top_left, top_right, bot_right}
			if ((X >= temp_orig_record[xMap][zMap][0]) && (Z >= temp_orig_record[xMap][zMap][2]) &&
			   (X < (temp_orig_record[xMap][zMap][0]+STEP_SIZE)) && (Z < (temp_orig_record[xMap][zMap][2]+STEP_SIZE)) &&
			   (animate))
			{
				nextxMap = xMap+1;
				nextzMap = zMap+1;
				///////////////////////////////////////////////////////////EDGE CASES HANDLING (WORKS ON PLANE ONLY?)/////////////////////////////////////////////////////
				if((xMap==32)&&(face_id==2)&&(world_vert_record[xMap][zMap][2]==752)) //left to front blanks out for one frame
					{xMap=31;xMapRevert=1;
					////printf("reverting\n");
					}
				if ((xMap==31)&&((face_id==3)||(face_id==2)))//front to left (and vice versa)
					{nextxMap = xMap+2;
					////printf("reverting0.5\n");
				}
				else
					nextxMap = xMap+1;
				
				if((zMap==32)&&((face_id==4)||(face_id==5))&&(world_vert_record[xMap][zMap][2]==496)) //left to back blanks out for one frame (dont increment nextxMap by 2 so happening after)
					{zMap=31;zMapRevert=1;nextzMap = zMap+1;
					////printf("reverting1\n");
					}
					
				if((xMap==32)&&(face_id==2)&&(world_vert_record[xMap][zMap][2]==496)) //left to back blanks out for one frame (dont increment nextxMap by 2 so happening after)
					{xMap=31;xMapRevert=1;nextxMap = xMap+1;
					////printf("reverting2\n");
					}
					
				if ((zMap==31)&&((face_id==5)||(face_id==4)))//right to back blank out fix
					{world_vert_record[xMap][nextzMap][2] = 512;
					//nextzMap = zMap+2;
					////printf("reverting3\n");
					}
				else
					{nextzMap = zMap+1;
					////printf("reverting4\n");
					}
				
				if ((world_vert_record[xMap][zMap][2] == 496)&&((face_id==2)||(face_id==4)||(face_id==50)||(face_id==6)))//left to back blankout fix
					{world_vert_record[xMap][nextzMap][2] = 512;
					////printf("reverting5\n");
					}
					
				if((zMap==32)&&(face_id==4)&&(world_vert_record[xMap][zMap][2]==752)) //back to left blanks out for one frame
					{zMap=31;zMapRevert=1;
					////printf("reverting6\n");
					}
				///////////////////////////////////////////////////////////EDGE CASES HANDLING DONE/////////////////////////////////////////////////////
				//in X
				percentStepSizeTraversed = (X - temp_orig_record[xMap][zMap][0])/STEP_SIZE;
				
				////printf("X face %d, percent %f world %f worldnext %f xMap %d nextxMap %d\n",face_id,percentStepSizeTraversed,world_vert_record[xMap][zMap][0],world_vert_record[nextxMap][zMap][0],xMap,nextxMap);
								
				internalX = world_vert_record[xMap][zMap][0]-((world_vert_record[xMap][zMap][0]-world_vert_record[nextxMap][zMap][0])*percentStepSizeTraversed);
				//in Z
				percentStepSizeTraversed = (Z - temp_orig_record[xMap][zMap][2])/STEP_SIZE;
				
				////printf("Z face %d, percent %f world %f worldnext %f zMap %d nextzMap %d\n",face_id,percentStepSizeTraversed,world_vert_record[xMap][zMap][2],world_vert_record[xMap][nextzMap][2],zMap,nextzMap);
				
				internalZ = world_vert_record[xMap][zMap][2]-((world_vert_record[xMap][zMap][2]-world_vert_record[xMap][nextzMap][2])*percentStepSizeTraversed);
				//in Y
				for (int a = 0;a<3;a++)
				{
					internalStepBox[0][a] = world_vert_record[xMap][zMap][a];
					internalStepBox[1][a] = world_vert_record[xMap][nextzMap][a];
					internalStepBox[2][a] = world_vert_record[nextxMap][nextzMap][a];
					internalStepBox[3][a] = world_vert_record[nextxMap][zMap][a];
					
					//normalStepBox[0][a] = world_norm_record[xMap][zMap][a];
					//normalStepBox[1][a] = world_norm_record[xMap][nextzMap][a];
					//normalStepBox[2][a] = world_norm_record[nextxMap][nextzMap][a];
					//normalStepBox[3][a] = world_norm_record[nextxMap][zMap][a];
					if ((world_norm_record[xMap][zMap][a] == 0)|| (world_norm_record[xMap][zMap][a] == 1))
					{
						world_norm_record[xMap][zMap][a] += 0.0001;
					}
					
				}
				//printf("insidepercentStepSizeTraversed %f world %f worldnext %f step %d\n",percentStepSizeTraversed,world_vert_record[xMap][zMap][0],world_vert_record[xMap+1][zMap][0],xMap+1);			
				*internalCharSpherePosY = BiInterpol(internalStepBox,internalX,internalZ)+4.9;
				
				//NORMALS///////////////////////////////////////////////////////////////////////////
				float xyangle = VectorAngle (0,1,world_norm_record[xMap][zMap][0],world_norm_record[xMap][zMap][1]);
				float yzangle = VectorAngle (0,1,world_norm_record[xMap][zMap][2],world_norm_record[xMap][zMap][1]);
				printf("world_norm_record %f %f %f ",world_norm_record[xMap][zMap][0], world_norm_record[xMap][zMap][1], world_norm_record[xMap][zMap][2]);
				printf ("xyangle %f yzangle %f \n",xyangle/(PI/360),yzangle/(PI/360));

			}
			if (xMapRevert==1)
			{
				xMap = 32;//32 is evil number 
				xMapRevert = 0;
			}
			if (zMapRevert==1)
			{
				zMap = 32;//32 is evil number 
				zMapRevert = 0;
			}			
		}
	}
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
			face_id=5;
		}
		//right to back
		else if ((currentFace==5)&&(*FuncPropel >= FACE_SIZE*2))
		{
			*FuncPropel = *FuncPropel+(FACE_SIZE-*FuncStrafe);
			*FuncStrafe = FACE_SIZE;
			*turnAngle+=90;
			printf("right to back\n");
			face_id=4;
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

float VectorAngle(float x, float y, float x1, float y1)
{
    return atan2f(y1-y, x1-x);
}

float VectorAngleTan(float x, float y, float x1, float y1)
{
    float dot = x*x1 + y*y1;
	float det = x*y1 + y*x1;
	return atan2f(det, dot);
}

float VectorAngleCos(float x, float y, float x1, float y1)
{
    float dot = x*x1 + y*y1;
	float firstsize = sqrtf((x*x)+(y*y));
	float secondsize = sqrtf((x1*x1)+(y1*y1));
	printf("dot %f 1st %f 2nd %f \n",dot,firstsize,secondsize);
	return acos(dot/(firstsize*secondsize));
}
