/***********************************************************************
 *    Title:  display.c
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


static void RenderScreenText(void);
static void RenderGrid(void);
static void print2DText(int x, int y, void *font, char *format, ...);
static void print3DText(Vec p, void *font, char *format, ...);


Vec text;

GLfloat mat_ambient[]  = {0,0,0,1};
GLfloat mat_diffuse[]  = {0.5,0.5,0.5,1};
GLfloat mat_specular[] = {1,1,1,1};
GLfloat mat_shiny = 100;
GLfloat mat_emission[] = {0,0,0,1};

/***********************************************************************
 *GLOBAL FUNCTIONS
 ***********************************************************************/
void initscreen(void)
{
    glClearColor(0, 0, 0, 1);

    text.x = 0;
    text.y = 0;
    text.z = 0;
    text.w = 0;

    glEnable(GL_CULL_FACE);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &mat_shiny);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    light1.ambient[R] = 0;
    light1.ambient[G] = 0;
    light1.ambient[B] = 0;
    light1.ambient[A] = 1;

    light1.diffuse[R] = 1;
    light1.diffuse[G] = 1;
    light1.diffuse[B] = 1;
    light1.diffuse[A] = 1;

    light1.position[X] = 20;
    light1.position[Y] = 20;
    light1.position[Z] = 20;
    light1.position[W] = 0;

    light1.v.x = 0;
    light1.v.y = 0;
    light1.v.z = 0;
    light1.v.w = 0;

    glLightfv(GL_LIGHT0, GL_AMBIENT, light1.ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light1.diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light1.position);

}

void drawscreen(void)
{
    // don't render until the timer fires
    if (RenderFrame == 0)
        return;
    else
        RenderFrame = 0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glScalef(1.0,1.0,1.0);
    glDepthRange(-1.0,1.0);

    // puts camera into the scene
    placecamera();

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);

    RenderGrid();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //draw the Mass
    glColor3ub(100,106,187);
    glPushMatrix();
    glTranslated(body1.p.x,body1.p.y,body1.p.z);
    glutSolidSphere(body1.radius,20,20);
    glPopMatrix();

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);

    // this is will render all the screen text
    RenderScreenText();

    glutSwapBuffers();

    Frames++;
    Finish = clock();

    // if more than a second of time has elapsed, calculate the frame rate
    if ((Finish-Start) > CLOCKS_PER_SEC)
    {
        Fps = (double)Frames / ((double)(Finish-Start)/CLOCKS_PER_SEC);
        Start = clock();
        Frames = 0;
    }
}

void frameratelimiter(int value)
{
    GLenum err = glGetError();
    GLubyte *pErr = gluErrorString(err);
    //if (err != 0)
    {
        GLfloat v;
        GLfloat range[2];
        glGetFloatv(GL_POINT_SIZE,&v);
        glGetFloatv(GL_DEPTH_RANGE, range);
        printf("Timer Fired! (%d %s)\n",err,pErr);
        printf("Point Size: %lf\n", v);
        printf("Depth Range: near %lf to far %lf\n", range[0], range[1]);
        fflush(stdout);
    }
    CalculateMovement = 1;
    movesystem();
    // Enable a single frame to render
    glutPostRedisplay();
    RenderFrame = 1;
    // Reregister this function to fire later
    glutTimerFunc(1000/FPS,frameratelimiter, 0);
}

/*****************************************************************************
 *
 *
 *
 ****************************************************************************/
void reshapescreen(int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, width, height);
    placecamera();
}

/***********************************************************************
 *STATIC FUNCTIONS
 ***********************************************************************/

/*****************************************************************************
 *
 *
 *
 ****************************************************************************/
static void print3DText(Vec p, void *font, char *format, ...)
{
    int i,len;
    va_list args;
    static char buffer[255];

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    glRasterPos3d(p.x,p.y,p.z);
    len = strlen(buffer);

    for(i=0;i<len;i++)
        glutBitmapCharacter(font,buffer[i]);

}
/*****************************************************************************
 *
 *
 *
 ****************************************************************************/
static void print2DText(int x, int y, void *font, char *format, ...)
{
    int i,len;
    va_list args;
    static char buffer[255];

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    glRasterPos2i(x,y);
    len = strlen(buffer);
    for(i=0;i<len;i++)
        glutBitmapCharacter(font,buffer[i]);
}

/*****************************************************************************
 *
 *
 *
 ****************************************************************************/
static void RenderScreenText(void)
{
    int start_point=0;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0,(float) WindowWidth,(float) WindowHeight,0.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.0,1.0,1.0);

    print3DText(body1.p, GLUT_BITMAP_HELVETICA_10, "Body");

    if(RenderText)
    {
        start_point += 15;
        print2DText(5,start_point += 15,GLUT_BITMAP_HELVETICA_10,"f.p.s.: %lf",Fps);
        print2DText(5,start_point += 15,GLUT_BITMAP_HELVETICA_10,"Run Time: %lf secs",(float)Timer/CLOCKS_PER_SEC);
#ifdef BUILD_DATE
        print2DText(5,WindowHeight-5,GLUT_BITMAP_HELVETICA_10,"Compiled on %s at %s",__DATE__,__TIME__);
#endif
    }
    if(RenderHelp)
    {
        start_point = WindowHeight/4;

        print2DText(WindowWidth/3,start_point    ,GLUT_BITMAP_HELVETICA_10,"Help Menu");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"-----------------------------------");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"h - This help screen");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"r - Switch between text view and grid view");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"f - start simulation");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"w,a,s,d - Moves camera up,left, down and right respectively.");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"q,e - Zooms camera out and in.");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"v - Fullscreen mode.");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"b - Quarter the screen size.");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"g - Reinitialize the body position.");
        print2DText(WindowWidth/3,start_point+=15,GLUT_BITMAP_HELVETICA_10,"ESC - Quit");
    }
}

