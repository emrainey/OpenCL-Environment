/***********************************************************************
 *    Title:  movement.c
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

#define GRAVITY_FUDGE           (pow(10,-6.5))

/***********************************************************************
 *MACRO's
 ***********************************************************************/



/***********************************************************************
 *STATICS (VAR's, PROTOTYPES, and STRUCTS)
 ***********************************************************************/



 /***********************************************************************
 *GLOBAL FUNCTIONS
 ***********************************************************************/

void movesystem(void)
{
    int i,j,k;

    if (CalculateMovement == 0)
        return;
    else
        CalculateMovement = 0;

    if(RenderMovement)
    {
        double mag = VecAbs(&body1.p);
        printf("Mag of Position = %lf\n",mag);
        if(mag > MAX_X)
        {
            printf("Wrapping Body 1 to new position!\n");
            body1.p.x = -MAX_X;
            body1.p.y = 0;
            body1.p.z = 0;
            body1.p.w = 0;
        }

        body1.p.x += body1.v.x;
        body1.p.y += body1.v.y;
        body1.p.z += body1.v.z;
        body1.p.w = 0;

        printf("Body: {%lf,%lf,%lf} => %{%lf,%lf,%lf}\n",
                body1.p.x,body1.p.y,body1.p.z,
                body1.v.x,body1.v.y,body1.v.z);

        for(i=0;i<X_DIM;i++)
        {
            for(j=0;j<Y_DIM;j++)
            {
                for(k=0;k<Z_DIM;k++)
                {
                    unit_body.p.x = STGrid[i][j][k].p.x;
                    unit_body.p.y = STGrid[i][j][k].p.y;
                    unit_body.p.z = STGrid[i][j][k].p.z;
                    STGrid[i][j][k].f = gravity(body1,unit_body);

                    STGrid[i][j][k].f.x *= GRAVITY_FUDGE;
                    STGrid[i][j][k].f.y *= GRAVITY_FUDGE;
                    STGrid[i][j][k].f.z *= GRAVITY_FUDGE;

                    STGrid[i][j][k].v.x = STGrid[i][j][k].f.x + STGrid[i][j][k].p.x;
                    STGrid[i][j][k].v.y = STGrid[i][j][k].f.y + STGrid[i][j][k].p.y;
                    STGrid[i][j][k].v.z = STGrid[i][j][k].f.z + STGrid[i][j][k].p.z;
                }
            }
        }
    }

    movecamera();

    //glutPostRedisplay();
}


