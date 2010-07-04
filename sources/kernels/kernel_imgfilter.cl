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

uchar clamp_uc(float a, uchar l, uchar h)
{
	if ((uint)a > (uint)h)
		return h;
	if ((int)a < (int)l)
		return l;
	return (uchar)a;
}

__kernel void kernel_edge_filter(uint  width, 
	 							 uint  height,
								 __global uchar *pSrc,  // planar 1 byte pixel data
	 							 int srcStride, 
							     __global uchar *pDst, // planar 1 byte pixel data
								 int dstStride,
								 __global char *operator, // a fixed sized array of 2xNxN elements
								 uint n,
								 uint range,
								 uint limit) 
{
    // these are the coordinates in the image
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    int i,j; // linear indexes for pSrc and pDst
	int a, b; // relative x,y pixel index
	int p,q,r; // operator pixel indexes
	int s; // linear index for operator
	int nh = n >> 1; // rounded half-dimension size i.e. n = 3, nh = 1
	int grad = 0;
	int sum = 0;
	float g = 0;
	for (p = 0; p < 2; p++)
    {
		grad = 0;
        for (q = 0, b = y-nh; q < n, b <= y+nh; q++, b++)
        {
			if (b < 0) continue;
			if (b >= height) continue;
			
            for (r = 0, a = x-nh; r < n, a <= x+nh; r++, a++)
            {
				if (a < 0) continue;
				if (a >= width) continue;
				
				i = (b * srcStride) + a;
				//if (pSrc[i] == 0) continue;
				
				s = (p * n * n) + (q * n) + r; 
				grad += (int)((int)operator[s] * (uint)pSrc[i]);
            }
        }
		sum += (grad * grad);
    }
	j = (y * dstStride) + x;
	g = sqrt((float)sum);
	pDst[j] = ((g/range) * limit);
}	
