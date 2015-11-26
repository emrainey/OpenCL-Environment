/**
 * Copyright (C) 2010 Erik Rainey
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*! \file
 * \brief The "OpenCL Environment" core features header.
 * \see http://github.com/emrainey/OpenCL-Environment
 * \author Erik Rainey <erik.rainey@gmail.com>
 */

#ifndef CL_ENVIRONMENT_H
#define CL_ENVIRONMENT_H

#if defined(DARWIN) || defined(__APPLE__)
#include <sys/types.h>
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define CL_MAX_DEVICES                 (30)     // @TODO determine a better way to limit the max number of devices.
#define CL_MAX_PATHSIZE                (256)    /**< Similar to Window's MAX_PATH */
#define CL_MAX_STRING                  (1024)   /**< Maximum length of a string from the command line or elsewhere */
#define CL_MAX_LINESIZE                (1024)   /**< Maximum length of a source file line */

/** Handy macro to allow you to allocate a structure without all that pesky typecasting! */
#define cl_malloc_struct(type)  (type *)cl_malloc(sizeof(type))

/** Handy macro to allow you to allocate an array of a type without peksy typecasting */
#define cl_malloc_array(type, num) (type *)cl_malloc(sizeof(type) * num)

/** Macro to create strings from types/enums */
#ifndef STRINGERIZE
#define STRINGERIZE(x)          (#x)
#endif

/** This pulls the define list and includes from the makefile into the C/C++ files for compilation with the OpenCL kernels at run/compile time */
#if defined(DEFINES) && defined(INCLUDES)
#define CL_ARGS                 (STRINGERIZE(DEFINES)" "STRINGERIZE(INCLUDES))
#else
#define CL_ARGS                 NULL
#endif

#define MAX_PARAMS				(10)

#ifndef dimof
/** Dimension of */
#define dimof(x)                (sizeof(x)/sizeof(x[0]))
#endif

#ifndef KDIR
/** Use this incase you don't define the path to your kernels */
#ifdef Windows_NT
#define KDIR ".\\"
#else
#define KDIR "./"
#endif
#endif

#ifndef CL_USER_DEVICE_TYPE
#define CL_USER_DEVICE_TYPE "all"
#endif

#ifndef CL_USER_DEVICE_COUNT
#define CL_USER_DEVICE_COUNT (1)
#endif

#if defined(ARCH_64)
#pragma OPENCL EXTENSION cl_khr_fp64: enable
#endif

#if defined(WIN32)
#define FMT_SIZE_T      "%llu"
#else
#define FMT_SIZE_T      "%zu"
#endif

/** A byte type */
typedef unsigned char cl_byte;

/** This is the data structure which holds precompiled OpenCL kernels for a series of devices */
typedef struct _cl_kernel_bin_t {
    cl_device_type deviceTypes; /**< the device types used in generating this set of kernels */
    size_t    numDevices;       /**< the number of devices these kernels have been compiled against */
    size_t    numBytesSizes;    /**< The number of bytes in the sizes array */
    size_t    numBytesData;     /**< The number of bytes in the data array */
    size_t   *sizes;            /**< An array of size_t values which indicates the length in bytes of the same indexed data buffer */
    cl_byte **data;             /**< An array to precompiled device code, each length is indicated in the same index of sizes. */
} cl_kernel_bin_t;

/**< This indicates the type of parameter */
typedef enum _cl_kernel_param_e {
    CL_KPARAM_BUFFER_0D = 0,    /**< This is simply a single value to pass to the OpenCL kernel */
    CL_KPARAM_BUFFER_1D,        /**< This is an array of values to pass to the OpenCL kernel */
    CL_KPARAM_BUFFER_2D,        /**< This is a two dimensional array of values to pass to the OpenCL Kernel */
    CL_KPARAM_BUFFER_3D,        /**< This is a three dimensional array of values to pass to the OpenCL kernel */
    CL_KPARAM_LOCAL,            /**< This indicates that the memory indicated by this parameter should be local to the device */
} cl_kernel_param_e ;

/**< The enumeration of the dimensions for multi-dimensional buffers. */
typedef enum _cl_dimensions_e {
    X_DIM = 0,
    Y_DIM,
    Z_DIM,
    W_DIM,
    DIM_MAX,
} clDimensions_e;

#define PLANE_MAX   (4)

