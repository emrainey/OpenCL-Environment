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
#ifdef _MSC_VER
#include <CL/cl.h>
#else
#include <OpenCL/opencl.h>
#endif
#include <clenvironment.h>

typedef enum _cl_type_e { CL_STRING_TYPE, CL_ULONG_TYPE, CL_UINT_TYPE, CL_BYTE_TYPE} cl_type_e;

#define CL_ITEM_MAX     (10)
#define L_STRINGERIZE(x)    #x, x

typedef struct _cl_error_strings_t {
    char *string;
    cl_uint err;
} cl_error_string_t;

cl_error_string_t errors[] = {
    {L_STRINGERIZE(CL_INVALID_VALUE)},
    {L_STRINGERIZE(CL_INVALID_DEVICE_TYPE)},
    {L_STRINGERIZE(CL_INVALID_PLATFORM)},
    {L_STRINGERIZE(CL_INVALID_DEVICE)},
    {L_STRINGERIZE(CL_INVALID_CONTEXT)},
    {L_STRINGERIZE(CL_INVALID_QUEUE_PROPERTIES)},
    {L_STRINGERIZE(CL_INVALID_COMMAND_QUEUE)},
    {L_STRINGERIZE(CL_INVALID_HOST_PTR)},
    {L_STRINGERIZE(CL_INVALID_MEM_OBJECT)},
    {L_STRINGERIZE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)},
    {L_STRINGERIZE(CL_INVALID_IMAGE_SIZE)},
    {L_STRINGERIZE(CL_INVALID_SAMPLER)},
    {L_STRINGERIZE(CL_INVALID_BINARY)},
    {L_STRINGERIZE(CL_INVALID_BUILD_OPTIONS)},
};
cl_uint numErrors = dimof(errors);

void clPrintError(cl_uint err)
{
    cl_uint i = 0;
    for (i = 0; i < numErrors; i++)
    {
        if (err == errors[i].err)
        {
            printf("ERROR: %s\n",errors[i].string);
            break;
        }
    }
}

typedef struct _cl_info_type_t {
    cl_type_e type;
      char  *name;
    cl_uint   info;
} cl_info_type_t;

cl_info_type_t clPlatInfoType[] = {
    {CL_STRING_TYPE, L_STRINGERIZE(CL_PLATFORM_PROFILE)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_PLATFORM_VERSION)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_PLATFORM_NAME)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_PLATFORM_VENDOR)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_PLATFORM_EXTENSIONS)},
};
cl_info_type_t clDeviceInfoType[] = {
    {CL_ULONG_TYPE, L_STRINGERIZE(CL_DEVICE_TYPE)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_VENDOR_ID)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_COMPUTE_UNITS)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_WORK_GROUP_SIZE)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_WORK_ITEM_SIZES)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_CLOCK_FREQUENCY)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_ADDRESS_BITS)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_READ_IMAGE_ARGS)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_WRITE_IMAGE_ARGS)},
    {CL_ULONG_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_MEM_ALLOC_SIZE)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_IMAGE2D_MAX_WIDTH)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_IMAGE2D_MAX_HEIGHT)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_IMAGE3D_MAX_WIDTH)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_IMAGE3D_MAX_HEIGHT)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_IMAGE3D_MAX_DEPTH)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_IMAGE_SUPPORT)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_PARAMETER_SIZE)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_SAMPLERS)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MEM_BASE_ADDR_ALIGN)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE)},
    {CL_ULONG_TYPE, L_STRINGERIZE(CL_DEVICE_SINGLE_FP_CONFIG)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE)},
    {CL_ULONG_TYPE, L_STRINGERIZE(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE)},
    {CL_ULONG_TYPE, L_STRINGERIZE(CL_DEVICE_GLOBAL_MEM_SIZE)},
    {CL_ULONG_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_MAX_CONSTANT_ARGS)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_LOCAL_MEM_TYPE)},
    {CL_ULONG_TYPE, L_STRINGERIZE(CL_DEVICE_LOCAL_MEM_SIZE)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_ERROR_CORRECTION_SUPPORT)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_PROFILING_TIMER_RESOLUTION)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_ENDIAN_LITTLE)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_AVAILABLE)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_COMPILER_AVAILABLE)},
    {CL_ULONG_TYPE, L_STRINGERIZE(CL_DEVICE_EXECUTION_CAPABILITIES)},
    {CL_ULONG_TYPE, L_STRINGERIZE(CL_DEVICE_QUEUE_PROPERTIES)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_DEVICE_NAME)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_DEVICE_VENDOR)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_DEVICE_VERSION)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_DEVICE_PROFILE)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_DEVICE_EXTENSIONS)},
    {CL_UINT_TYPE, L_STRINGERIZE(CL_DEVICE_PLATFORM)},
};
cl_info_type_t clContextInfoType[] = {
    {CL_STRING_TYPE, L_STRINGERIZE(CL_CONTEXT_REFERENCE_COUNT)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_CONTEXT_DEVICES)},
    {CL_STRING_TYPE, L_STRINGERIZE(CL_CONTEXT_PROPERTIES)},
};
cl_info_type_t clQueueInfoType[] = {
	{CL_UINT_TYPE,   L_STRINGERIZE(CL_QUEUE_CONTEXT)},
	{CL_UINT_TYPE,   L_STRINGERIZE(CL_QUEUE_DEVICE)},
	{CL_UINT_TYPE,   L_STRINGERIZE(CL_QUEUE_REFERENCE_COUNT)},
	{CL_UINT_TYPE,   L_STRINGERIZE(CL_QUEUE_PROPERTIES)},
};
cl_info_type_t clKernelInfoType[] = {
	{CL_STRING_TYPE, L_STRINGERIZE(CL_KERNEL_FUNCTION_NAME)},
	{CL_UINT_TYPE,   L_STRINGERIZE(CL_KERNEL_NUM_ARGS)},
	{CL_UINT_TYPE,   L_STRINGERIZE(CL_KERNEL_REFERENCE_COUNT)},
	{CL_UINT_TYPE,   L_STRINGERIZE(CL_KERNEL_CONTEXT)},
	{CL_UINT_TYPE,   L_STRINGERIZE(CL_KERNEL_PROGRAM)},
};
cl_uint numPlatInfoTypes = dimof(clPlatInfoType);
cl_uint numDeviceInfoTypes = dimof(clDeviceInfoType);
cl_uint numContextInfoTypes = dimof(clContextInfoType);
cl_uint numQueueInfoTypes = dimof(clQueueInfoType);
cl_uint numKernelInfoTypes = dimof(clKernelInfoType);

