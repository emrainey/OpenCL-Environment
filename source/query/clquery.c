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
#ifdef DARWIN
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <clenvironment.h>

typedef enum _cl_type_e {
    CL_STRING_TYPE,     // char[]
    CL_ULONG_TYPE,      // cl_ulong
    CL_SIZET_TYPE,      // size_t
    CL_UINT_TYPE,       // cl_uint
    CL_BOOL_TYPE,       // cl_bool
    CL_DEVFPCFG_TYPE,   // cl_device_fp_config
    CL_DEVEXCCAP_TYPE,  // cl_device_exec_capabilities
    CL_DEVMEMCHE_TYPE,  // cl_device_mem_cache_type
    CL_LCLDEVMEM_TYPE,  // cl_device_local_mem_type
    CL_PLATFORM_TYPE,   // cl_platform_id
    CL_CMDQUEUE_TYPE,   // cl_command_queue_properties
    CL_DEVTYPE_TYPE,    // cl_device_type
} cl_type_e;

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
    {L_STRINGERIZE(CL_INVALID_KERNEL_DEFINITION)},
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
    cl_uint   count;
    char     *name;
    cl_uint   info;
} cl_info_type_t;

cl_info_type_t clPlatInfoType[] = {
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_PLATFORM_PROFILE)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_PLATFORM_VERSION)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_PLATFORM_NAME)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_PLATFORM_VENDOR)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_PLATFORM_EXTENSIONS)},
};
cl_info_type_t clDeviceInfoType[] = {
    {CL_DEVTYPE_TYPE, 1, L_STRINGERIZE(CL_DEVICE_TYPE)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_VENDOR_ID)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_COMPUTE_UNITS)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS)},
    {CL_SIZET_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_WORK_GROUP_SIZE)},
    {CL_SIZET_TYPE, 3, L_STRINGERIZE(CL_DEVICE_MAX_WORK_ITEM_SIZES)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_CLOCK_FREQUENCY)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_ADDRESS_BITS)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_READ_IMAGE_ARGS)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_WRITE_IMAGE_ARGS)},
    {CL_ULONG_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_MEM_ALLOC_SIZE)},
    {CL_SIZET_TYPE, 1, L_STRINGERIZE(CL_DEVICE_IMAGE2D_MAX_WIDTH)},
    {CL_SIZET_TYPE, 1, L_STRINGERIZE(CL_DEVICE_IMAGE2D_MAX_HEIGHT)},
    {CL_SIZET_TYPE, 1, L_STRINGERIZE(CL_DEVICE_IMAGE3D_MAX_WIDTH)},
    {CL_SIZET_TYPE, 1, L_STRINGERIZE(CL_DEVICE_IMAGE3D_MAX_HEIGHT)},
    {CL_SIZET_TYPE, 1, L_STRINGERIZE(CL_DEVICE_IMAGE3D_MAX_DEPTH)},
    {CL_BOOL_TYPE, 1, L_STRINGERIZE(CL_DEVICE_IMAGE_SUPPORT)},
    {CL_SIZET_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_PARAMETER_SIZE)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_SAMPLERS)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MEM_BASE_ADDR_ALIGN)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE)},
    {CL_DEVFPCFG_TYPE, 1, L_STRINGERIZE(CL_DEVICE_SINGLE_FP_CONFIG)},
#if 0
    {CL_DEVFPCFG_TYPE, 1, L_STRINGERIZE(CL_DEVICE_HALF_FP_CONFIG)},
    {CL_DEVFPCFG_TYPE, 1, L_STRINGERIZE(CL_DEVICE_DOUBLE_FP_CONFIG)},
