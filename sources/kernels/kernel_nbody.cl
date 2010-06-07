/**
 * Copyright (C) 2010 Erik Rainey
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

__kernel void kernel_magnitude(__global float *x,
                               __global float *y,
                               __global float *z,
                               __global float *mag)
{
    int i = get_global_id(0);
    // mag = sqrt(x^2+y^2+z^2)
    mag[i] = sqrt(x[i]*x[i] + y[i]*y[i] + z[i]*z[i]);
}
/*
__kernel kernel_renormalize(__global float *x,
                            __global float *y,
                            __global float *z,
                            __global float *om,
                            __global float *nm)
{
    int i = get_global_id(0);

    kernel_magnitude(x,y,z,om);

    x[i] /= om[i];
    y[i] /= om[i];
    z[i] /= om[i];

    x[i] *= nm[i];
    y[i] *= nm[i];
    z[i] *= nm[i];
}

__kernel kernel_distance(__global float *v1_x,
                         __global float *v1_y,
                         __global float *v1_z,
                         __global float *v2_x,
                         __global float *v2_y,
                         __global float *v2_z,
                         __global float *d)
{
    int i = get_global_id(0);

    float dx = v2_x[i] - v1_x[i];
    float dy = v2_y[i] - v1_x[i];
    float dz = v2_z[i] - v1_z[i];

    // distance = sqrt(sum(dx^2 + dy^2 + dz^2))
    d[i] = sqrt(dx*dx + dy*dy + dz*dz);
}

__kernel kernel_gravity(__global float Mg,
                        __global float *m,
                        __global float *d,
                        __global float *f_mag)
{
    int i = get_global_id(0); // data fields are 1 dimensional

    // G = gravitational constant = 6.67300 × 10^-11 m^3 kg^-1 s^-2
    float G = 0.00000000006673;

    // force of gravity = (M1*M2*G)/d^2
    f_mag[i] = (Mg * m[i] * G)/(d[i] * d[i]);
}

__kernel kernel_kinetic(__global *m,
                        __global *v_x,
                        __global *v_y,
                        __global *v_z,
                        __global *k_x,
                        __global *k_y,
                        __global *k_z)
{
    int i = get_global_id(0);

    k_x[i] = 0.5 * m[i] * v_x[i] * v_x[i];
    k_y[i] = 0.5 * m[i] * v_y[i] * v_y[i];
    k_z[i] = 0.5 * m[i] * v_z[i] * v_z[i];
}

__kernel kernel_energy(__global *m,
                       __global *E)
{
    int i = get_global_id(0);
    float c = 299792458; // speed of light in meters/second;
    E[i] = m[i] * c * c;
}
*/
__kernel kernel_nbody(__global float *m,
                      __global float *a_x,
                      __global float *a_y,
                      __global float *a_z,
                      __global float *v_x,
                      __global float *v_y,
                      __global float *v_z,
                      __global float *p_x,
                      __global float *p_y,
                      __global float *p_z,
                      __global float *t,
                      __global float *d,
                      __global float *g)
{
    int i = get_global_id(0);
    float G = 0.00000000006673;

    // position is updated by velocity and some small quantity of acceration
    p_x[i] += v_x[i]*t[i] + 0.5*t[i]*t[i]*a_x[i];
    p_y[i] += v_y[i]*t[i] + 0.5*t[i]*t[i]*a_y[i];
    p_z[i] += v_z[i]*t[i] + 0.5*t[i]*t[i]*a_z[i];

    // velocity is updated by acceration and friction (which is zero in space)
    v_x[i] += a_x[i]*t[i];
    v_y[i] += a_y[i]*t[i];
    v_z[i] += a_z[i]*t[i];

    // calculate the magnitude of the position with respec to 0,0,0 (this is a shortcut for distance)
    d[i] = sqrt(p_x[i]*p_x[i] + p_y[i]*p_y[i] + p_z[i]*p_z[i]);
    //kernel_magnitude(p_x, p_y, p_z, d);

    // gravity is based on position and mass.
    g[i] = (10 * m[i] * G)/(d[i]*d[i]);
    //kernel_gravity(10.0, m, d, g_mag); //@TODO this is a fixed M0 mass of 10.0

    // acceration is updated by changes in gravity, which is based on position
    a_x[i] = -p_x[i]*g[i]/d[i];
    a_y[i] = -p_y[i]*g[i]/d[i];
    a_z[i] = -p_z[i]*g[i]/d[i];

    //kernel_renormalize(a_x, a_y, a_z, d, g_mag);
}



