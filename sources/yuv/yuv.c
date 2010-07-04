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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _MSC_VER
#include <CL/cl.h>
#else
#include <OpenCL/opencl.h>
#endif
#include <clenvironment.h>
#include <clmath.h>
#ifndef CL_BUILD_RUNTIME
#include <kernel_yuv.h>
#endif

void notify(cl_program program, void *arg)
{
    //printf("Compiled Program %p Arg %p\n",program, arg);
}

cl_int cl_convert_rgbf_to_yuvf_bt601(cl_environment_t *pEnv,
                                     float *r,
                                     float *g,
                                     float *b,
                                     float *y,
                                     float *u,
                                     float *v,
                                     cl_uint numPixels)
{
    cl_int err = CL_SUCCESS;
    const size_t numBytes = numPixels * sizeof(float);
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_1D, numBytes, r, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, g, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, b, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, y, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, u, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, v, NULL, CL_MEM_WRITE_ONLY},
    };
    cl_kernel_call_t call = {
        "kernel_rgb2yuv_bt601",
        params, dimof(params),
        1, 
		{0,0,0},
        {numPixels, 0,0},
        {1,1,1},
        CL_SUCCESS,0,0,0
    };
    err = clCallKernel(pEnv, &call,1);
    printf("%s took %llu ns\n", call.kernel_name, (call.stop - call.start));
    return err;
}

cl_int cl_convert_rgbf_to_yuvf(cl_environment_t *pEnv,
                               float *r,
                               float *g,
                               float *b,
                               float *y,
                               float *u,
                               float *v,
                               float *a, // constants
                               cl_uint numPixels)
{
    cl_int err = CL_SUCCESS;
    const size_t numBytes = numPixels * sizeof(float);
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_1D, numBytes, r, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, g, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, b, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, sizeof(float) * 9, a, NULL, CL_MEM_READ_ONLY}, // constants only have 9 elements
        {CL_KPARAM_BUFFER_1D, numBytes, y, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, u, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, v, NULL, CL_MEM_WRITE_ONLY},
    };
    cl_kernel_call_t call = {
        "kernel_rgb2yuv",
        params, dimof(params),
        1, 
		{0,0,0},
        {numPixels, 0, 0},
        {1,1,1},
        CL_SUCCESS,0,0,0
    };
    err = clCallKernel(pEnv, &call, 1);
    printf("%s took %llu ns\n", call.kernel_name, (call.stop - call.start));
    return err;
}