#endif
    {CL_DEVMEMCHE_TYPE, 1, L_STRINGERIZE(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE)},
    {CL_ULONG_TYPE, 1, L_STRINGERIZE(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE)},
    {CL_ULONG_TYPE, 1, L_STRINGERIZE(CL_DEVICE_GLOBAL_MEM_SIZE)},
    {CL_ULONG_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE)},
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_DEVICE_MAX_CONSTANT_ARGS)},
    {CL_LCLDEVMEM_TYPE, 1, L_STRINGERIZE(CL_DEVICE_LOCAL_MEM_TYPE)},
    {CL_ULONG_TYPE, 1, L_STRINGERIZE(CL_DEVICE_LOCAL_MEM_SIZE)},
    {CL_BOOL_TYPE, 1, L_STRINGERIZE(CL_DEVICE_ERROR_CORRECTION_SUPPORT)},
    {CL_SIZET_TYPE, 1, L_STRINGERIZE(CL_DEVICE_PROFILING_TIMER_RESOLUTION)},
    {CL_BOOL_TYPE, 1, L_STRINGERIZE(CL_DEVICE_ENDIAN_LITTLE)},
    {CL_BOOL_TYPE, 1, L_STRINGERIZE(CL_DEVICE_AVAILABLE)},
    {CL_BOOL_TYPE, 1, L_STRINGERIZE(CL_DEVICE_COMPILER_AVAILABLE)},
    {CL_DEVEXCCAP_TYPE, 1, L_STRINGERIZE(CL_DEVICE_EXECUTION_CAPABILITIES)},
    {CL_CMDQUEUE_TYPE, 1, L_STRINGERIZE(CL_DEVICE_QUEUE_PROPERTIES)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_DEVICE_NAME)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_DEVICE_VENDOR)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_DEVICE_VERSION)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_DEVICE_PROFILE)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_DEVICE_EXTENSIONS)},
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_DRIVER_VERSION)},
    {CL_PLATFORM_TYPE, 1, L_STRINGERIZE(CL_DEVICE_PLATFORM)},
};
cl_info_type_t clContextInfoType[] = {
    {CL_UINT_TYPE, 1, L_STRINGERIZE(CL_CONTEXT_REFERENCE_COUNT)},
    {CL_ULONG_TYPE, 1, L_STRINGERIZE(CL_CONTEXT_DEVICES)},
    {CL_UINT_TYPE, 10, L_STRINGERIZE(CL_CONTEXT_PROPERTIES)},
};
cl_info_type_t clQueueInfoType[] = {
    {CL_UINT_TYPE,   1,L_STRINGERIZE(CL_QUEUE_CONTEXT)},
    {CL_UINT_TYPE,   1,L_STRINGERIZE(CL_QUEUE_DEVICE)},
    {CL_UINT_TYPE,   1,L_STRINGERIZE(CL_QUEUE_REFERENCE_COUNT)},
    {CL_UINT_TYPE,   10,L_STRINGERIZE(CL_QUEUE_PROPERTIES)},
};
cl_info_type_t clKernelInfoType[] = {
    {CL_STRING_TYPE, CL_MAX_STRING, L_STRINGERIZE(CL_KERNEL_FUNCTION_NAME)},
    {CL_UINT_TYPE,   1,L_STRINGERIZE(CL_KERNEL_NUM_ARGS)},
    {CL_UINT_TYPE,   1,L_STRINGERIZE(CL_KERNEL_REFERENCE_COUNT)},
    {CL_UINT_TYPE,   1,L_STRINGERIZE(CL_KERNEL_CONTEXT)},
    {CL_UINT_TYPE,   1,L_STRINGERIZE(CL_KERNEL_PROGRAM)},
};
cl_info_type_t clKernelWorkInfoType[] = {
    {CL_SIZET_TYPE,  1, L_STRINGERIZE(CL_KERNEL_WORK_GROUP_SIZE)},
    {CL_SIZET_TYPE,  3, L_STRINGERIZE(CL_KERNEL_COMPILE_WORK_GROUP_SIZE)},
    {CL_ULONG_TYPE,  1, L_STRINGERIZE(CL_KERNEL_LOCAL_MEM_SIZE)},
};
cl_uint numPlatInfoTypes = dimof(clPlatInfoType);
cl_uint numDeviceInfoTypes = dimof(clDeviceInfoType);
cl_uint numContextInfoTypes = dimof(clContextInfoType);
cl_uint numQueueInfoTypes = dimof(clQueueInfoType);
cl_uint numKernelInfoTypes = dimof(clKernelInfoType);
cl_uint numKernelWorkInfoTypes = dimof(clKernelWorkInfoType);

