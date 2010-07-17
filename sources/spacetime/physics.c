/***********************************************************************
 *    Title:  physics.c
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

#define GRAV_CONS			    (0.0000000000667)   //Graviational constant of the universe
#define C                       (300000000)         //Speed of Light (m/s)

/***********************************************************************
 *GLOBAL FUNCTIONS
 ***********************************************************************/

Vec gravity(Body one, Body two)
{
    //returns the gravity Vec resulting from body one on body two. i.e. Sun, Earth
    static double F;
    static double r;
    static Vec a;
	static double mass;

   	a.x = -(two.p.x-one.p.x);
   	a.y = -(two.p.y-one.p.y);
   	a.z = -(two.p.z-one.p.z);

    r = VecAbs(&a);

	if(r<one.radius)
	{
		mass = one.mass*((4/3)*PI*pow(r,3))/((4/3)*PI*pow(one.radius,3));
	}
	else
	{
		mass = one.mass;
	}

    F = (GRAV_CONS*mass*two.mass)/(pow(r,2));
    VecNormalize(&a);
    VecScale(&a,F);
    return a;
}

