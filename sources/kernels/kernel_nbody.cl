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

__kernel void kernel_magnitude(__global float4 *p,
                               __global float *mag)
{
    int i = get_global_id(0);
    // mag = sqrt(x^2+y^2+z^2)
    mag[i] = sqrt(p[i][0]*p[i][0] + p[i][1]*p[i][1] + p[i][2]*p[i][2]);
}

__kernel void kernel_renormalize(__global float4 *p,
                                 __global float *om,
                                 __global float *nm)
{
    int i = get_global_id(0);

    kernel_magnitude(p,om);

    p[i][0] /= om[i];
    p[i][1] /= om[i];
    p[i][2] /= om[i];

    p[i][0] *= nm[i];
    p[i][1] *= nm[i];
    p[i][2] *= nm[i];
}

__kernel void kernel_distance(__global float4 *v1,
                              __global float4 *v2,
                              __global float *d)
{
    int i = get_global_id(0);

    float dx = v2[i][0] - v1[i][0];
    float dy = v2[i][1] - v1[i][1];
    float dz = v2[i][2] - v1[i][2];

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

__kernel kernel_kinetic(__global float *m,
                        __global float4 *v,
                        __global float4 *ke)
{
    int i = get_global_id(0);

    ke[i][0] = 0.5 * m[i] * v[i][0] * v[i][0];
    ke[i][1] = 0.5 * m[i] * v[i][1] * v[i][1];
    ke[i][2] = 0.5 * m[i] * v[i][2] * v[i][2];
}

__kernel kernel_energy(__global *m,
                       __global *E)
{
    int i = get_global_id(0);
    float c = 299792458; // speed of light in meters/second;
    E[i] = m[i] * c * c;
}

__kernel kernel_nbody(__global float *ms,
					  __global float *m,
                      __global float4 *a,
                      __global float4 *v,
                      __global float4 *p,
                      __global float *t,
                      __global float *d,
                      __global float *g)
{
    int i = get_global_id(0);
    float G = 0.00000000006673;
	
	// position is updated by velocity and some small quantity of acceration
    p[i][0] += v[i][0]*t[i] + 0.5*t[i]*t[i]*a[i][0];
    p[i][1] += v[i][1]*t[i] + 0.5*t[i]*t[i]*a[i][1];
    p[i][2] += v[i][2]*t[i] + 0.5*t[i]*t[i]*a[i][2];

    // velocity is updated by acceration and friction (which is zero in space)
    v[i][0] += a[i][0]*t[i];
    v[i][1] += a[i][1]*t[i];
    v[i][2] += a[i][2]*t[i];

    // calculate the magnitude of the position with respec to 0,0,0 (this is a shortcut for distance)
    d[i] = sqrt(p[i][0]*p[i][0] + p[i][1]*p[i][1] + p[i][2]*p[i][2]);
    
    // gravity is based on position and mass.
    g[i] = (ms[i] * m[i] * G)/(d[i]*d[i]);
    
    // acceleration is updated by changes in gravity, which is based on position
    a[i][0] = -p[i][0]*g[i]/d[i];
    a[i][1] = -p[i][1]*g[i]/d[i];
    a[i][2] = -p[i][2]*g[i]/d[i];
}