/** This function allows us to type-cast the various query functions */
typedef cl_int (*QueryInfo_f)(void *, cl_uint, size_t, void *, size_t *);

// atomic or array
// query format or not
// size == sizeof(param) or > 0
// format string type
//

#define CL_QUERY_ATOMIC_TYPE(err, type, query, id, info, size, types, index, format) {\
    type param;\
    char format_str[CL_MAX_STRING];\
    memset(&param, 0, sizeof(param));\
    err = query(id, info, sizeof(param), &param, &size);\
    if (size == sizeof(type)) {\
        sprintf(format_str, "%%41s:[%%02lu] %s\n", format);\
        printf(format_str, types[index].name, size, param);\
    } else {\
        printf("%41s:[%02lu] NO RETURNED VALUE! (err = %d)\n", types[index].name, size, err);\
    }\
}

#define CL_QUERY_ARRAY_TYPE(err, type, count, query, id, info, size, types, index, format) {\
    type *param = (type *)malloc(sizeof(type)*count);\
    if (param != NULL) {\
        char format_str[CL_MAX_STRING];\
        memset(param, 0, sizeof(type)*count);\
        err = query(id, info, sizeof(type)*count, param, &size);\
        if (size/sizeof(type) > 0) {\
            int j;\
            if (strcmp(format, "%s") == 0) {\
                printf("%41s:[%02lu] %s\n", types[index].name, size, param);\
            } else {\
                printf("%41s:[%02lu] ", types[index].name, size);\
                sprintf(format_str, "%s",  format);\
                for (j = 0; j < size/sizeof(type); j++) {\
                    printf(format_str, param[j]);\
                } printf("\n");\
            }\
        } else { \
            printf("%41s:[%02lu] NO RETURNED VALUE (err = %d)\n", types[index].name, size, err);\
        }\
        free(param);\
    }\
}

