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

__kernel void kernel_edge_filter(__global uint  width, 
	 							 __global uint  height,
								 __global uchar *pSrc,  // planar 1 byte pixel data
	 							 __global uint srcStride, 
							     __global uchar *pDst, // planar 1 byte pixel data
								 __global uint dstStride,
								 __global uchar *operator, // a fixed sized array of 2xNxN elements
								 __global uint n) 
{
    // these are the coordinates in the image
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    int i,j; // linear indexes for pSrc and pDst
	int a, b; // co-pixel index
	float sum = 0;
	int p,q,r; // operator pixel indexes
	int nh = n / 2;
	
    for (p = 0; p < 2; p++)
    {
		int grad = 0;
        for (q = 0, b = y-nh; q < n, b < y+nh; q++, b++)
        {
            for (r = 0, a = x-nh; r < n, a < x+nh; r++, a++)
            {
				int s = (p*n*n) + (q*n) + r; // linear index for operator
				i = (b*srcStride) + a;
				grad += operator[s] * pSrc[i];
            }
        }
		sum += grad*grad;
    }
	j = (y*dstStride) + x;
	pDst[j] = (uchar)sqrt(sum); 
}