/*****************************************************************************
 *
 *
 *
 ****************************************************************************/
void RenderGrid(void)
{
    int i,j,k;
    GLenum err;

    if(RenderLines)
    {
        glLineWidth(1.0);
        glEnable(GL_LINE_SMOOTH);
        glColor4ub(255,255,255,0);
        err = glGetError();
        if (err) printf("Error before glBegin/glEnd\n");
        glBegin(GL_LINES);
        for(i=0;i<X_DIM-1;i++)
        {
            for(j=0;j<Y_DIM-1;j++)
            {
                for(k=0;k<Z_DIM-1;k++)
                {
                    glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                    glVertex3f(STGrid[i+1][j][k].v.x,STGrid[i+1][j][k].v.y,STGrid[i+1][j][k].v.z);

                    glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                    glVertex3f(STGrid[i][j+1][k].v.x,STGrid[i][j+1][k+1].v.y,STGrid[i][j+1][k].v.z);

                    glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                    glVertex3f(STGrid[i][j][k+1].v.x,STGrid[i][j][k+1].v.y,STGrid[i][j][k+1].v.z);
                }
            }
        }

        //cap off the k side
        k = Z_DIM - 1;
        for(i=0;i<X_DIM-1;i++)
        {
            for(j=0;j<Y_DIM-1;j++)
            {
                glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                glVertex3f(STGrid[i+1][j][k].v.x,STGrid[i+1][j][k].v.y,STGrid[i+1][j][k].v.z);

                glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                glVertex3f(STGrid[i][j+1][k].v.x,STGrid[i][j+1][k].v.y,STGrid[i][j+1][k].v.z);

            }
        }

        //cap off the j side
        j = Y_DIM - 1;
        for(i=0;i<X_DIM-1;i++)
        {
            for(k=0;k<Z_DIM-1;k++)
            {
                glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                glVertex3f(STGrid[i+1][j][k].v.x,STGrid[i+1][j][k].v.y,STGrid[i+1][j][k].v.z);

                glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                glVertex3f(STGrid[i][j][k+1].v.x,STGrid[i][j][k+1].v.y,STGrid[i][j][k+1].v.z);

            }
        }

        //cap off the i side
        i = X_DIM - 1;
        for(j=0;j<Y_DIM-1;j++)
        {
            for(k=0;k<Z_DIM-1;k++)
            {
                glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                glVertex3f(STGrid[i][j+1][k].v.x,STGrid[i][j+1][k].v.y,STGrid[i][j+1][k].v.z);

                glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                glVertex3f(STGrid[i][j][k+1].v.x,STGrid[i][j][k+1].v.y,STGrid[i][j][k+1].v.z);

            }
        }

        //cap off the end line of i
        j = Y_DIM - 1;
        k = Z_DIM - 1;
        for(i=0;i<X_DIM-1;i++)
        {
            glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
            glVertex3f(STGrid[i+1][j][k].v.x,STGrid[i+1][j][k].v.y,STGrid[i+1][j][k].v.z);
        }

        //cap off the end line of i
        i = X_DIM - 1;
        k = Z_DIM - 1;
        for(j=0;j<Y_DIM-1;j++)
        {
            glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
            glVertex3f(STGrid[i][j+1][k].v.x,STGrid[i][j+1][k].v.y,STGrid[i][j+1][k].v.z);
        }

        //cap off the end line of i
        i = X_DIM - 1;
        j = Y_DIM - 1;
        for(k=0;k<Z_DIM-1;k++)
        {
            glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
            glVertex3f(STGrid[i][j][k+1].v.x,STGrid[i][j][k+1].v.y,STGrid[i][j][k+1].v.z);
        }

        glEnd();
        glDisable(GL_LINE_SMOOTH);
    }
    else
    {
        //printf("Rendering Points\n");
        glPointSize(1.0);
        glEnable(GL_POINT_SMOOTH);
        glColor4ub(255,255,255,0);
        err = glGetError();
        if (err) printf("Error before glBegin/glEnd\n");
        glBegin(GL_POINTS);
        for(i=0;i<X_DIM;i++)
        {
            for(j=0;j<Y_DIM;j++)
            {
                for(k=0;k<Z_DIM;k++)
                {
                    glVertex3f(STGrid[i][j][k].v.x,STGrid[i][j][k].v.y,STGrid[i][j][k].v.z);
                }
            }
        }
        glEnd();
        glDisable(GL_POINT_SMOOTH);
    }
    return;
}
