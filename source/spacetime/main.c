/***********************************************************************
 *    Title:  main.c
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


/***********************************************************************
 *STATIC FUNCTIONS
 ***********************************************************************/


/***********************************************************************
 *MAIN
 ***********************************************************************/

int main(int argc, char** argv)
{
    char title[80];

    sprintf(title,"Space-Time Warping Due to Mass; Simulation by Erik Rainey");

    DepthBufferMax = 1000000;

    Timer = 0;

    printf("Initializing Window: %s\n",title);

    glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE|GLUT_MULTISAMPLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
    glutInit(&argc, argv);
    glutCreateWindow(title);

    init(0);

    printf("Registering Callbacks!\n");

    glutKeyboardFunc(keyboardevent);
    glutSpecialFunc(speckeyevent);
    glutDisplayFunc(drawscreen);
    glutReshapeFunc(reshapescreen);
    CalculateMovement = 1;
    glutIdleFunc(NULL);
    //glutIdleFunc(movesystem);
    RenderFrame = 1;
    glutTimerFunc(1000/FPS,frameratelimiter, 0);
    Fps = FPS;
    Start = clock();
    Finish = 0;

    printf("Entering GLUT Main Loop!\n");

    glutMainLoop();

    return 0;
}
