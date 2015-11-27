/**
 * Copyright (C) 2015 Erik Rainey
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

// pointless prototypes
precision gravity(precision M1, precision M2, precision d);

precision gravity(precision M1, precision M2, precision d) {
    // G = gravitational constant = 6.67300 × 10^-11 m^3 kg^-1 s^-2
    const precision G = 6.673E-11;
    return (G * M1 * M2) / (d * d);
}

__kernel void kernel_distort(precision4 point1,
                             precision mass1,
                             __global precision4 *point2,
                             __global precision  *mass2,
                             __global precision4 *budge) {
    int i = get_global_id(0);
    precision d = distance(point1, point2[i]);
    precision g = gravity(mass1, mass2[i], d);
    precision4 delta = point1 - point2[i];
    precision4 u = normalize(delta);
    budge[i] = u * fmin(d,g);
}
