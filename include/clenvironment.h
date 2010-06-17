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

#ifndef _CL_ENVIRONMENT_H_
#define _CL_ENVIRONMENT_H_

#define CL_MAX_DEVICES                 (30) // @TODO determine a better way to limit the max number of devices.
#define CL_MAX_PATHSIZE                (256)
#define CL_MAX_STRING                  (1024)
#define CL_MAX_LINESIZE                (1024)

#define cl_assert(expr, action) { if (!(expr)) {printf("ERROR! "#expr" is false! %s:%u\n",__FILE__,__LINE__); action;} }

#define cl_malloc_struct(type)  (type *)cl_malloc(sizeof(type))
#define cl_malloc_array(type, num) (type *)cl_malloc(sizeof(type) * num)

// Defines and includes should be pulled in from the compile line.
#define STRINGERIZE(x)          (#x)
#define CL_ARGS                 (STRINGERIZE(DEFINES)" "STRINGERIZE(INCLUDES))

#ifndef dimof
#define dimof(x)                (sizeof(x)/sizeof(x[0]))
#endif

#ifndef KDIR
#define KDIR // this is incase you don't define the path to your kernels
#endif

typedef unsigned char cl_byte;

/** This is the data structure which holds precompiled OpenCL kernels for a series of devices */
typedef struct _cl_kernel_bin_t {
    size_t    numDevices;		/**< the number of devices these kernels have been compiled against */
    size_t    numBytesSizes;	/**< The number of bytes in the sizes array */
    size_t    numBytesData;		/**< The number of bytes in the data array */
    size_t   *sizes;			/**< An array of size_t values which indicates the length in bytes of the same indexed data buffer */
    cl_byte **data;				/**< An array to precompiled device code, each length is indicated in the same index of sizes. */
} cl_kernel_bin_t;

/**< This indicates the type of parameter */
typedef enum _cl_kernel_param_e {
	CL_KPARAM_BUFFER_0D = 0,	/**< This is simply a single value to pass to the OpenCL kernel */
	CL_KPARAM_BUFFER_1D,		/**< This is an array of values to pass to the OpenCL kernel */
	CL_KPARAM_BUFFER_2D,		/**< This is a two dimensional array of values to pass to the OpenCL Kernel */
	CL_KPARAM_BUFFER_3D,		/**< This is a three dimensional array of values to pass to the OpenCL kernel */
} cl_kernel_param_e ;

typedef struct _cl_kernel_param_t {
	cl_kernel_param_e type;
    size_t numBytes;
    void *data;
    cl_mem mem;
    cl_mem_flags flags;
    cl_event event;
} cl_kernel_param_t;

typedef struct _cl_kernel_call_t {
    char *kernel_name;
    cl_kernel_param_t *params;
    cl_uint numParams;
    cl_uint numDim;
	size_t global_work_offset[3];
    size_t global_work_size[3];
    size_t  local_work_size[3];
    cl_int err;
    cl_event event;
    cl_ulong start;
    cl_ulong stop;
} cl_kernel_call_t;

typedef struct _cl_environemnt_t
{
    cl_platform_id    platform;
    cl_device_id     *devices;
    cl_uint           numDevices;
    cl_context        context;
    cl_context_properties context_props;
    cl_command_queue *queues;
    char            **sources;
    size_t            numLines;
    size_t           *lengths;
    cl_program        program;
    cl_kernel        *kernels;
    cl_uint           numKernels;
    cl_int           *retCodes;
} cl_environment_t;

// Build Callback Notifier
typedef void (*clnotifier_f)(cl_program program, void *args);

// User API
cl_environment_t *clCreateEnvironment(char *filename,
                                      cl_uint numDevices,
                                      clnotifier_f notifier,
                                      char *cl_args);

cl_environment_t *clCreateEnvironmentFromBins(cl_kernel_bin_t *bins,
                                              clnotifier_f notifier,
                                              char *cl_args);

void clDeleteEnvironment(cl_environment_t *pEnv);

cl_kernel_bin_t *cl_create_kernel_bin(size_t numDevices);
void cl_delete_kernel_bin(cl_kernel_bin_t * bins);

cl_kernel_bin_t *cl_extract_kernels(cl_program program);

void cl_dump_kernels(char *filename, cl_kernel_bin_t *bins);
void cl_precompiled_header(char *filename, cl_kernel_bin_t *bins);

cl_byte *cl_serialize_kernels(cl_kernel_bin_t *bins, size_t *pNumBytes);
cl_kernel_bin_t *cl_unserialize_kernels(cl_byte * bin, size_t numBytes);

cl_kernel clGetKernelByName(cl_environment_t *pEnv, char *func_name);
cl_int clCallKernel(cl_environment_t *pEnv, cl_kernel_call_t *pCall);

void *cl_malloc(size_t numBytes);
void cl_free(void *ptr);

#endif

