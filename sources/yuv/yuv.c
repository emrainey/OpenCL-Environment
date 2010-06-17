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

#ifdef CL_PRECOMPILED
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
        {numBytes, r, NULL, CL_MEM_READ_ONLY},
        {numBytes, g, NULL, CL_MEM_READ_ONLY},
        {numBytes, b, NULL, CL_MEM_READ_ONLY},
        {numBytes, y, NULL, CL_MEM_WRITE_ONLY},
        {numBytes, u, NULL, CL_MEM_WRITE_ONLY},
        {numBytes, v, NULL, CL_MEM_WRITE_ONLY},
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
    err = clCallKernel(pEnv, &call);
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
        {numBytes, r, NULL, CL_MEM_READ_ONLY},
        {numBytes, g, NULL, CL_MEM_READ_ONLY},
        {numBytes, b, NULL, CL_MEM_READ_ONLY},
        {sizeof(float) * 9, a, NULL, CL_MEM_READ_ONLY}, // constants only have 9 elements
        {numBytes, y, NULL, CL_MEM_WRITE_ONLY},
        {numBytes, u, NULL, CL_MEM_WRITE_ONLY},
        {numBytes, v, NULL, CL_MEM_WRITE_ONLY},
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
    err = clCallKernel(pEnv, &call);
    printf("%s took %llu ns\n", call.kernel_name, (call.stop - call.start));
    return err;
}

int main(int argc, char *argv[])
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

    float bt601[9] = {0.257, 0.504, 0.098, -0.148, -0.291, 0.439, 0.439, -0.368, -0.071};
    time_t start, diff;
    clock_t c_start, c_diff1, c_diff2;

#ifdef CL_PRECOMPILED
    cl_environment_t *pEnv = clCreateEnvironmentFromBins(&gKernelBins, notify, NULL);
#else
    cl_environment_t *pEnv = clCreateEnvironment(KDIR"kernel_yuv.cl",1,notify, NULL);
#endif
    if (pEnv && r && g && b && y && u && v)
    {
        cl_uint i = 0;

        srand(time(NULL));
        // initialize the data
        for (i = 0; i < numPixels; i++)
        {
            r[i] = frand(0.0,1.0); // [0-1]
            g[i] = frand(0.0,1.0); // [0-1]
            b[i] = frand(0.0,1.0); // [0-1]
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
            r[i] = frand(0.0,1.0); // [0-1]
            g[i] = frand(0.0,1.0); // [0-1]
            b[i] = frand(0.0,1.0); // [0-1]
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
    return 0;
}

