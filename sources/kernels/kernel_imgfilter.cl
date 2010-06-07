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

#include <climgfilter.h>

__kernel void kernel_convolution(__global cl_Image_t *pIn,
                                 __global cl_Image_t *pOut,
                                 __global cl_Kernel_t *pK)
{
    // these are the coordinates in the image
    const int x = get_global_id(0);
    const int y = get_global_id(1);
/**
    int i = (y*pIn->strides[Y_DIM]) + (x*pIn->strides[X_DIM]);
    int j = (y*pOut->strides[Y_DIM]) + (x*pOut->strides[X_DIM]);
    int s = 0;
    int p,q,r,k = 0; // p=n, q=y, r=x
    int

    for (p = 0; p < pK->n_dim; p++)
    {
        for (q = 0; q < pK->y_dim; q++)
        {
            for (r = 0; r < pK->x_dim; r++)
            {
                unsigned char a = pK->data[r + (q * pK->y_dim) + (p * (pK->n_dim)]
                s +=
            }
        }
    }
    pOut->data[0][j] =
*/


}