/** This function allows us to type-cast the various query functions */
typedef void (*QueryInfo_f)(void *, cl_uint, size_t, void *, size_t *);

void clPrintQueryInfo(void *id, cl_uint info, QueryInfo_f query, cl_info_type_t *infoTypes, cl_uint numInfoTypes)
{
    cl_uint i = 0;
    size_t size = 0;
    for (i = 0; i < numInfoTypes; i++)
    {
        if (info == infoTypes[i].info)
        {
            switch (infoTypes[i].type)
            {
                case CL_STRING_TYPE:
                {
                    cl_char param[CL_MAX_STRING];
                    memset(param, 0, sizeof(param));
                    query(id, info, sizeof(param), param, &size);
                    if (size > 0) printf("%41s:[%02lu] %s\n", infoTypes[i].name, size, param);
                    break;
                }
                case CL_ULONG_TYPE:
                {
                    cl_ulong param;
                    query(id, info, sizeof(param), &param, &size);
                    if (size == 8) printf("%41s:[%02lu] %llu\n", infoTypes[i].name, size, param);
                    break;
                }
                case CL_UINT_TYPE:
                {
                    cl_uint param;
                    query(id, info, sizeof(param), &param, &size);
                    if (size == 4) printf("%41s:[%02lu] 0x%08x\n", infoTypes[i].name, size, param);
                    break;
                }
                case CL_BYTE_TYPE:
                {
                    cl_char param;
                    query(id, info, sizeof(param), &param, &size);
                    if (size == 1) printf("%41s:[%02lu] 0x%02x\n", infoTypes[i].name, size, param);
                    break;
                }
            }
        }
    }
}

void clPrintPlatformInfo(cl_platform_id plat, cl_platform_info info)
{
	clPrintQueryInfo(plat, info, (QueryInfo_f)clGetPlatformInfo, clPlatInfoType, numPlatInfoTypes);
}

void clPrintDeviceInfo(cl_device_id device, cl_device_info info)
{
	clPrintQueryInfo(device, info, (QueryInfo_f)clGetDeviceInfo, clDeviceInfoType, numDeviceInfoTypes);
}

