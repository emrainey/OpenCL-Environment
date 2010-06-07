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
#include <CL/opencl.h>
#else
#include <OpenCL/opencl.h>
#endif
#include <clenvironment.h>

typedef enum _cl_type_e { CL_STRING_TYPE, CL_ULONG_TYPE, CL_UINT_TYPE, CL_BYTE_TYPE} cl_type_e;

#define CL_ITEM_MAX     (10)

typedef struct _cl_error_strings_t {
    char *string;
    cl_uint err;
} cl_error_string_t;

cl_error_string_t errors[] = {
    {STRINGERIZE(CL_INVALID_VALUE)},
    {STRINGERIZE(CL_INVALID_DEVICE_TYPE)},
    {STRINGERIZE(CL_INVALID_PLATFORM)},
    {STRINGERIZE(CL_INVALID_DEVICE)},
    {STRINGERIZE(CL_INVALID_CONTEXT)},
    {STRINGERIZE(CL_INVALID_QUEUE_PROPERTIES)},
    {STRINGERIZE(CL_INVALID_COMMAND_QUEUE)},
    {STRINGERIZE(CL_INVALID_HOST_PTR)},
    {STRINGERIZE(CL_INVALID_MEM_OBJECT)},
    {STRINGERIZE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)},
    {STRINGERIZE(CL_INVALID_IMAGE_SIZE)},
    {STRINGERIZE(CL_INVALID_SAMPLER)},
    {STRINGERIZE(CL_INVALID_BINARY)},
    {STRINGERIZE(CL_INVALID_BUILD_OPTIONS)},
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
    {CL_STRING_TYPE, STRINGERIZE(CL_PLATFORM_PROFILE)},
    {CL_STRING_TYPE, STRINGERIZE(CL_PLATFORM_VERSION)},
    {CL_STRING_TYPE, STRINGERIZE(CL_PLATFORM_NAME)},
    {CL_STRING_TYPE, STRINGERIZE(CL_PLATFORM_VENDOR)},
    {CL_STRING_TYPE, STRINGERIZE(CL_PLATFORM_EXTENSIONS)},
};
cl_info_type_t clDeviceInfoType[] = {
    {CL_ULONG_TYPE, STRINGERIZE(CL_DEVICE_TYPE)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_VENDOR_ID)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MAX_COMPUTE_UNITS)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MAX_WORK_GROUP_SIZE)},
    {CL_STRING_TYPE, STRINGERIZE(CL_DEVICE_MAX_WORK_ITEM_SIZES)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MAX_CLOCK_FREQUENCY)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_ADDRESS_BITS)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MAX_READ_IMAGE_ARGS)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MAX_WRITE_IMAGE_ARGS)},
    {CL_ULONG_TYPE, STRINGERIZE(CL_DEVICE_MAX_MEM_ALLOC_SIZE)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_IMAGE2D_MAX_WIDTH)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_IMAGE2D_MAX_HEIGHT)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_IMAGE3D_MAX_WIDTH)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_IMAGE3D_MAX_HEIGHT)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_IMAGE3D_MAX_DEPTH)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_IMAGE_SUPPORT)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MAX_PARAMETER_SIZE)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MAX_SAMPLERS)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MEM_BASE_ADDR_ALIGN)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE)},
    {CL_ULONG_TYPE, STRINGERIZE(CL_DEVICE_SINGLE_FP_CONFIG)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE)},
    {CL_ULONG_TYPE, STRINGERIZE(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE)},
    {CL_ULONG_TYPE, STRINGERIZE(CL_DEVICE_GLOBAL_MEM_SIZE)},
    {CL_ULONG_TYPE, STRINGERIZE(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_MAX_CONSTANT_ARGS)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_LOCAL_MEM_TYPE)},
    {CL_ULONG_TYPE, STRINGERIZE(CL_DEVICE_LOCAL_MEM_SIZE)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_ERROR_CORRECTION_SUPPORT)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_PROFILING_TIMER_RESOLUTION)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_ENDIAN_LITTLE)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_AVAILABLE)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_COMPILER_AVAILABLE)},
    {CL_ULONG_TYPE, STRINGERIZE(CL_DEVICE_EXECUTION_CAPABILITIES)},
    {CL_ULONG_TYPE, STRINGERIZE(CL_DEVICE_QUEUE_PROPERTIES)},
    {CL_STRING_TYPE, STRINGERIZE(CL_DEVICE_NAME)},
    {CL_STRING_TYPE, STRINGERIZE(CL_DEVICE_VENDOR)},
    {CL_STRING_TYPE, STRINGERIZE(CL_DEVICE_VERSION)},
    {CL_STRING_TYPE, STRINGERIZE(CL_DEVICE_PROFILE)},
    {CL_STRING_TYPE, STRINGERIZE(CL_DEVICE_EXTENSIONS)},
    {CL_UINT_TYPE, STRINGERIZE(CL_DEVICE_PLATFORM)},
};
cl_info_type_t clContextInfoTypes[] = {
    {CL_STRING_TYPE, STRINGERIZE(CL_CONTEXT_REFERENCE_COUNT)},
    {CL_STRING_TYPE, STRINGERIZE(CL_CONTEXT_DEVICES)},
    {CL_STRING_TYPE, STRINGERIZE(CL_CONTEXT_PROPERTIES)},
};
cl_uint numPlatInfoTypes = dimof(clPlatInfoType);
cl_uint numDeviceInfoTypes = dimof(clDeviceInfoType);
cl_uint numContextInfoTypes = dimof(clContextInfoTypes);

