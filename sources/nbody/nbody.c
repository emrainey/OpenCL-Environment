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

#define CL_OPTIONS  ("-I/Users/emrainey/Source/OpenCL/include")

void notify(cl_program program, void *arg)
{
    printf("Program %p Arg %p\n",program, arg);
}

float sum(float *a, cl_uint num)
{
    float b = 0.0;
    cl_uint i = 0;
    for (i = 0; i < num; i++)
        b += a[i];
    return b;
}

cl_int distance(cl_environment_t *pEnv,
                float *x, float *y, float *z, float *d,
                cl_uint numPoints)
{
    cl_kernel_call_t call;
    cl_kernel_param_t params[] = {
        {sizeof(float)*numPoints,x,NULL,CL_MEM_READ_ONLY},
        {sizeof(float)*numPoints,y,NULL,CL_MEM_READ_ONLY},
        {sizeof(float)*numPoints,z,NULL,CL_MEM_READ_ONLY},
        {sizeof(float)*numPoints,d,NULL,CL_MEM_WRITE_ONLY},
    };

    call.kernel_name = "kernel_magnitude";
    call.params = params;
    call.numParams = dimof(params);
    call.numDim = 1;
    call.global_work_size[0] = numPoints;
    call.global_work_size[1] = 0;
    call.global_work_size[2] = 0;
    call.local_work_size[0] = 0;
    call.local_work_size[1] = 0;
    call.local_work_size[2] = 0;

    return clCallKernel(pEnv, &call);
}

cl_int nbodies(cl_environment_t *pEnv,
               float *m,
               float *a_x, float *a_y, float *a_z,
               float *v_x, float *v_y, float *v_z,
               float *p_x, float *p_y, float *p_z,
               float *t, float *d,   float *g,
               size_t numBodies)
{
    cl_mem  m_mem, d_mem, g_mem, t_mem;
    cl_mem ax_mem, ay_mem, az_mem;
    cl_mem vx_mem, vy_mem, vz_mem;
    cl_mem px_mem, py_mem, pz_mem;

    const cl_uint numBytes = numBodies * sizeof(float);
    cl_int err = CL_SUCCESS;
    size_t global_work_size = numBodies;
    cl_uint i = 0;

    cl_kernel kernel = clGetKernelByName(pEnv, "kernel_nbody");
    if (kernel == NULL)
        return CL_INVALID_KERNEL_NAME;

    // create the rgb and yuv memory
    m_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    t_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    d_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    g_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    ax_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    ay_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    az_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    vx_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    vy_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    vz_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    px_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    py_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);
    pz_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_WRITE, numBytes, NULL, &err);
    cl_assert(err == CL_SUCCESS,);

    // enqueue the write memory
    for (i = 0; i < pEnv->numDevices; i++) {
        err = 0;
        err |= clEnqueueWriteBuffer(pEnv->queues[i], m_mem, CL_TRUE, 0, numBytes, m, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], t_mem, CL_TRUE, 0, numBytes, t, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], d_mem, CL_TRUE, 0, numBytes, d, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], g_mem, CL_TRUE, 0, numBytes, g, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], ax_mem, CL_TRUE, 0, numBytes, a_x, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], ay_mem, CL_TRUE, 0, numBytes, a_y, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], az_mem, CL_TRUE, 0, numBytes, a_z, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], vx_mem, CL_TRUE, 0, numBytes, v_x, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], vy_mem, CL_TRUE, 0, numBytes, v_y, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], vz_mem, CL_TRUE, 0, numBytes, v_z, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], px_mem, CL_TRUE, 0, numBytes, p_x, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], py_mem, CL_TRUE, 0, numBytes, p_y, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(pEnv->queues[i], pz_mem, CL_TRUE, 0, numBytes, p_z, 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("ERROR: Write Enqueue Error = %d\n",err);
    }

    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    // enqueue the kernel
    for (i = 0; i < pEnv->numDevices; i++) {
        clSetKernelArg(kernel, 0, sizeof(cl_mem), &m_mem);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), &ax_mem);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), &ay_mem);
        clSetKernelArg(kernel, 3, sizeof(cl_mem), &az_mem);
        clSetKernelArg(kernel, 4, sizeof(cl_mem), &vx_mem);
        clSetKernelArg(kernel, 5, sizeof(cl_mem), &vy_mem);
        clSetKernelArg(kernel, 6, sizeof(cl_mem), &vz_mem);
        clSetKernelArg(kernel, 7, sizeof(cl_mem), &px_mem);
        clSetKernelArg(kernel, 8, sizeof(cl_mem), &py_mem);
        clSetKernelArg(kernel, 9, sizeof(cl_mem), &pz_mem);
        clSetKernelArg(kernel, 10, sizeof(cl_mem), &t_mem);
        clSetKernelArg(kernel, 11, sizeof(cl_mem), &d_mem);
        clSetKernelArg(kernel, 12, sizeof(cl_mem), &g_mem);
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
        err |= clEnqueueReadBuffer(pEnv->queues[i], d_mem, CL_TRUE, 0, numBytes, d, 0, NULL, NULL);
        err |= clEnqueueReadBuffer(pEnv->queues[i], g_mem, CL_TRUE, 0, numBytes, g, 0, NULL, NULL);
        if (err != CL_SUCCESS) printf("ERROR: Read Enqueue Error=%d\n",err);
    }
    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    // release memory objects
    clReleaseMemObject(m_mem);
    clReleaseMemObject(d_mem);
    clReleaseMemObject(g_mem);
    clReleaseMemObject(ax_mem); clReleaseMemObject(ay_mem); clReleaseMemObject(az_mem);
    clReleaseMemObject(vx_mem); clReleaseMemObject(vy_mem); clReleaseMemObject(vz_mem);
    clReleaseMemObject(px_mem); clReleaseMemObject(py_mem); clReleaseMemObject(pz_mem);
    return err;
}

