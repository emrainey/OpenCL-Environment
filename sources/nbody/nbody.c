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
#ifndef CL_BUILD_RUNTIME
#include <kernel_nbody.h>
#endif
#include <clmath.h>
#include <clenvironment.h>

void notify(cl_program program, void *arg)
{
    //printf("Program %p Arg %p\n",program, arg);
}

cl_int distance(cl_environment_t *pEnv,
                cl_float4 *p, 
				cl_float *d,
                cl_uint numPoints)
{
	cl_uint n  = sizeof(float) * numPoints;
	cl_uint n4 = sizeof(cl_float4) * numPoints;
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_1D, n4,p,NULL,CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, n ,d,NULL,CL_MEM_WRITE_ONLY},
    };
    cl_kernel_call_t call = {
		"kernel_magnitude",
		params, dimof(params),
		1, 
		{0,0,0},
		{numPoints, 0, 0},
		{1,1,1},
		CL_SUCCESS, 0,0,0
	};
    return clCallKernel(pEnv, &call,1);
}

cl_int nbodies(cl_environment_t *pEnv,
			   cl_float *m,
               cl_float4 *a,
               cl_float4 *v,
               cl_float4 *p,
               cl_float *t, 
		       cl_uint numBodies)
{
	cl_uint n = sizeof(cl_float)*numBodies;
	cl_uint n4 = sizeof(cl_float4)*numBodies;
	cl_kernel_param_t params[] = {
		{CL_KPARAM_BUFFER_1D, n,  m, NULL, CL_MEM_READ_ONLY},
		{CL_KPARAM_BUFFER_1D, n4, a, NULL, CL_MEM_READ_WRITE},
		{CL_KPARAM_BUFFER_1D, n4, v, NULL, CL_MEM_READ_WRITE},
		{CL_KPARAM_BUFFER_1D, n4, p, NULL, CL_MEM_READ_WRITE},
		{CL_KPARAM_BUFFER_1D, n,  t, NULL, CL_MEM_READ_ONLY},
		{CL_KPARAM_BUFFER_0D, sizeof(size_t), &numBodies, NULL, CL_MEM_READ_ONLY},		
	};
	cl_kernel_call_t call = {
		"kernel_nbody",
		params, dimof(params),
		1, 
		{0,0,0},
		{numBodies, 0, 0},
		{1,1,1},
		CL_SUCCESS,0,0,0
	};
	return clCallKernel(pEnv, &call,1);
}

int main(int argc, char *argv[])
{
    const cl_uint numBodies = 10;
    float *m     = cl_malloc_array(float, numBodies);
	float *t     = cl_malloc_array(float, numBodies);
    cl_float4 *a = cl_malloc_array(cl_float4, numBodies);
    cl_float4 *v = cl_malloc_array(cl_float4, numBodies);
    cl_float4 *p = cl_malloc_array(cl_float4, numBodies);

	cl_uint type = clGetTypeFromString(CL_USER_DEVICE_TYPE);
	cl_uint count = CL_USER_DEVICE_COUNT; 
	
#ifdef CL_BUILD_RUNTIME
    cl_environment_t *pEnv = clCreateEnvironment(KDIR"kernel_nbody.cl",type,count,notify, CL_ARGS);
#else	
	cl_environment_t *pEnv = clCreateEnvironmentFromBins(&gKernelBins, notify, CL_ARGS);
#endif	
	if (pEnv)
    {
        cl_uint i = 0, j = 0;
		cl_uint numIterations = (argc > 1?atoi(argv[1]):10);
        for (i = 0; i < numBodies; i++)
        {
            m[i] = frand() * ipow(10,rrand(4,27)); // masses should be 10^4 - 10^27 ("Earth heavy")
            frand4(a[i], 1, 3);
			frand4(v[i], 1, 2);
			frand4(p[i], 4, 8);
            t[i] = 0.1f; // 100 millisecond.
        }
		i = 0;
		for (j = 0; j < numIterations; j++)
		{
        	nbodies(pEnv, m, a, v, p, t, numBodies);
            printf("[%6u] p={%lf,%lf,%lf} v={%lf,%lf,%lf} a={%lf,%lf,%lf}\n", i,
                    p[i][0], p[i][1], p[i][2],
                    v[i][0], v[i][1], v[i][2],
                    a[i][0], a[i][1], a[i][2]);
		}
        clDeleteEnvironment(pEnv);
		cl_free(t);
		cl_free(m);
		cl_free(v); 
		cl_free(a);
		cl_free(p);
    }
    return 0;
}