typedef struct _cl_nd_buffer_t {
    cl_image_format format;         /**< contains the needed formatting information about the type of nD buffer this is */
    void   *data[PLANE_MAX];        /**< planar data. for single plane data, just use [0] */
    cl_uint planes;                 /**< the number of planes */
    cl_uint dim[DIM_MAX];           /**< the size in pixels of each dimension */
    cl_int  strides[DIM_MAX];       /**< the stride of each dimension (x stride == channel size, y stride == pitch, z stride == frame separation ) */
    size_t  size;                   /**< the total size of the all the data involved in the buffer */
} cl_nd_buffer_t;

typedef struct _cl_kernel_param_t {
    cl_kernel_param_e type;         /**< @see cl_kernel_param_e */
    size_t numBytes;                /**< total number of bytes to allocate */
    void *data;                     /**< pointer to data type, for 0D, 1D, use normal pointers, for 2+ use cl_nd_buffer_t */
    cl_mem mem;                     /**< OpenCL memory handle. */
    cl_mem_flags flags;             /**< read/write attributes */
} cl_kernel_param_t;

typedef struct _cl_event_perf_t {
	cl_ulong queued;
	cl_ulong submitted;
	cl_ulong start;
	cl_ulong stop;

	cl_ulong overhead; 				/**< Enqueuing to submital to device */
	cl_ulong stall;					/**< Submital to Start */
	cl_ulong execute;				/**< Start to Stop */
} cl_event_perf_t;

typedef struct _cl_kernel_call_t {
    char *kernel_name;              /**< The name of OpenCL Kernel function you want to call. */
    cl_kernel_param_t *params;      /**< The pointer to the parameter list */
    cl_uint numParams;              /**< The number of parameters in the params variable */
    cl_uint numDim;                 /**< The number of dimensions in the problem (not necessarily the number of dimensions in the buffer) */
    size_t global_work_offset[3];   /**< The offset that each kernel call will use (not implemented currently) */
    size_t global_work_size[3];     /**< The size of each dimension */
    size_t  local_work_size[3];     /**< The local size of each sub-groups dimension limit */
    cl_int err;                     /**< The return code from the kernel call */
    cl_event event;                 /**< The handle to the event which will be used to time the call */
    cl_event_perf_t	perf;			/**< The performance structure of this call */
    cl_event dependencies[MAX_PARAMS];/**< The array of the input dependencies */
    cl_uint  dep_events;			/**< The count of the number of dependent events (previous functions) */
    cl_event inputs[MAX_PARAMS];	/**< The array of input events */
    cl_uint	 input_events;			/**< The count of the number of input events */
    cl_event outputs[MAX_PARAMS];	/**< The array of output events */
    cl_uint  output_events;			/**< The count of the number of output events */
} cl_kernel_call_t;

typedef struct _cl_environemnt_t
{
    cl_platform_id    platform;     /**< The handle to the OpenCL platform reference */
    cl_device_id     *devices;      /**< The array of devices */
    cl_uint           numDevices;   /**< The number of devices in the devices array */
    cl_context        context;      /**< The handle to the context for this grouping of devices */
    cl_context_properties context_props; /**< The properties field for the context */
    cl_command_queue *queues;       /**< The command and data queue handle for this environment */
    char            **sources;      /**< The loaded source code from the source files, if you loaded and build in this runtime. This is blank if you load from binaries */
    cl_uint           numLines;     /**< The number of lines of source code in sources */
    size_t           *lengths;      /**< The array of line lengths of sources */
    cl_program        program;      /**< The handle to the program */
    cl_kernel        *kernels;      /**< The array of kernel handles */
    cl_uint           numKernels;   /**< The number of kernel handles in kernels */
    cl_int           *retCodes;     /**< The array of return codes */
    cl_int			  supports_images; /**< Determines if the implementation supports images */
} cl_environment_t;

void clPrintEnvironment(cl_environment_t *pEnv);


/**< Build Callback Notifier Typedef */
typedef void (*clnotifier_f)(cl_program program, void *args);

/**
 * This function creates an OpenCL environment from a source file with a set of arguments for the run-time compiler.
 * @param filename The name of the file to read the sources from.
 * @param dev_type The device types to use to build your kernels.
 * @param numDevices The number of devices to build your environment against. (use 0 to indicate to the function to use the max number available)
 * @param notifier The function pointer to the notification function which will be called when the build is complete (though not necessarily successfully).
 * @param cl_args The string of arguments to give to the runtime compiler.
 */
