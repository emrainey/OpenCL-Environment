    /***********************************************************************
 *    Title:  keyboard.c
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

void keyboardevent(unsigned char key, int x, int y)
{
#ifdef PRINT_KEY
    printf("keyboardevent(): key \"%c\" [0x%02x]\n",key,key);
#endif
    switch(key)
    {
    case 27:
        exit(0);
        break;
    case 'w':
        camera.phi -= PI/200;
        if(camera.phi<=0)
            camera.phi = 0;
        break;
    case's':
        camera.phi += PI/200;
        if(camera.phi>=PI)
            camera.phi = PI;
        break;
    case 'd':
        camera.theta += PI/100;
        break;
    case 'a':
        camera.theta -= PI/100;
        break;
    case 'q':
        if(camera.radius < ACCEL_RADIUS)
            camera.accel = 40;
        else if(camera.radius < MAX_ZOOM)
            camera.accel += 20;
        camera.radius += camera.accel;
        if(camera.radius >= MAX_ZOOM)
            camera.radius = MAX_ZOOM;
        break;
    case 'e':
        if(camera.radius < ACCEL_RADIUS)
            camera.accel = 40;
        else if(camera.radius < MAX_ZOOM)
            camera.accel -= 20;
        if(camera.accel < 40)
            camera.accel = 40;
        camera.radius -= camera.accel;
        if(camera.radius <= MIN_ZOOM)
            camera.radius = MIN_ZOOM;
        break;
    case 'r':
        TOGGLE(RenderText);
        TOGGLE(RenderLines);
        break;
    case 'f':
        TOGGLE(RenderMovement);
        break;
    case 'v':
        glutFullScreen();
        glutSetCursor(GLUT_CURSOR_NONE);
        break;
    case 'b':
        glutReshapeWindow(WindowWidth/2,WindowHeight/2);
        glutPositionWindow(50,50);
        glutSetCursor(GLUT_CURSOR_INHERIT);
        break;
    case 'h':
        TOGGLE(RenderHelp);
        break;
    case 'g':
        init(1);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void speckeyevent(int key, int x, int y)
{
#ifdef PRINT_KEY
    printf("speckeyevent(): key [0x%08x]\n",key,key);
#endif
}

/***********************************************************************
 *STATIC FUNCTIONS
 ***********************************************************************/


