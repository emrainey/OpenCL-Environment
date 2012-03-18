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

#include <clspacetime.h>

__kernel void kernel_budge(double4 point1, 
						   __global double4 *point2,
						   __global double  *pull, 
						   __global double4 *budge)
{
	int i = get_global_id(0); // data fields are 1 dimensional
	double4 direction;
	direction.x = point1.x - point2[i].x;
	direction.y = point1.y - point2[i].y;
	direction.z = point1.z - point2[i].z;
	direction = normalize(direction);
	budge[i].x = direction.x*pull[i];
	budge[i].y = direction.y*pull[i];
	budge[i].z = direction.z*pull[i];
}						

__kernel void kernel_gravity(double mass1,
                             __global double *mass2,
                             __global double *distance,
                             __global double *pull)
{
    int i = get_global_id(0); // data fields are 1 dimensional

    // G = gravitational constant = 6.67300 × 10^-11 m^3 kg^-1 s^-2
    double G = 0.00000000006673;

    // force of gravity = (M1*M2*G)/d^2
    pull[i] = (mass1 * mass2[i] * G)/(distance[i] * distance[i]);
}

__kernel void kernel_distance(__global double *distance, 
						      double4 point1, 
						      __global double4 *point2)
{
	int i = get_global_id(0); 
	
	double dx = point2[i].x - point1.x;
	double dy = point2[i].y - point1.y;
	double dz = point2[i].z - point1.z;
	
	dx*=dx;
	dy*=dy;
	dz*=dz;
	
	distance[i] = sqrt(dx+dy+dz);
}

/*
__kernel void kernel_spacetime(__global cl_spacetime_grid_t *grid, __global cl_body_t *body)
{
	__local double distance[GRID_SIZE];
	
	kernel_distance(distance, &body->m_position, grid->m_position);
	kernel_gravity(body->m_mass, grid->m_mass, distance, grid->m_pull);
}
*/