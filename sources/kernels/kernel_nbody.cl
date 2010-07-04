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

	float dx = p[i].x*p[i].x;
	float dy = p[i].y*p[i].y;
	float dz = p[i].z*p[i].z;

    mag[i] = sqrt(dx*dx + dy*dy + dz*dz);
}

__kernel void kernel_renormalize(__global float4 *p,
                                 __global float *om,
                                 __global float *nm)
{
    int i = get_global_id(0);

    kernel_magnitude(p,om);

    p[i].x /= om[i];
    p[i].y /= om[i];
    p[i].z /= om[i];

    p[i].x *= nm[i];
    p[i].y *= nm[i];
    p[i].z *= nm[i];
}

__kernel void kernel_distance(__global float4 *v1,
                              __global float4 *v2,
                              __global float *d)
{
    int i = get_global_id(0);

    float dx = v2[i].x - v1[i].x;
    float dy = v2[i].y - v1[i].y;
    float dz = v2[i].z - v1[i].z;

    // distance = sqrt(sum(dx^2 + dy^2 + dz^2))
    d[i] = sqrt(dx*dx + dy*dy + dz*dz);
}

__kernel void kernel_gravity(float Mg,
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

__kernel void kernel_kinetic(__global float *m,
							 __global float4 *v,
							 __global float4 *ke)
{
    int i = get_global_id(0);

    ke[i].x = 0.5 * m[i] * v[i].x * v[i].x;
    ke[i].y = 0.5 * m[i] * v[i].y * v[i].y;
    ke[i].z = 0.5 * m[i] * v[i].z * v[i].z;
}

__kernel void kernel_energy(__global float *m, __global float *E)
{
    int i = get_global_id(0);
    float c = 299792458; // speed of light in meters/second;
    E[i] = m[i] * c * c;
}

int gravity_averaging(__global float *m,
					  __global float4 *p,
 					  __global float4 *a,
 					  int num)
{
	int i = get_global_id(0);
	float G = 0.00000000006673;
	float4 sum;
	int j = 0;

	sum.x = 0;
	sum.y = 0;
	sum.z = 0;
	sum.w = 0;

	// calculate gravity between each object and this object.
	for (j = 0; j < num; j++)
	{
		// if it's this object, skip it
		if (i == j) continue;

		// generate the difference vectors
		float dx = p[i].x - p[j].x;
		float dy = p[i].y - p[j].y;
		float dz = p[i].z - p[j].z;

		// calculate the distance
		float d = sqrt(dx*dx + dy*dy + dz*dz);

		// this is magnitude of the vector
		float g = (G * m[i] * m[j])/(d*d);

		// combine the gravity vectors together
		sum.x += (dx/d)*g;
		sum.y += (dy/d)*g;
		sum.z += (dz/d)*g;
	}

	// assign the sum back
	a[i].x = sum.x;
	a[i].y = sum.y;
	a[i].z = sum.z;

	return 0;
}

__kernel void kernel_nbody(__global float *m,
						   __global float4 *a, // previously calculated gravirty vector per object
						   __global float4 *v,
						   __global float4 *p,
						   __global float *t,
						   int num)
{
    int i = get_global_id(0);

	gravity_averaging(m, p, a, num);

	// position is updated by velocity and some small quantity of acceration
    p[i].x += v[i].x*t[i] + 0.5*t[i]*t[i]*a[i].x;
    p[i].y += v[i].y*t[i] + 0.5*t[i]*t[i]*a[i].y;
    p[i].z += v[i].z*t[i] + 0.5*t[i]*t[i]*a[i].z;

    // velocity is updated by acceration and friction (which is zero in space)
    v[i].x += a[i].x*t[i];
    v[i].y += a[i].y*t[i];
    v[i].z += a[i].z*t[i];
}


