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


unsigned char clamp_uc(int v, int l, int h)
{
	if (v > h)
		v = h;
	if (v < l)
		v = l;
	return (unsigned char)v;
}

char clamp_c(int v, int l, int h)
{
	if (v > h)
		v = h;
	if (v < l)
		v = l;
	return (char)v;
}

__kernel void kernel_uyvy_to_2bgr(__global unsigned char *uyvy,
								__global unsigned char *bgr,
								unsigned int width,
								unsigned int height,
								unsigned int srcStride,
								unsigned int dstStride)
{
	int x = get_global_id(0) * 2; // extend the x-width since it is macropixel indexed
	int y = get_global_id(1);
	int y0,u0,y1,v0,r,g,b;
	
	// determine UYVY index 
	int i = (y * srcStride) + (x * 2); // xstride == 2
	
	// determine BGR index
	int j = (y * dstStride) + (x * 3); // xstride == 3
	
	u0 = uyvy[i+0] - 128;
	y0 = uyvy[i+1] - 16;
	v0 = uyvy[i+2] - 128;
	y1 = uyvy[i+3] - 16;
	
	r = ((74 * y0) + (102 * v0)) >> 6; 
	g = ((74 * y0) - (52 * v0) - (25 * u0)) >> 6;
	b = ((74 * y0) + (129 * u0)) >> 6;
	
	bgr[j + 0] = clamp_uc(b, 0, 255);
	bgr[j + 1] = clamp_uc(g, 0, 255);
	bgr[j + 2] = clamp_uc(r, 0, 255);
	
	r = ((74 * y1) + (102 * v0)) >> 6; 
	g = ((74 * y1) - (52 * v0) - (25 * u0)) >> 6;
	b = ((74 * y1) + (129 * u0)) >> 6;
	
	bgr[j + 3] = clamp_uc(b, 0, 255);
	bgr[j + 4] = clamp_uc(g, 0, 255);
	bgr[j + 5] = clamp_uc(r, 0, 255);
}

__kernel void kernel_yuv420_to_luma(__global uchar *pYUV420,
									__global uchar *pLuma,
									uint width, 
									uint height,
									int srcStride,
									int dstStride)
{
	int x = get_global_id(0);
	int y = get_global_id(1);
	
	int i = (y * srcStride) + x;
	int j = (y * dstStride) + x;
	
	pLuma[j] = pYUV420[i];
}

__kernel void kernel_rgb2yuv(__global float *r,
							__global float *g,
							__global float *b,
							__constant float *a, // you could use BT601 or BT709 constants
							__global float *y,
							__global float *u,
							__global float *v)
{
	int i = get_global_id(0);
	y[i] =  a[0]*r[i] + a[1]*g[i] + a[2]*b[i];
	u[i] =  a[3]*r[i] + a[4]*g[i] + a[5]*b[i];
	v[i] =  a[6]*r[i] + a[6]*g[i] + a[7]*b[i];
}

__kernel void kernel_rgb2yuv_bt601(__global float *r,
								__global float *g,
								__global float *b,
								__global float *y,
								__global float *u,
								__global float *v)
{
	int i = get_global_id(0);
	y[i] =  0.257*r[i] + 0.504*g[i] + 0.098*b[i];
	u[i] = -0.148*r[i] - 0.291*g[i] + 0.439*b[i];
	v[i] =  0.439*r[i] - 0.368*g[i] - 0.071*b[i];
}

__kernel void kernel_yuv2rgb(__global unsigned char *y,
							__global unsigned char *u,
							__global unsigned char *v,
							__constant unsigned char *a,
							__global unsigned char *r,
							__global unsigned char *g,
							__global unsigned char *b)
{
	int i = get_global_id(0);
	r[i] = a[0]*(y[i] - a[5]) + a[1]*(u[i] - a[6]);
	g[i] = a[0]*(y[i] - a[5]) + a[2]*(u[i] - a[6]) + a[3]*(v[i] - a[6]);
	b[i] = a[0]*(y[i] - a[5]) + a[4]*(v[i] - a[6]);
}


