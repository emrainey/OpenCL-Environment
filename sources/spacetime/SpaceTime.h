/*****************************************************************************
 *
 *  Lagrangian Point Simulation
 *
 *  by: Erik Rainey
 *  04/26/99
 *
 *
 ****************************************************************************/

/*****************************************************************************
 *
 * INCLUDES
 *
 ****************************************************************************/

#ifndef _INC_SPACETIME_H_
#define _INC_SPACETIME_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

#ifndef __APPLE__
#include <CL/cl.h>
#else
#include <OpenCL/opencl.h>
#endif
#include <clenvironment.h>

/* OPENGL INCLUDES */
#include <Glut.h>

/*****************************************************************************
 *
 * DEFINES
 *
 ****************************************************************************/

#define PI                      (3.141592653)

#define FPS                     (30)
#define MIN_FRAMES              (30)

#define ACCEL_RADIUS            (3000)
#define MAX_ZOOM                (100000)
#define MIN_ZOOM                (20)
#define MAX_X                   (1500)

#define R   (0)
#define G   (1)
#define B   (2)
#define A   (3)

#define X   (0)
#define Y   (1)
#define Z   (2)
#define W   (3)

#define X_DIM   (21)
#define Y_DIM   (21)
#define Z_DIM   (21)

/*****************************************************************************
 *
 * DEBUG
 *
 ****************************************************************************/

#undef PRINT_KEY
#undef DEBUG
#define BUILD_DATE
#undef PHYSICS_DEBUG
#undef PRINT_COMPATIBILITY
#undef TEXTURES

/*****************************************************************************
 *
 * ERROR CODES
 *
 ****************************************************************************/


/*****************************************************************************
 *
 * MACROS
 *
 ****************************************************************************/

#define TOGGLE(a)    a^=1

/*****************************************************************************
 *
 * enum's, typedef's and struct's
 *
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _vec {
    double x;
    double y;
    double z;
    double w;
} Vec;

typedef unsigned char byte;

typedef struct _colorb
{
    byte r,g,b,a;
} Colorb;

typedef struct _colorf
{
    float r,g,b,a;
} Colorf;

typedef struct _point
{
    Vec p,f,v;
} Point;

typedef struct _light
{
    Vec v;
    float position[4];
    float ambient[4];
    float diffuse[4];
} Light;

typedef struct _body
{
    Vec p,v;
    double mass;
    double radius;
    double t;
#ifdef TEXTURES
    Colorf texture[64][64];
#endif
} Body;

typedef struct _camera {
    Vec from;                       //position of camera
    Vec at;                         //where camera points
    Vec up;                         //orientation of camera
    Vec pan;                        //Vector to pan to new object
    double pan_reps;                //Number of times the pan will be calculated to reach the specified target
    int state;
    double radius;                  //distance away from the target
    double theta;                   //in radians
    double phi;                     //in radians
    double accel;
} Camera;

/*****************************************************************************
 *
 * GLOBAL VARIABLES
 *
 ****************************************************************************/

Camera camera;

int WindowHeight;
int WindowWidth;

char RenderText;
char RenderLines;
char RenderHelp;
char RenderMovement;

double DepthBufferMax;

clock_t Start, Finish, Timer;
double Fps;
int Frames;
int RenderFrame;
int CalculateMovement;

Body body1,unit_body;

Light light1;

Point STGrid[X_DIM][Y_DIM][Z_DIM];

/*****************************************************************************
 *
 * FUNCTION METHODS
 *
 ****************************************************************************/

void movesystem(void);

void initcamera(void);
void placecamera(void);
void movecamera(void);
void updatecamera(void);

void initscreen(void);
void drawscreen(void);
void frameratelimiter(int value);
void reshapescreen(int width, int height);

void init(int reinit);

void speckeyevent(int key, int x, int y);
void keyboardevent(unsigned char key, int x, int y);

int random_i(int low, int high);
double random_d(double low, double high);
int round_d(double x);

// Vec.c
double VecAbs(Vec *a);
void VecScale(Vec *a, double f);
void VecNormalize(Vec *a);

// physics.c
Vec gravity(Body one, Body two);

#ifdef __cplusplus
}
#endif

#endif
