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

__kernel void kernel_budge(precision4 point1,
                           __global precision4 *point2,
                           __global precision  *pull,
                           __global precision4 *budge)
{
    int i = get_global_id(0); // data fields are 1 dimensional
    precision4 direction;
    direction.x = point1.x - point2[i].x;
    direction.y = point1.y - point2[i].y;
    direction.z = point1.z - point2[i].z;
    direction = normalize(direction);
    budge[i].x = direction.x*pull[i];
    budge[i].y = direction.y*pull[i];
    budge[i].z = direction.z*pull[i];
}

__kernel void kernel_gravity(precision mass1,
                             __global precision *mass2,
                             __global precision *distance,
                             __global precision *pull)
{
    int i = get_global_id(0); // data fields are 1 dimensional

    // G = gravitational constant = 6.67300 × 10^-11 m^3 kg^-1 s^-2
    precision G = 0.00000000006673;

    // force of gravity = (M1*M2*G)/d^2
    pull[i] = (mass1 * mass2[i] * G)/(distance[i] * distance[i]);
}

__kernel void kernel_distance(__global precision *d,
                              precision4 point1,
                              __global precision4 *point2)
{
    int i = get_global_id(0);

    precision dx = point2[i].x - point1.x;
    precision dy = point2[i].y - point1.y;
    precision dz = point2[i].z - point1.z;

    dx*=dx;
    dy*=dy;
    dz*=dz;

    d[i] = sqrt(dx+dy+dz);
}

