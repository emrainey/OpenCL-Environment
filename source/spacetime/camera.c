/***********************************************************************
 *    Title:  camera.c
 *    Author: Erik Rainey
 *    Date:   04/26/99
 ***********************************************************************/

/***********************************************************************
 *INCLUDE
 ***********************************************************************/

#include "SpaceTime.h"

/***********************************************************************
 *DEFINE
 ***********************************************************************/



/***********************************************************************
 *MACRO's
 ***********************************************************************/



/***********************************************************************
 *STATICS (VAR's, PROTOTYPES, and STRUCTS)
 ***********************************************************************/


/***********************************************************************
 *GLOBAL FUNCTIONS
 ***********************************************************************/

void initcamera(void)
{
    memset(&camera,0,sizeof(Camera));

    camera.radius = 200;
    camera.theta = PI/2;
    camera.phi = PI/2;
    camera.up.x = 0;
    camera.up.y = 0;
    camera.up.z = 1;

}

void placecamera(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0,(GLfloat)WindowWidth/WindowHeight, 1, DepthBufferMax);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(camera.from.x,camera.from.y,camera.from.z,
              camera.at.x,  camera.at.y,  camera.at.z,
              camera.up.x,  camera.up.y,  camera.up.z);
}

void movecamera(void)
{
    //now we figure out the from vector
    //take the "at" location and add the sphereical translated coordinates
    camera.from.x = camera.at.x + camera.radius*cos(PI/2 - camera.phi)*cos(camera.theta);
    camera.from.y = camera.at.y + camera.radius*cos(PI/2 - camera.phi)*sin(camera.theta);
    camera.from.z = camera.at.z + camera.radius*cos(camera.phi);
}


/***********************************************************************
 *STATIC FUNCTIONS
 ***********************************************************************/