cl_int convert_uyvyto_2bgr(cl_environment_t *pEnv, cl_uchar *pUYVY, cl_uchar *pBGR, cl_uint width, cl_uint height, cl_int srcStride, cl_int dstStride)
{
	cl_int err = CL_SUCCESS;
	cl_kernel_param_t params[] = {
		{CL_KPARAM_BUFFER_1D, srcStride*height, pUYVY, NULL, CL_MEM_READ_ONLY},
		{CL_KPARAM_BUFFER_1D, dstStride*height, pBGR, NULL, CL_MEM_WRITE_ONLY},
		{CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &width, NULL, CL_MEM_READ_ONLY},
		{CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &height, NULL, CL_MEM_READ_ONLY},
		{CL_KPARAM_BUFFER_0D, sizeof(cl_int), &srcStride, NULL, CL_MEM_READ_ONLY},
		{CL_KPARAM_BUFFER_0D, sizeof(cl_int), &dstStride, NULL, CL_MEM_READ_ONLY},
	};
	cl_kernel_call_t call = {
		"kernel_uyvy_to_2bgr",
		params, dimof(params),
		2,
		{0,0,0},
		{width/2, height, 0}, // width/2 since it is macropixel packed! 
		{1,1,1},
		CL_SUCCESS, 0,0,0
	};
	err = clCallKernel(pEnv, &call, 1);
	printf("%s took %llu ns\n", call.kernel_name, (call.stop - call.start));
	return err;
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
 	    const cl_uint width = 1920;
	    const cl_uint height = 1080;
	    const cl_uint numPixels = height * width;
	    cl_int err = CL_SUCCESS;

	    printf("Processing %ux%u => %u pixels\n", width, height, numPixels);

	    float *r = cl_malloc_array(float, numPixels);
	    float *g = cl_malloc_array(float, numPixels);
	    float *b = cl_malloc_array(float, numPixels);
	    float *y = cl_malloc_array(float, numPixels);
	    float *u = cl_malloc_array(float, numPixels);
	    float *v = cl_malloc_array(float, numPixels);
	    cl_uchar *Yp = cl_malloc_array(cl_uchar, numPixels);
	    cl_uchar *Up = cl_malloc_array(cl_uchar, numPixels);
	    cl_uchar *Vp = cl_malloc_array(cl_uchar, numPixels);

	    float bt601[9] = {0.257f, 0.504f, 0.098f, -0.148f, -0.291f, 0.439f, 0.439f, -0.368f, -0.071f};
	    time_t start, diff;
	    clock_t c_start, c_diff1, c_diff2;

#ifdef CL_BUILD_RUNTIME
	    cl_environment_t *pEnv = clCreateEnvironment(KDIR"kernel_yuv.cl",CL_DEVICE_TYPE_GPU,2,notify, CL_ARGS);
#else
	    cl_environment_t *pEnv = clCreateEnvironmentFromBins(&gKernelBins, notify, CL_ARGS);
#endif
	    if (pEnv && r && g && b && y && u && v)
	    {
	        cl_uint i = 0;

	        srand((unsigned int)time(NULL));
	        // initialize the data
	        for (i = 0; i < numPixels; i++)
	        {
	            r[i] = frrand(0.0,1.0); // [0-1]
	            g[i] = frrand(0.0,1.0); // [0-1]
	            b[i] = frrand(0.0,1.0); // [0-1]
	            y[i] = 0.00;
	            u[i] = 0.00;
	            v[i] = 0.00;
	        }

	        start = time(NULL);
	        c_start = clock();
	        err = cl_convert_rgbf_to_yuvf_bt601(pEnv, r, g, b, y, u, v, numPixels);
	        cl_assert(err == CL_SUCCESS,printf("Error = %d\n",err));
	        c_diff1 = clock() - c_start;
	        diff = time(NULL) - start;
	        printf("With Constants Version Ran in %lu seconds (%lu ticks)\n", diff, c_diff1);

	        // initialize the data
	        for (i = 0; i < numPixels; i++)
	        {
	            r[i] = frrand(0.0,1.0); // [0-1]
	            g[i] = frrand(0.0,1.0); // [0-1]
	            b[i] = frrand(0.0,1.0); // [0-1]
	            y[i] = 0.00;
	            u[i] = 0.00;
	            v[i] = 0.00;
	        }

	        start = time(NULL);
	        c_start = clock();
	        cl_convert_rgbf_to_yuvf(pEnv, r, g, b, y, u, v, bt601, numPixels);
	        c_diff2 = clock() - c_start;
	        diff = time(NULL) - start;
	        printf("With No Constants Version Ran in %lu seconds (%lu ticks)\n", diff, c_diff2);

	        normalize_float(y,   16, 235, Yp, numPixels);
	        normalize_float(u, -128, 128, Up, numPixels);
	        normalize_float(v, -128, 128, Vp, numPixels);

	#ifdef CL_DEBUG
	        for (i = 0; i < numPixels; i++)
	            printf("YUV = {0x%02x, 0x%02x, 0x%02x}\n", Yp[i], Up[i], Vp[i]);
	#endif
	        clDeleteEnvironment(pEnv);
	    }

	    cl_free(r); cl_free(g); cl_free(b);
	    cl_free(y); cl_free(u); cl_free(v);
	}
	else if (argc == 6)
	{
		char filename[CL_MAX_PATHSIZE];
		char outfile[CL_MAX_PATHSIZE];
		int width = atoi(argv[2]);
		int height = atoi(argv[3]);
		int f, numFrames = atoi(argv[4]);
		unsigned int srcStride = width * 2;
		unsigned int dstStride = width * 3;
		int numYUVBytes = srcStride * height;
		int numRGBBytes = dstStride * height;
		strncpy(filename, argv[1], CL_MAX_PATHSIZE);
		strncpy(outfile, argv[5], CL_MAX_PATHSIZE);
		FILE *fi = fopen(filename, "rb");
		FILE *fo = fopen(outfile, "wb+");
		cl_uchar *pUYVY = cl_malloc_array(cl_uchar, numYUVBytes);
		cl_uchar *pBGR  = cl_malloc_array(cl_uchar, numRGBBytes);
		
#ifdef CL_BUILD_RUNTIME
	    cl_environment_t *pEnv = clCreateEnvironment(KDIR"kernel_yuv.cl",CL_DEVICE_TYPE_GPU, 2,notify, CL_ARGS);
#else
	    cl_environment_t *pEnv = clCreateEnvironmentFromBins(&gKernelBins, notify, CL_ARGS);
#endif
		if (pEnv && fi && fo && pUYVY && pBGR)
		{
			for (f = 0; f < numFrames; f++) 
			{
				fread(pUYVY, 1, numYUVBytes, fi);
				convert_uyvyto_2bgr(pEnv, pUYVY, pBGR, width, height, srcStride, dstStride);
				fwrite(pBGR, 1, numRGBBytes, fo);
			}
			clDeleteEnvironment(pEnv);
		}
		cl_free(pUYVY);
		cl_free(pBGR);
		
		fclose(fi); 
		fclose(fo);
	}
    return 0;
}