void clPrintQueryInfo(void *id, cl_uint info, QueryInfo_f query, cl_info_type_t *infoTypes, cl_uint numInfoTypes)
{
    cl_int err  = CL_SUCCESS;
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
                    CL_QUERY_ARRAY_TYPE(err, cl_char, infoTypes[i].count, query, id, info, size, infoTypes, i, "%s");
                    break;
                }
                case CL_ULONG_TYPE:
                {
                    if (infoTypes[i].count == 1) {
                        CL_QUERY_ATOMIC_TYPE(err, cl_ulong, query, id, info, size, infoTypes, i, "%llu");
                    } else {
                        CL_QUERY_ARRAY_TYPE(err, cl_ulong, infoTypes[i].count, query, id, info, size, infoTypes, i, "%llu ");
                    }
                    break;
                }
                case CL_UINT_TYPE:
                {
                    if (infoTypes[i].count == 1) {
                        CL_QUERY_ATOMIC_TYPE(err, cl_uint, query, id, info, size, infoTypes, i, "%lu");
                    } else {
                        CL_QUERY_ARRAY_TYPE(err, cl_uint, infoTypes[i].count, query, id, info, size, infoTypes, i, "%lu ");
                    }
                    break;
                }
                case CL_SIZET_TYPE:
                {
                    if (infoTypes[i].count == 1) {
                        CL_QUERY_ATOMIC_TYPE(err, size_t, query, id, info, size, infoTypes, i, "%llu");
                    } else {
                        CL_QUERY_ARRAY_TYPE(err, size_t, infoTypes[i].count, query, id, info, size, infoTypes, i, "%llu ");
                    }
                    break;
                }
                case CL_BOOL_TYPE:
                {
                    CL_QUERY_ATOMIC_TYPE(err, cl_bool, query, id, info, size, infoTypes, i, (param?"TRUE":"FALSE"));
                    break;
                }
                case CL_DEVMEMCHE_TYPE:
                {
                    CL_QUERY_ATOMIC_TYPE(err, cl_device_mem_cache_type, query, id, info, size, infoTypes, i, "0x%08x");
                    break;
                }
                case CL_DEVFPCFG_TYPE:
                {
                    CL_QUERY_ATOMIC_TYPE(err, cl_device_fp_config, query, id, info, size, infoTypes, i, "0x%08x");
                    break;
                }
                case CL_DEVEXCCAP_TYPE:
                {
                    CL_QUERY_ATOMIC_TYPE(err, cl_device_exec_capabilities, query, id, info, size, infoTypes, i, "0x%08x");
                    break;
                }
                case CL_LCLDEVMEM_TYPE:
                {
                    CL_QUERY_ATOMIC_TYPE(err, cl_device_local_mem_type, query, id, info, size, infoTypes, i, "0x%08x");
                    break;
                }
                case CL_PLATFORM_TYPE:
                {
                    CL_QUERY_ATOMIC_TYPE(err, cl_platform_id, query, id, info, size, infoTypes, i, "0x%08x");
                    break;
                }
                case CL_CMDQUEUE_TYPE:
                {
                    CL_QUERY_ATOMIC_TYPE(err, cl_command_queue_properties, query, id, info, size, infoTypes, i, "0x%08x");
                    break;
                }
                case CL_DEVTYPE_TYPE:
                {
                    CL_QUERY_ATOMIC_TYPE(err, cl_device_type, query, id, info, size, infoTypes, i, "0x%08x");
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

void clPrintKernelWorkInfo(cl_kernel kernel, cl_device_id device, cl_kernel_info info)
{
    cl_int err = CL_SUCCESS;
    cl_uint i = 0;
    size_t size = 0;
    for (i = 0; i < numKernelWorkInfoTypes; i++)
    {
        if (info == clKernelWorkInfoType[i].info)
        {
            switch (clKernelWorkInfoType[i].type)
            {
                case CL_ULONG_TYPE:
                {
                    cl_ulong param;
                    err = clGetKernelWorkGroupInfo(kernel, device, info, sizeof(param), &param, &size);
                    if (size == sizeof(param))
                        printf("%41s:[%02lu] %llu\n", clKernelWorkInfoType[i].name, size, param);
                    else
                        printf("%41s:[00] NO RETURNED VALUE! (err = %d)\n",clKernelWorkInfoType[i].name, err);
                    break;
                }
                case CL_SIZET_TYPE:
                {
                    if (clKernelWorkInfoType[i].count == 1)
                    {
                        size_t param;
                        err = clGetKernelWorkGroupInfo(kernel, device, info, sizeof(param), &param, &size);
                        if (size == sizeof(param))
                            printf("%41s:[%02lu] %llu\n", clKernelWorkInfoType[i].name, size, param);
                        else
                            printf("%41s:[00] NO RETURNED VALUE! (err = %d)\n",clKernelWorkInfoType[i].name, err);
                    }
                    else
                    {
                        size_t param[3];
                        err = clGetKernelWorkGroupInfo(kernel, device, info, sizeof(param), &param, &size);
                        if (size == sizeof(param))
                            printf("%41s:[%02lu] %llu %llu %llu\n", clKernelWorkInfoType[i].name, size, param[0], param[1], param[2]);
                        else
                            printf("%41s:[00] NO RETURNED VALUE! (err = %d)\n",clKernelWorkInfoType[i].name, err);
                    }
                    break;
                }
            }
        }
    }
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

void clPrintAllKernelWorkInfo(cl_kernel kernel, cl_device_id device)
{
    cl_uint i = 0;
    for (i = 0; i < numKernelWorkInfoTypes; i++)
        clPrintKernelWorkInfo(kernel, device, clKernelWorkInfoType[i].info);
}