cl_environment_t *clCreateEnvironment(char *filename,
                                      cl_uint dev_type,
                                      cl_uint numDevices,
                                      clnotifier_f notifier,
                                      char *cl_args);

/**
 * This function creates in OpenCL environment from a series of binaries which have been precompiled by the clCompiler.
 * @param bins The pointer to the data structure which houses the precompiled kernels.
 * @param notifier The function pointer to the notification function which will be called when the build is complete. Since these binaries were precompiled, no build errors can be reported, though other types of errors may still happen.
 * @param cl_args The string of arguments to give to the runtime compiler. This is still used in the precompiled case.
 */
cl_environment_t *clCreateEnvironmentFromBins(cl_kernel_bin_t *bins,
                                              clnotifier_f notifier,
                                              char *cl_args);

/** This function destroys an environment regardless of how it was created */
void clDeleteEnvironment(cl_environment_t *pEnv);

/** This function creates a blank kernel binary structure for use in saving the compiled kernels */
cl_kernel_bin_t *cl_create_kernel_bin(size_t numDevices);

/** This function deletes a kernel binary structure */
void cl_delete_kernel_bin(cl_kernel_bin_t * bins);

/** This function extracts the compiled kernels from the program */
cl_kernel_bin_t *cl_extract_kernels(cl_program program);

/** This function writes the compiled kernel binaries to a file in binary format.
 * @see cl_extract_kernels
 * @param filename The name of the file to create.
 * @param bins The pointer to the kernel binaries.
 */
void cl_dump_kernels(char *filename, cl_kernel_bin_t *bins);

/** This function writes the compiled kernel binaries to a text header file. Use this to have a statically defined set of kernels for your program.
 * @see clCreateEnvironmentFromBins
 * @see cl_extract_kernels
 * @param filename The name of the header to create.
 * @param bins The kernel binaries.
 * @return int
 * @retval 1 on Success.
 * @retval 0 on Failure.
 */
int cl_precompiled_header(char *filename, cl_kernel_bin_t *bins);

/**
 * This function takes the kernel binary structure and flattens it to a single 1D buffer.
 * @param bins The pointer to the kernel binaries.
 * @param pNumBytes The pointer to the location to store the size of the returned flat buffer.
 */
cl_byte *cl_serialize_kernels(cl_kernel_bin_t *bins, size_t *pNumBytes);

/**
 * This function takes a flattened buffer and creates a kernel binary structure
 * @param bin The flat buffer.
 * @param numBytes The number of bytes in the flat buffer.
 */
cl_kernel_bin_t *cl_unserialize_kernels(cl_byte * bin, size_t numBytes);

/**
 * This function gets the handle to a kernel function from the environment structure by querying for the name.
 * @param pEnv The pointer to the environment created by either clCreateEnvironment or clCreateEnvironmentFromBins
 * @param func_name The name of the kernel function.
 */
cl_kernel clGetKernelByName(cl_environment_t *pEnv, char *func_name);

/**
 * This function calls the kernel function as specified by the pCall parameter.
 * @note Currently one function call will be made
 * @TODO allow function chaining.
 * @param pEnv The pointer to the environment.
 * @param pCall The pointer to one or more kernel call data structures with all the fields filled in.
 * @param numCalls the number of kernels calls in the call chain.
 */
cl_int clCallKernel(cl_environment_t *pEnv, cl_kernel_call_t *pCall, cl_uint numCalls);

/** A handy wrapper for debugging */
void *cl_malloc(size_t numBytes);

/** A handy wrapper for debugging */
void cl_free(void *ptr);

/** Converts the string type to the the CL_DEVICE_TYPE_... */
cl_uint clGetTypeFromString(char *typestring);

/** Converts an error to a printout */
cl_int clPrintError(cl_int err, char *label, const char *function, const char *file, int line);

/** A macro to use to indicate the error */
#define CL_ERROR(err, label)    clPrintError(err, label, __FUNCTION__, __FILE__, __LINE__)

/** Assertion to allow the developer to print an error message then take an action. */
#define cl_assert(expr, action) { if (!(expr)) {printf("ERROR! "#expr" is false! %s:%u\n",__FILE__,__LINE__); action;} }

#endif
