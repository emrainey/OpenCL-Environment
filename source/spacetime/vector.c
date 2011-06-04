
#include <SpaceTime.h>

double VecAbs(Vec *a)
{
    double dx = a->x;
    double dy = a->y;
    double dz = a->z;
    double dw = a->w;
    return sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
}

void VecScale(Vec *a, double f)
{
    a->x *= f;
    a->y *= f;
    a->z *= f;
    a->w *= f;
}

void VecNormalize(Vec *a)
{
    double m = VecAbs(a);
    VecScale(a, 1/m);
}

