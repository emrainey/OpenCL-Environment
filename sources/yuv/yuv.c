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

void notify(cl_program program, void *arg)
{
    printf("Program %p Arg %p\n",program, arg);
}

void normalize_float(float *a, cl_uint low, cl_uint hi, cl_uchar *b, cl_uint numPixels)
{
    cl_uint i = 0;
    cl_uint range = hi - low;
    for (i = 0; i < numPixels; i++)
    {
        cl_uint c = (range * a[i]) + low;
        if (c > hi)
            b[i] = (cl_uchar)hi;
        else if (c < low)
            b[i] = (cl_uchar)low;
        else
            b[i] = (cl_uchar)c;
    }
}

cl_int convert_rgb2yuv_bt601(cl_environment_t *pEnv,
                             float *r,
                             float *b,
                             float *g,
                             float *y,
                             float *u,
                             float *v,
                             cl_uint numPixels)
{
    cl_mem r_mem;
    cl_mem g_mem;
    cl_mem b_mem;
    cl_mem y_mem;
    cl_mem u_mem;
    cl_mem v_mem;
    const cl_uint numBytes = numPixels * sizeof(float);
    cl_int err = CL_SUCCESS;
    size_t global_work_size = numPixels;
    cl_uint i = 0;

    cl_kernel kernel = clGetKernelByName(pEnv, "kernel_rgb2yuv_bt601");
    if (kernel == NULL)
        return CL_INVALID_KERNEL_NAME;

    // create the rgb and yuv memory
    r_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    g_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    b_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    y_mem = clCreateBuffer(pEnv->context, CL_MEM_WRITE_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    u_mem = clCreateBuffer(pEnv->context, CL_MEM_WRITE_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    v_mem = clCreateBuffer(pEnv->context, CL_MEM_WRITE_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);

    // enqueue the read memory
    for (i = 0; i < pEnv->numDevices; i++) {
        err = 0;
        err |= clEnqueueWriteBuffer(pEnv->queues[i], r_mem, CL_TRUE, 0, numBytes, r, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], g_mem, CL_TRUE, 0, numBytes, g, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], b_mem, CL_TRUE, 0, numBytes, b, 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("ERROR: Write Enqueue Error = %d\n",err);
    }

    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    // enqueue the kernel
    for (i = 0; i < pEnv->numDevices; i++) {
        clSetKernelArg(kernel, 0, sizeof(cl_mem), &r_mem);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), &g_mem);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), &b_mem);
        clSetKernelArg(kernel, 3, sizeof(cl_mem), &y_mem);
        clSetKernelArg(kernel, 4, sizeof(cl_mem), &u_mem);
        clSetKernelArg(kernel, 5, sizeof(cl_mem), &v_mem);

        err = 0;
        err |= clEnqueueNDRangeKernel(pEnv->queues[i], kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("ERROR: Work Queue Error = %d\n",err);
    }

    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    printf("Executed kernels!\n");

    // read the result memory
    for (i = 0; i<pEnv->numDevices; i++) {
        err = 0;
        err |= clEnqueueReadBuffer(pEnv->queues[i], y_mem, CL_TRUE, 0, numBytes, y, 0, NULL, NULL);
        err |= clEnqueueReadBuffer(pEnv->queues[i], u_mem, CL_TRUE, 0, numBytes, u, 0, NULL, NULL);
        err |= clEnqueueReadBuffer(pEnv->queues[i], v_mem, CL_TRUE, 0, numBytes, v, 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("ERROR: Read Enqueue Error=%d\n",err);
    }
    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);
#ifdef CL_DEBUG
    for (i = 0; i < numPixels; i++)
    {
        printf("rgb = {%lf, %lf, %lf} :: yuv = {%lf, %lf, %lf}\n",r[i],g[i],b[i],y[i],u[i],v[i]);
    }
#endif

    // release memory objects
    clReleaseMemObject(r_mem);
    clReleaseMemObject(g_mem);
    clReleaseMemObject(b_mem);
    clReleaseMemObject(y_mem);
    clReleaseMemObject(u_mem);
    clReleaseMemObject(v_mem);

    return err;
}