void clPrintPlatformInfo(cl_platform_id plat, cl_platform_info info)
{
    cl_uint i = 0;
    size_t size = 0;
    for (i = 0; i < numPlatInfoTypes; i++)
    {
        if (info == clPlatInfoType[i].info)
        {
            switch (clPlatInfoType[i].type)
            {
                case CL_STRING_TYPE:
                {
                    cl_char param[CL_MAX_STRING];
                    memset(param, 0, sizeof(param));
                    clGetPlatformInfo(plat, info, sizeof(param), param, &size);
                    if (size > 0) printf("%41s:[%02lu] %s\n", clPlatInfoType[i].name, size, param);
                    break;
                }
                case CL_ULONG_TYPE:
                {
                    cl_ulong param;
                    clGetPlatformInfo(plat, info, sizeof(param), &param, &size);
                    if (size == 8) printf("%41s:[%02lu] %llu\n", clPlatInfoType[i].name, size, param);
                    break;
                }
                case CL_UINT_TYPE:
                {
                    cl_uint param;
                    clGetPlatformInfo(plat, info, sizeof(param), &param, &size);
                    if (size == 4) printf("%41s:[%02lu] 0x%08x\n", clPlatInfoType[i].name, size, param);
                    break;
                }
                case CL_BYTE_TYPE:
                {
                    cl_char param;
                    clGetPlatformInfo(plat, info, sizeof(param), &param, &size);
                    if (size == 1) printf("%41s:[%02lu] 0x%02x\n", clPlatInfoType[i].name, size, param);
                    break;
                }
            }
        }
    }
}

void clPrintDeviceInfo(cl_device_id plat, cl_device_info info)
{
    cl_uint i = 0;
    size_t size = 0;
    for (i = 0; i < numDeviceInfoTypes; i++)
    {
        if (info == clDeviceInfoType[i].info)
        {
            switch (clDeviceInfoType[i].type)
            {
                case CL_STRING_TYPE:
                {
                    cl_char param[CL_MAX_STRING];
                    memset(param, 0, sizeof(param));
                    clGetDeviceInfo(plat, info, sizeof(param), param, &size);
                    if (size > 0) printf("%41s:[%02lu] %s\n", clDeviceInfoType[i].name, size, param);
                    break;
                }
                case CL_ULONG_TYPE:
                {
                    cl_ulong param;
                    clGetDeviceInfo(plat, info, sizeof(param), &param, &size);
                    if (size == 8) printf("%41s:[%02lu] %llu\n", clDeviceInfoType[i].name, size, param);
                    break;
                }
                case CL_UINT_TYPE:
                {
                    cl_uint param;
                    clGetDeviceInfo(plat, info, sizeof(param), &param, &size);
                    if (size == 4) printf("%41s:[%02lu] 0x%08x\n", clDeviceInfoType[i].name, size, param);
                    break;
                }
                case CL_BYTE_TYPE:
                {
                    cl_char param;
                    clGetDeviceInfo(plat, info, sizeof(param), &param, &size);
                    if (size == 1) printf("%41s:[%02lu] 0x%02x\n", clDeviceInfoType[i].name, size, param);
                    break;
                }
            }
        }
    }
}

void clPrintContextInfo(cl_context context, cl_context_info info)
{
    cl_uint i = 0;
    size_t size = 0;
    for (i = 0; i < numPlatInfoTypes; i++)
    {
        if (info == clPlatInfoType[i].info)
        {
            switch (clPlatInfoType[i].type)
            {
                case CL_STRING_TYPE:
                {
                    cl_char param[CL_MAX_STRING];
                    memset(param, 0, sizeof(param));
                    clGetContextInfo(context, info, sizeof(param), param, &size);
                    if (size > 0) printf("%41s:[%02lu] %s\n", clContextInfoTypes[i].name, size, param);
                    break;
                }
                case CL_ULONG_TYPE:
                {
                    cl_ulong param;
                    clGetContextInfo(context, info, sizeof(param), &param, &size);
                    if (size == 8) printf("%41s:[%02lu] %llu\n", clContextInfoTypes[i].name, size, param);
                    break;
                }
                case CL_UINT_TYPE:
                {
                    cl_uint param;
                    clGetContextInfo(context, info, sizeof(param), &param, &size);
                    if (size == 4) printf("%41s:[%02lu] 0x%08x\n", clContextInfoTypes[i].name, size, param);
                    break;
                }
                case CL_BYTE_TYPE:
                {
                    cl_char param;
                    clGetContextInfo(context, info, sizeof(param), &param, &size);
                    if (size == 1) printf("%41s:[%02lu] 0x%02x\n", clContextInfoTypes[i].name, size, param);
                    break;
                }
            }
        }
    }
}

void clQueryNotify(const char *errinfo,
                   const void * private_info,
                   size_t cb,
                   void * user_data)
{
    printf("ERROR! %s\n",errinfo);
}

typedef struct _cl_system_t {
    cl_platform_id 	platform_id[CL_ITEM_MAX];
    cl_uint 		numPlatforms;
    cl_device_id 	device_id[CL_ITEM_MAX];
    cl_uint 		numDevices;
} cl_system_t;

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
            for (i = 0; i < dimof(clPlatInfoType); i++)
                clPrintPlatformInfo(platform_id[p], clPlatInfoType[i].info);

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
                    cl_uint i = 0;
                    for (i = 0; i < dimof(clDeviceInfoType); i++)
                        clPrintDeviceInfo(device_id[d],clDeviceInfoType[i].info);

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
                        cl_uint i = 0;
                        for (i = 0; i < dimof(clContextInfoTypes); i++)
                            clPrintContextInfo(context, clContextInfoTypes[i].info);

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