int ipow(int v, int p)
{
    if (p == 0)
        return 1;
    if (p == 1)
        return v;
    return v*ipow(v,p-1);
}

int rrand(int low, int hi)
{
    int r = hi - low;
    return (rand() % r) + low;
}

float frand(void)
{
    return (float)(rand()%1000)/999;
}

#define tmalloc(num,type)                    (type *)malloc(num * sizeof(type))

int main(int argc, char *argv[])
{
    const size_t numBodies = 10;
    float *m   = tmalloc(numBodies, float);
    float *d   = tmalloc(numBodies, float);
    float *g   = tmalloc(numBodies, float);
    float *t   = tmalloc(numBodies, float);
    float *a_x = tmalloc(numBodies, float);
    float *a_y = tmalloc(numBodies, float);
    float *a_z = tmalloc(numBodies, float);
    float *v_x = tmalloc(numBodies, float);
    float *v_y = tmalloc(numBodies, float);
    float *v_z = tmalloc(numBodies, float);
    float *p_x = tmalloc(numBodies, float);
    float *p_y = tmalloc(numBodies, float);
    float *p_z = tmalloc(numBodies, float);

    time_t start, diff;
    clock_t c_start, c_diff;

    cl_environment_t *pEnv = clCreateEnvironment(KDIR"kernel_nbody.cl",1,notify, CL_OPTIONS);
    if (pEnv)
    {
        cl_uint i = 0;
        printf("Created environment %p\n", pEnv);

        for (i = 0; i < numBodies; i++)
        {
            m[i]   = frand() * ipow(10,rrand(4,27)); // masses should be 10^4 - 10^27 (Earth heavy)
            a_x[i] = frand() * ipow(10,rrand(1,8));
            a_y[i] = frand() * ipow(10,rrand(1,8));
            a_z[i] = frand() * ipow(10,rrand(1,8));
            v_x[i] = frand() * ipow(10,rrand(1,9));  // velocity must be lower than 0.8*c!
            v_y[i] = frand() * ipow(10,rrand(1,9));  // velocity must be lower than 0.8*c!
            v_z[i] = frand() * ipow(10,rrand(1,9));  // velocity must be lower than 0.8*c!
            p_x[i] = frand() * ipow(10,rrand(4,8));
            p_y[i] = frand() * ipow(10,rrand(4,8));
            p_z[i] = frand() * ipow(10,rrand(4,8));
            t[i] = 0.001; // 1 millisecond.
            d[i] = 0.00; // this will be initialized in the kernel
            g[i] = 0.00; // this will be initialized in the kernel
        }

        start = time(NULL);
        c_start = clock();
        nbodies(pEnv, m, a_x,a_y, a_z,v_x, v_y, v_z, p_x, p_y, p_z, t, d, g, numBodies);
        distance(pEnv, p_x, p_y, p_z, d, numBodies);
        //c_diff = clock() - c_start;
        diff = time(NULL) - start;
        printf("Constant Version Ran in %lu seconds (%lu ticks)\n", diff, c_diff);

//#ifdef CL_DEBUG
        for (i = 0; i < numBodies; i++)
        {
            printf("[%6u] p={%.2lf,%.2lf,%.2lf} v={%.2lf,%.2lf,%.2lf} a={%.2lf,%.2lf,%.2lf} d=%lf g=%lf\n", i,
                    p_x[i], p_y[i], p_z[i],
                    v_x[i], v_y[i], v_z[i],
                    a_x[i], a_y[i], a_z[i], d[i],g[i]);
        }
//#endif
        clDeleteEnvironment(pEnv);
    }
    return 0;
}