cl_int convert_rgb2yuv(cl_environment_t *pEnv,
                       float *r,
                       float *b,
                       float *g,
                       float *bt601,
                       float *y,
                       float *u,
                       float *v,
                       cl_uint numPixels)
{
    cl_mem r_mem;
    cl_mem g_mem;
    cl_mem b_mem;
    cl_mem y_mem;
    cl_mem u_mem;
    cl_mem v_mem;
    cl_mem bt601_mem;
    const cl_uint numBytes = numPixels * sizeof(float);
    cl_int err = CL_SUCCESS;
    size_t global_work_size = numPixels;
    cl_uint i = 0;


    cl_kernel kernel = clGetKernelByName(pEnv, "kernel_rgb2yuv");
    if (kernel == NULL)
        return CL_INVALID_KERNEL_NAME;

    // create the rgb and yuv memory
    r_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    g_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    b_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    y_mem = clCreateBuffer(pEnv->context, CL_MEM_WRITE_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    u_mem = clCreateBuffer(pEnv->context, CL_MEM_WRITE_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    v_mem = clCreateBuffer(pEnv->context, CL_MEM_WRITE_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    bt601_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, sizeof(float)*9, NULL, &err);
    cl_assert(err == CL_SUCCESS,);

    // enqueue the read memory
    for (i = 0; i < pEnv->numDevices; i++) {
        err = 0;
        err |= clEnqueueWriteBuffer(pEnv->queues[i], r_mem, CL_TRUE, 0, numBytes, r, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], g_mem, CL_TRUE, 0, numBytes, g, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], b_mem, CL_TRUE, 0, numBytes, b, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], bt601_mem, CL_TRUE, 0, sizeof(float)*9, bt601, 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("ERROR: Write Enqueue Error = %d\n",err);
    }

    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    // enqueue the kernel
    for (i = 0; i < pEnv->numDevices; i++) {
        clSetKernelArg(kernel, 0, sizeof(cl_mem), &r_mem);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), &g_mem);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), &b_mem);
        clSetKernelArg(kernel, 3, sizeof(cl_mem), &bt601_mem);
        clSetKernelArg(kernel, 4, sizeof(cl_mem), &y_mem);
        clSetKernelArg(kernel, 5, sizeof(cl_mem), &u_mem);
        clSetKernelArg(kernel, 6, sizeof(cl_mem), &v_mem);

        err = 0;
        err |= clEnqueueNDRangeKernel(pEnv->queues[i], kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("ERROR: Work Queue Error = %d\n",err);
    }

    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    printf("Executed kernels!\n");

    // read the result memory
    for (i = 0; i<pEnv->numDevices; i++) {
        err = 0;
        err |= clEnqueueReadBuffer(pEnv->queues[i], y_mem, CL_TRUE, 0, numBytes, y, 0, NULL, NULL);
        err |= clEnqueueReadBuffer(pEnv->queues[i], u_mem, CL_TRUE, 0, numBytes, u, 0, NULL, NULL);
        err |= clEnqueueReadBuffer(pEnv->queues[i], v_mem, CL_TRUE, 0, numBytes, v, 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("ERROR: Read Enqueue Error = %d\n",err);
    }
    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);
#ifdef CL_DEBUG
    for (i = 0; i < numPixels; i++)
    {
        printf("rgb = {%lf, %lf, %lf} :: yuv = {%lf, %lf, %lf}\n",r[i],g[i],b[i],y[i],u[i],v[i]);
    }
#endif

    // release memory objects
    clReleaseMemObject(r_mem);
    clReleaseMemObject(g_mem);
    clReleaseMemObject(b_mem);
    clReleaseMemObject(y_mem);
    clReleaseMemObject(u_mem);
    clReleaseMemObject(v_mem);
    clReleaseMemObject(bt601_mem);

    return err;
}

int main(int argc, char *argv[])
{
    const cl_uint width = 10;
    const cl_uint height = 1;
    const cl_uint numPixels = height * width;
    float r[numPixels];
    float g[numPixels];
    float b[numPixels];
    float y[numPixels]; cl_uchar Yp[numPixels];
    float u[numPixels]; cl_uchar Up[numPixels];
    float v[numPixels]; cl_uchar Vp[numPixels];
    float bt601[9] = {0.257, 0.504, 0.098, -0.148, -0.291, 0.439, 0.439, -0.368, -0.071};
    time_t start, diff;
    clock_t c_start, c_diff;
    cl_environment_t *pEnv = clCreateEnvironment(KDIR"kernel_yuv.cl",1,notify, NULL);
    if (pEnv)
    {
        cl_uint i = 0;
        printf("Created environment %p\n", pEnv);

        // initialize the data
        for (i = 0; i < numPixels; i++)
        {
            r[i] = (float)(rand()%1000)/999;
            g[i] = (float)(rand()%1000)/999;
            b[i] = (float)(rand()%1000)/999;
            y[i] = 0.00;
            u[i] = 0.00;
            v[i] = 0.00;
        }

        start = time(NULL);
        c_start = clock();
        convert_rgb2yuv_bt601(pEnv, r, g, b, y, u, v, numPixels);
        c_diff = clock() - c_start;
        diff = time(NULL) - start;
        printf("Constant Version Ran in %lu seconds (%lu ticks)\n", diff, c_diff);

        start = time(NULL);
        c_start = clock();
        convert_rgb2yuv(pEnv, r, g, b, bt601, y, u, v, numPixels);
        c_diff = clock() - c_start;
        diff = time(NULL) - start;
        printf("Non-Constant Version Ran in %lu seconds (%lu ticks)\n", diff, c_diff);

        normalize_float(y,   16, 235, Yp, numPixels);
        normalize_float(u, -128, 128, Up, numPixels);
        normalize_float(v, -128, 128, Vp, numPixels);

        for (i = 0; i < numPixels; i++)
            printf("YUV = {0x%02x, 0x%02x, 0x%02x}\n", Yp[i], Up[i], Vp[i]);

        clDeleteEnvironment(pEnv);
    }
    return 0;
}

