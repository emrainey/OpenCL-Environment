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

__kernel void kernel_budge(float4 point1, 
						   __global float4 *point2,
						   __global float *pull, 
						   __global float4 *budge)
{
	int i = get_global_id(0); // data fields are 1 dimensional
	float4 direction;
	direction.x = point2[i].x - point1.x;
	direction.y = point2[i].y - point1.y;
	direction.z = point2[i].z - point1.z;
	direction = normalize(direction);
	budge[i].x = direction.x*pull[i];
	budge[i].y = direction.y*pull[i];
	budge[i].z = direction.z*pull[i];
}						

__kernel void kernel_gravity(float mass1,
                             __global float *mass2,
                             __global float *distance,
                             __global float *pull)
{
    int i = get_global_id(0); // data fields are 1 dimensional

    // G = gravitational constant = 6.67300 × 10^-11 m^3 kg^-1 s^-2
    float G = 0.00000000006673;

    // force of gravity = (M1*M2*G)/d^2
    pull[i] = (mass1 * mass2[i] * G)/(distance[i] * distance[i]);
}

__kernel void kernel_distance(__global float *distance, 
						      float4 point1, 
						      __global float4 *point2)
{
	int i = get_global_id(0); 
	
	float dx = point2[i].x - point1.x;
	float dy = point2[i].y - point1.y;
	float dz = point2[i].z - point1.z;
	
	dx*=dx;
	dy*=dy;
	dz*=dz;
	
	distance[i] = sqrt(dx+dy+dz);
}

/*
__kernel void kernel_spacetime(__global cl_spacetime_grid_t *grid, __global cl_body_t *body)
{
	__local float distance[GRID_SIZE];
	
	kernel_distance(distance, &body->m_position, grid->m_position);
	kernel_gravity(body->m_mass, grid->m_mass, distance, grid->m_pull);
}
*/