void clPrintContextInfo(cl_context context, cl_context_info info)
{
	clPrintQueryInfo(context, info, (QueryInfo_f)clGetContextInfo, clContextInfoType, numContextInfoTypes);
}

void clPrintQueueInfo(cl_command_queue queue, cl_command_queue_info info)
{
	clPrintQueryInfo(queue, info, (QueryInfo_f)clGetCommandQueueInfo, clQueueInfoType, numQueueInfoTypes);
}

void clPrintKernelInfo(cl_kernel kernel, cl_kernel_info info)
{
	clPrintQueryInfo(kernel, info, (QueryInfo_f)clGetKernelInfo, clKernelInfoType, numKernelInfoTypes);
}

void clQueryNotify(const char *errinfo,
                   const void *private_info,
                   size_t cb,
                   void * user_data)
{
    printf("ERROR! %s\n",errinfo);
}

void clPrintAllPlatformInfo(cl_platform_id platform)
{
	cl_uint i = 0;
    for (i = 0; i < numPlatInfoTypes; i++)
        clPrintPlatformInfo(platform, clPlatInfoType[i].info);
}

void clPrintAllDeviceInfo(cl_device_id device)
{	
	cl_uint i = 0;
    for (i = 0; i < numDeviceInfoTypes; i++)
        clPrintDeviceInfo(device,clDeviceInfoType[i].info);
}

void clPrintAllContextInfo(cl_context context)
{
	cl_uint i = 0;
    for (i = 0; i < numContextInfoTypes; i++)
        clPrintContextInfo(context, clContextInfoType[i].info);
}

void clPrintAllQueueInfo(cl_command_queue queue)
{
	cl_uint i = 0;
	for (i = 0; i < numQueueInfoTypes; i++)
		clPrintQueueInfo(queue, clQueueInfoType[i].info);
}

void clPrintAllKernelInfo(cl_kernel kernel)
{
	cl_uint i = 0;
	for (i = 0; i < numKernelInfoTypes; i++)
		clPrintKernelInfo(kernel, clKernelInfoType[i].info);
}

#ifdef CL_TEST
int main(int argc, char *argv[])
{
    cl_uint        p,d;
    cl_platform_id platform_id[CL_ITEM_MAX];
    cl_device_id   device_id[CL_ITEM_MAX];
    cl_int         err = 0;
    cl_uint        numPlatforms = 0;
    cl_uint        numDevices = 0;

    err = clGetPlatformIDs(CL_ITEM_MAX, &platform_id[0], &numPlatforms);
    if (err != CL_SUCCESS)
    {
        printf("Error in OpenCL Platform ID API\n");
    }
    else
    {
        for (p = 0; p < numPlatforms; p++)
        {
            cl_uint i = 0;
            clPrintAllPlatformInfo(platform_id[p]);
            err = clGetDeviceIDs(platform_id[p], CL_DEVICE_TYPE_ALL, 1, &device_id[0], &numDevices);
            if (err != CL_SUCCESS)
            {
                printf("Failed to acquire ID of OpenCL CPU\n");
                return -1;
            }
            else
            {
                cl_context context = 0;
                cl_context_properties props[] = {(cl_context_properties)CL_CONTEXT_PLATFORM,
                                                 (cl_context_properties)platform_id[p],
                                                 (cl_context_properties)0};

                for (d = 0; d < numDevices; d++)
                {
                    clPrintAllDeviceInfo(device_id[d]);

                    // create a context for this device....
                    context = clCreateContext(props,1,&device_id[d], clQueryNotify, NULL, &err);
                    if (context == NULL)
                    {
                        clPrintError(err);
                        printf("Failed to create a context for device %p, err=%i!\n",device_id[p],err);
                        continue;
                    }
                    else
                    {
                        cl_command_queue queue;
                        cl_command_queue_properties cmdprop = 0;
#ifdef CL_DEBUG
                        printf("Created Context %p\n",context);
#endif
						clPrintAllContextInfo(context);
						
                        queue = clCreateCommandQueue(context, device_id[d], cmdprop, &err);
                        if (queue != NULL)
                        {
                            //cl_mem memory_objects[CL_ITEM_MAX];
#ifdef CL_DEBUG
                            printf("Created Command Queue %p\n",queue);
#endif
                            clReleaseCommandQueue(queue);
                        }
                    }
                    clReleaseContext(context);
                }
            }
        }
    }
    return 0;
}
#endif