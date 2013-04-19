/*
 *  Copyright (C) 2009-2012 Texas Instruments, Inc.
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
 * \brief The "OpenCL Environment" core features.
 * \see http://github.com/emrainey/OpenCL-Environment
 * \author Erik Rainey <erik.rainey@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DARWIN
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <clquery.h>
#include <clenvironment.h>

#define CL_LOG_SIZE     (1024*1024)

void cl_free(void *ptr)
{
#ifdef CL_DEBUG
    //printf("Freeing %p\n",ptr);
#endif
    free(ptr);
}

void *cl_malloc(size_t numBytes)
{
    void *ptr = NULL;
    if (numBytes == 0)
        printf("WARNING: ALLOCATING A BUFFER OF ZERO BYTES!!\n");
    ptr = malloc(numBytes);
#ifdef CL_DEBUG
    //printf("Malloc'd %p for %lu bytes\n",ptr,numBytes);
#endif
    if (ptr)
        memset(ptr, 0, numBytes);
    return ptr;
}

size_t flen(FILE *fp)
{
    size_t len = 0;
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return len;
}

size_t flines(FILE *fp)
{
    size_t numLines = 0;
    char line[1024];
    fseek(fp, 0, SEEK_SET);
    while (fgets(line, sizeof(line), fp) > 0) {
        numLines++;
    }
#ifdef CL_DEBUG
    printf("%lu lines in file %p\n",numLines,fp);
#endif
    fseek(fp, 0, SEEK_SET);
    return numLines;
}

/*
static char *strlastitem(char *str, char item)
{
    int last = 0, i = 0;
    char *s = str;
    do {
        if (s[i] == item)
            last = i;
    } while (s[i++] != '\0');
    if (s[] != item)
        s = str;
    return s;
}
*/

#define CASE_STRINGERIZE(err, label, function, file, line) \
    case err: \
        fprintf(stderr, "%s: OpenCL error "#err" at %s in %s:%d\n", label, function, file, line); \
        break

cl_int clBuildError(cl_int build_status, char *label, const char *function, const char *file, int line)
{
    switch (build_status)
    {
        case CL_BUILD_SUCCESS:
            break;
        CASE_STRINGERIZE(CL_BUILD_NONE, label, function, file, line);
        CASE_STRINGERIZE(CL_BUILD_ERROR, label, function, file, line);
        CASE_STRINGERIZE(CL_BUILD_IN_PROGRESS, label, function, file, line);
        default:
            fprintf(stderr, "%s: Unknown build error %d at %s in %s:%d\n", label, build_status, function, file, line);
            break;
    }
    return build_status;
}

cl_int clPrintError(cl_int err, char *label, const char *function, const char *file, int line)
{
    switch (err)
    {
        case CL_SUCCESS:
            break;
        CASE_STRINGERIZE(CL_BUILD_PROGRAM_FAILURE, label, function, file, line);
        CASE_STRINGERIZE(CL_COMPILER_NOT_AVAILABLE, label, function, file, line);
        CASE_STRINGERIZE(CL_DEVICE_NOT_AVAILABLE, label, function, file, line);
        CASE_STRINGERIZE(CL_DEVICE_NOT_FOUND, label, function, file, line);
        CASE_STRINGERIZE(CL_IMAGE_FORMAT_MISMATCH, label, function, file, line);
        CASE_STRINGERIZE(CL_IMAGE_FORMAT_NOT_SUPPORTED, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_ARG_INDEX, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_ARG_SIZE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_ARG_VALUE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_BINARY, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_BUFFER_SIZE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_BUILD_OPTIONS, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_COMMAND_QUEUE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_CONTEXT, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_DEVICE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_DEVICE_TYPE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_EVENT, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_EVENT_WAIT_LIST, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_GL_OBJECT, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_GLOBAL_OFFSET, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_HOST_PTR, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_IMAGE_SIZE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_KERNEL_NAME, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_KERNEL, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_KERNEL_ARGS, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_KERNEL_DEFINITION, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_MEM_OBJECT, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_OPERATION, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_PLATFORM, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_PROGRAM, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_PROGRAM_EXECUTABLE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_QUEUE_PROPERTIES, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_SAMPLER, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_VALUE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_WORK_DIMENSION, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_WORK_GROUP_SIZE, label, function, file, line);
        CASE_STRINGERIZE(CL_INVALID_WORK_ITEM_SIZE, label, function, file, line);
        CASE_STRINGERIZE(CL_MAP_FAILURE, label, function, file, line);
        CASE_STRINGERIZE(CL_MEM_OBJECT_ALLOCATION_FAILURE, label, function, file, line);
        CASE_STRINGERIZE(CL_MEM_COPY_OVERLAP, label, function, file, line);
        CASE_STRINGERIZE(CL_OUT_OF_HOST_MEMORY, label, function, file, line);
        CASE_STRINGERIZE(CL_OUT_OF_RESOURCES, label, function, file, line);
        CASE_STRINGERIZE(CL_PROFILING_INFO_NOT_AVAILABLE, label, function, file, line);
        default:
            fprintf(stderr, "%s: Unknown error %d at %s in %s:%d\n", label, err, function, file, line);
            break;
    }
    return err;
}

void clPrintEnvironment(cl_environment_t *pEnv)
{
    if (pEnv)
    {
        printf("Environment: %p\n"
               "\t%u devices\n"
               "\t%u kernels\n",
               pEnv, pEnv->numDevices, pEnv->numKernels);
    }
}

void clDeleteEnvironment(cl_environment_t *pEnv)
{
    if (pEnv)
    {
        cl_uint i = 0;
        if (pEnv->kernels)
        {
            for(i = 0; i < pEnv->numKernels; i++)
                clReleaseKernel(pEnv->kernels[i]);
            cl_free(pEnv->kernels);
        }
        if (pEnv->retCodes)
            cl_free(pEnv->retCodes);
        if (pEnv->program)
            clReleaseProgram(pEnv->program);
        if (pEnv->queues)
        {
            for (i = 0; i < pEnv->numDevices; i++)
            {
                clReleaseCommandQueue(pEnv->queues[i]);
            }
            cl_free(pEnv->queues);
        }
        if (pEnv->sources)
        {
            for (i = 0; i < pEnv->numLines; i++)
            {
                cl_free(pEnv->sources[i]);
                pEnv->sources[i] = NULL;
            }
            cl_free(pEnv->lengths);
            cl_free(pEnv->sources);
        }
        if (pEnv->context)
            clReleaseContext(pEnv->context);
        if (pEnv->devices)
            cl_free(pEnv->devices);

        cl_free(pEnv);
    }
}


void cl_dump_bin(char *filename, cl_byte *bin, size_t numBytes)
{
    FILE *fp = fopen(filename, "wb+");
    if (fp)
    {
#ifdef CL_DEBUG
        printf("Writing from %p to file %s for %lu bytes\n", bin, filename, numBytes);
#endif
        fwrite(bin, 1, numBytes, fp);
#ifdef CL_DEBUG
        printf("Wrote file %s\n",filename);
#endif
        fclose(fp);
    }
}

cl_byte *cl_load_bin(char *filename, size_t *pNumBytes)
{
    cl_byte *bin = NULL;
    FILE *fp = fopen(filename, "rb");
    if (fp != NULL)
    {
        size_t numBytes = flen(fp);
        bin = (cl_byte *)cl_malloc(numBytes);
        if (bin)
        {
            size_t b = fread(bin, 1, numBytes, fp);
#ifdef CL_DEBUG
            printf("Read "FMT_SIZE_T" bytes\n", b);
#endif
            fclose(fp);
        }
    }
    return bin;
}

cl_byte *cl_serialize_kernels(cl_kernel_bin_t *bins, size_t *pNumBytes)
{
    cl_uint i = 0;
    cl_byte *bin = NULL;

    *pNumBytes = bins->numDevices*sizeof(size_t) + sizeof(size_t);
    for (i = 0; i < bins->numDevices; i++)
    {
#ifdef CL_DEBUG
        printf("Serialized clBinary [%02u]: %u bytes\n", i, bins->sizes[i]);
#endif
        *pNumBytes += bins->sizes[i];
    }
#ifdef CL_DEBUG
    printf("There are %lu total bytes to serialize\n",*pNumBytes);
#endif
    bin = cl_malloc_array(cl_byte,(*pNumBytes));
    if (bin)
    {
        size_t offset = 0;
        memcpy(&bins[offset], &bins->deviceTypes, sizeof(cl_uint));
        offset += sizeof(cl_uint);
        memcpy(&bins[offset], &bins->numDevices, sizeof(size_t));
        offset += sizeof(size_t);
        for (i = 0; i < bins->numDevices; i++)
        {
            memcpy(&bins[offset], &bins->sizes[i], sizeof(size_t));
            offset += sizeof(size_t);
            memcpy(&bins[offset], &bins->data[i], bins->sizes[i]);
            offset += bins->sizes[i];
        }
        if (offset != *pNumBytes)
            printf("Warning: Serialization totals are off!\n");
    }
    else
    {
        *pNumBytes = 0;
    }
    return bin;
}

void cl_delete_kernel_bin(cl_kernel_bin_t *bins)
{
    if (bins)
    {
        cl_uint i = 0;
        if (bins->data)
        {
            for (i = 0; i < bins->numDevices; i++)
            {
                if (bins->data[i])
                {
                    cl_free(bins->data[i]);
                    bins->data[i] = NULL;
                }
            }
            cl_free(bins->data);
            bins->data = NULL;
        }
        if (bins->sizes)
        {
            cl_free(bins->sizes);
            bins->sizes = NULL;
        }
        // @TODO Fill in the structure with a bunch of garbage which will cause an abort later if dereferenced.
        //memset(bins, 0xFE, sizeof(cl_kernel_bin_t));
        cl_free(bins);
    }
}

cl_kernel_bin_t *cl_create_kernel_bin(size_t numDevices)
{
    cl_kernel_bin_t *bins;

    bins = (cl_kernel_bin_t *)cl_malloc(sizeof(cl_kernel_bin_t));
    if (bins)
    {
        bins->deviceTypes = CL_DEVICE_TYPE_DEFAULT;
        bins->numDevices = numDevices;
        bins->numBytesSizes = sizeof(size_t) * bins->numDevices;
        bins->numBytesData = sizeof(unsigned char *) * bins->numDevices;
        bins->sizes = (size_t *)cl_malloc(bins->numBytesSizes);
        bins->data  = (unsigned char **)cl_malloc(bins->numBytesData);

        if (bins->sizes == NULL || bins->data == NULL)
        {
            cl_assert(bins->sizes != NULL && bins->data != NULL,);
            cl_delete_kernel_bin(bins);
            bins = NULL;
        }
        else
        {
            memset(bins->sizes, 0, bins->numBytesSizes);
            memset(bins->data, 0, bins->numBytesData);
        }
    }
    return bins;
}

cl_kernel_bin_t *cl_unserialize_kernels(cl_byte *bin, size_t numBytes)
{
    cl_uint i,deviceTypes = CL_DEVICE_TYPE_DEFAULT;
    size_t numDevices = 1;
    size_t offset = 0;
    cl_kernel_bin_t *bins = NULL;

    // minimum size
    if (numBytes <= (2* sizeof(size_t)))
        return NULL;

    memcpy(&deviceTypes, &bin[offset], sizeof(cl_uint));
    offset += sizeof(cl_uint);
    memcpy(&numDevices, &bin[offset], sizeof(size_t));
    offset += sizeof(size_t);
    printf("There are "FMT_SIZE_T" device kernels in the flat binary\n", numDevices);
    bins = cl_create_kernel_bin(numDevices);
    if (bins)
    {
        bins->deviceTypes = deviceTypes;
        bins->numBytesSizes = sizeof(size_t) * numDevices;
        bins->numBytesData  = sizeof(cl_byte *) * numDevices;
        for (i = 0; i < numDevices; i++)
        {
            memcpy(&bins->sizes[i], &bin[offset], sizeof(size_t));
            offset += sizeof(size_t);
            bins->data[i] = (cl_byte *)cl_malloc(bins->sizes[i]);
            if (bins->data[i] && offset < numBytes)
            {
                memcpy(bins->data[i], &bin[offset], bins->sizes[i]);
                offset += bins->sizes[i];
            }
            else
            {
                if (bins->data[i] == NULL)
                    printf("ERROR: Failed to allocate binary data array!\n");
                if (offset >= numBytes)
                    printf("ERROR: Offset is larger than read data\n");
                cl_delete_kernel_bin(bins);
                bins = NULL;
                break;
            }
        }
    }
    return bins;
}

void cl_dump_kernels(char *filename, cl_kernel_bin_t *bins)
{
    size_t numBytes = 0;
    cl_byte *bin = NULL;

    if (filename[0] == '\0')
        return;
    bin = cl_serialize_kernels(bins, &numBytes);
#ifdef CL_DEBUG
    printf("There are %lu binaries to write out\n", bins->numDevices);
#endif
    if (bin)
    {
        cl_dump_bin(filename, bin, numBytes);
        cl_free(bin);
    }
}

cl_kernel_bin_t *cl_extract_kernels(cl_program program)
{
    size_t numBinaries  = 0;
    size_t numBinaries2 = 0;
    cl_uint i = 0;
    cl_int err = CL_SUCCESS;
    size_t numDevices;
    cl_kernel_bin_t *bins = NULL;
    cl_device_id devices[CL_MAX_DEVICES];
    cl_device_type deviceTypes = 0;
    cl_build_status build_status = 0;

    err = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof(size_t), &numDevices, NULL);
    CL_ERROR(err, "clGetProgramInfo");
    if (err == CL_SUCCESS)
    {
#ifdef CL_DEBUG
        printf("There are %lu devices associated with this program %p.\n", numDevices, program);
#endif
        // get the devices
        err = clGetProgramInfo(program, CL_PROGRAM_DEVICES, sizeof(devices), devices, &numDevices);
        CL_ERROR(err, "clGetProgramInfo");
        numDevices /= sizeof(cl_device_id);
#ifdef CL_DEBUG
        printf("%u devices retrieved!\n",numDevices);
#endif
        for (i = 0; i < numDevices; i++)
        {
            cl_device_type tmp = 0;
            size_t bytes = 0;
            // query each device and ask it what type it is. OR that into a singe bit field

            err = clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(tmp), &tmp, &bytes);
            CL_ERROR(err, "clGetDeviceInfo");
            if (bytes == sizeof(tmp))
            {
                deviceTypes |= tmp;
            }
            else
            {
                printf("ERROR!!!! Can't receive the CL_DEVICE_TYPE!\n");
                return NULL;
            }
        }
#ifdef CL_DEBUG
        printf("CL_DEVICE_TYPE(s) = 0x%08x\n",deviceTypes);
#endif
        // did it compile successfully?
        err = clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_STATUS, sizeof(cl_int), &build_status, NULL);
        CL_ERROR(err, "clGetProgramBuildInfo");
        if (err != CL_SUCCESS)
        {
            printf("ERROR: Build was not successful! (Error=%d, Build_Status=%d)\n",err, build_status);
            return NULL;
        }
        if (build_status != CL_BUILD_SUCCESS)
        {
            size_t logSize = CL_LOG_SIZE;
            char *log = malloc(CL_LOG_SIZE);
            clBuildError(build_status, "clGetProgramBuildInfo", __FUNCTION__, __FILE__, __LINE__);
            clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, logSize, log, &logSize);
            printf("BUILD INFO ["FMT_SIZE_T"]:\n%s<<BUILD INFO\n", logSize, log);
            return NULL;
        }

        bins = cl_create_kernel_bin(numDevices);
        if (bins == NULL)
            return NULL;

        // set the device types
        bins->deviceTypes = deviceTypes;

        err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, bins->numBytesSizes, bins->sizes, &numBinaries);
        CL_ERROR(err, "clGetProgramInfo");
        if (err != CL_SUCCESS)
        {
            printf("Error: When requesting the number of binary outputs, error %d was returned\n", err);
            cl_delete_kernel_bin(bins);
            bins = NULL;
        }
        else if (err == CL_SUCCESS)
        {
            numBinaries /= sizeof(size_t);
#ifdef CL_DEBUG
            printf("There are %lu binaries associated with this program %p\n", numBinaries, program);
#endif
            for (i = 0; i < numBinaries; i++)
            {
#ifdef CL_DEBUG
                printf("Binary size for device %u is %lu\n", i, bins->sizes[i]);
#endif
                bins->data[i] = (unsigned char *)cl_malloc(bins->sizes[i]);
            }
            err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, bins->numBytesData, bins->data, &numBinaries2);
            CL_ERROR(err, "clGetProgramInfo");
            numBinaries2 /= sizeof(unsigned char *);
#ifdef CL_DEBUG
            printf("Wrote %u pointers to the binary structure!\n", numBinaries);
#endif
        }
    }
    return bins;
}

char **clLoadSources(char *filename, cl_uint *pNumLines)
{
    FILE *fp = fopen(filename, "r");
    cl_uint numLines = 0;
    char **lines = NULL;
    cl_uint i, lineSize = sizeof(char) * CL_MAX_LINESIZE;
#ifdef CL_DEBUG
    printf("Reading file: %s (%p)\n",filename,fp);
#endif
    if (fp != NULL)
    {
        numLines = (cl_uint)flines(fp);
        *pNumLines = numLines;
        lines = (char **)cl_malloc(numLines * sizeof(char *));
        if (lines != NULL)
        {
            for (i = 0; i < numLines; i++)
            {
                lines[i] = (char *)cl_malloc(lineSize);
                if (lines[i])
                {
                    if (fgets(lines[i], lineSize, fp) == NULL)
                        printf("End of %s\n", filename);
                }
                else
                {
                    for (i-=1; i >= 0; i--)
                        cl_free(lines[i]);
                    cl_free(lines);
                    lines = NULL;
                    break;
                }
            }
        }
        fclose(fp);
        fp = NULL;
    }
    return lines;
}

int cl_precompiled_header(char *filename, cl_kernel_bin_t *bins)
{
    FILE *fo = NULL;
    size_t bw = 0;
    if (filename[0] == '\0')
    {
        printf("ERROR: Filename is empty!\n");
        return 0;
    }
    fo = fopen(filename, "w+");
    if (fo != NULL)
    {
        cl_uint d, b;
        bw += fprintf(fo, "// PRECOMPILED HEADER, DO NOT MODIFY!\n");
        bw += fprintf(fo, "#ifndef _PRECOMPILED_KERNEL_H_\n");
        bw += fprintf(fo, "#define _PRECOMPILED_KERNEL_H_\n");
        bw += fprintf(fo, "#include <clenvironment.h>\n");
        bw += fprintf(fo, "static size_t gKernelBinarySizes["FMT_SIZE_T"] = {\n",bins->numDevices);
        for (d = 0; d < bins->numDevices; d++)
            bw += fprintf(fo, "\t"FMT_SIZE_T",\n", bins->sizes[d]);
        bw += fprintf(fo, "};\n");
        for (d = 0; d < bins->numDevices; d++)
        {
            bw += fprintf(fo, "static cl_byte gKernelBinaryData%09u["FMT_SIZE_T"] = {\n", d, bins->sizes[d]);
            for (b= 0 ; b < bins->sizes[d]; b++)
            {
                bw += fprintf(fo, "\t0x%02x,\n", bins->data[d][b]);
            }
            bw += fprintf(fo, "};\n");
        }
        bw += fprintf(fo, "static cl_byte *gKernelBinaries["FMT_SIZE_T"] = {\n", bins->numDevices);
        for (d = 0; d < bins->numDevices; d++)
        {
            bw += fprintf(fo, "\t(cl_byte *)&gKernelBinaryData%09u,\n", d);
        }
        bw += fprintf(fo, "};\n");
        bw += fprintf(fo, "static cl_kernel_bin_t gKernelBins = { 0x%08x, "FMT_SIZE_T", "FMT_SIZE_T", "FMT_SIZE_T", (size_t *)&gKernelBinarySizes, (cl_byte **)&gKernelBinaries };\n", (uint32_t)bins->deviceTypes, bins->numDevices, bins->numBytesSizes, bins->numBytesData);
        bw += fprintf(fo, "#endif\n");
        bw += fclose(fo);
        fo = NULL;
        printf("Wrote %zu bytes to precompiled header\n", bw);
        return 1;
    } else {
        printf("ERROR: Failed to open %s for writing\n", filename);
        return 0;
    }
}

cl_environment_t *clCreateEnvironmentFromBins(cl_kernel_bin_t *bins,
                                              clnotifier_f notifier,
                                              char *cl_args)
{
    cl_int err = CL_SUCCESS;
    cl_uint i,numPlatforms;
    cl_uint numDevices = 0;
    cl_environment_t *pEnv = cl_malloc_struct(cl_environment_t);
    if (pEnv == NULL)
        return NULL;
    pEnv->numDevices = (cl_uint)bins->numDevices;
    pEnv->devices = cl_malloc_array(cl_device_id, pEnv->numDevices);
    pEnv->queues  = cl_malloc_array(cl_command_queue, pEnv->numDevices);
    if (pEnv->devices == NULL || pEnv->queues == NULL)
    {
        printf("ERROR: Failed allocate data structures for compilation.\n");
        clDeleteEnvironment(pEnv);
        return NULL;
    }
    err = clGetPlatformIDs(1, &pEnv->platform, &numPlatforms);
    CL_ERROR(err, "clGetPlatformIDs");
    if (err != CL_SUCCESS)
    {
        printf("ERROR: Failed to get any platform ID for OpenCL (%d)\n", err);
        clDeleteEnvironment(pEnv);
        return NULL;
    }
    else if (err == CL_SUCCESS && pEnv->platform)
    {
        cl_context_properties props[] = {(cl_context_properties)CL_CONTEXT_PLATFORM,
                                         (cl_context_properties)pEnv->platform,
                                         (cl_context_properties)0};
#ifdef CL_DEBUG
        printf("Platform ID %p\n", pEnv->platform);
#endif
        err = clGetDeviceIDs(pEnv->platform, bins->deviceTypes, pEnv->numDevices, pEnv->devices, &numDevices);
        if (err == CL_SUCCESS)
        {
#ifdef CL_DEBUG
            printf("Returned %u devices! (Asked for %u)\n", numDevices, pEnv->numDevices);
            if (numDevices > pEnv->numDevices)
                printf("Using only the requested number of devices!\n");
            for (i = 0; i < pEnv->numDevices; i++)
                printf("Device ID[%02x] = %p\n", i, pEnv->devices[i]);
#endif
            pEnv->context = clCreateContext(props, pEnv->numDevices, pEnv->devices, NULL, NULL, &err);
            if (pEnv->context)
            {
#ifdef CL_DEBUG
                printf("Created context %p about to create %u queues\n",pEnv->context, pEnv->numDevices);
#endif
                // create the command queues
                for (i = 0; i < pEnv->numDevices; i++)
                {
                    pEnv->queues[i] = clCreateCommandQueue(pEnv->context, pEnv->devices[i], CL_QUEUE_PROFILING_ENABLE, &err);
                    if (pEnv->queues[i] == NULL)
                    {
                        clDeleteEnvironment(pEnv);
                        return NULL;
                    }
                }
                // we aren't loading from sources, so that pointer will remain NULL

                // need to allocate retCodes though
                pEnv->retCodes = cl_malloc_array(cl_int, pEnv->numDevices);
                if (pEnv->retCodes == NULL)
                {
                    clDeleteEnvironment(pEnv);
                    return NULL;
                }

                // create the program from binaries
                pEnv->program = clCreateProgramWithBinary(pEnv->context, pEnv->numDevices, pEnv->devices, bins->sizes, (const unsigned char **)bins->data, pEnv->retCodes, &err);
                if (err != CL_SUCCESS)
                {
                    printf("Failed to create program Error=%d!\n",err);
                    clDeleteEnvironment(pEnv);
                    return NULL;
                }
#ifdef CL_DEBUG
                printf("Created Program %p\n",pEnv->program);
#endif
                // now actually build the program!
                err = clBuildProgram(pEnv->program, pEnv->numDevices, pEnv->devices, cl_args, notifier, NULL);
                if (err != CL_SUCCESS)
                {
                    printf("Error building kernels! Error=%d: ",err);
                    switch (err)
                    {
                        case CL_INVALID_VALUE:
                            printf("Invalid value!\n");
                            break;
                        case CL_INVALID_DEVICE:
                            printf("Invalid device!\n");
                            break;
                        case CL_INVALID_BINARY:
                            printf("Invalid binary!\n");
                            break;
                        case CL_INVALID_PROGRAM:
                            printf("Invalid program\n");
                            break;
                        case CL_INVALID_BUILD_OPTIONS:
                            printf("Invalid build options!\n");
                            break;
                        case CL_BUILD_PROGRAM_FAILURE:
                        {
                            size_t logSize = 10*1024;
                            char *log = (char *)cl_malloc(logSize);
                            printf("Build failure!\n");

                            err = clGetProgramBuildInfo(pEnv->program, pEnv->devices[0], CL_PROGRAM_BUILD_LOG, logSize, log, &logSize);
                            if (err == CL_SUCCESS)
                                printf("Log["FMT_SIZE_T"]: %s\n", logSize,log);
                            else
                                printf("Could not get build log! Error=%d\n",err);
                            cl_free(log);
                            break;
                        }
                        default:
                            printf("Unknown\n");
                            break;
                    }
                    clDeleteEnvironment(pEnv);
                    return NULL;
                }

                printf("No build errors\n");

#ifndef CL_ITERATIVE_QUERY
                // do the initial query to find the exact number of kernels
                err = clCreateKernelsInProgram(pEnv->program, 0, NULL, &pEnv->numKernels);
                if (err != CL_SUCCESS || pEnv->numKernels == 0)
                {
                    printf("ERROR! Failed to retreived the number of compiled kernels! (%u)\n",pEnv->numKernels);
                    clDeleteEnvironment(pEnv);
                    return NULL;
                }
#else
                // now we're going to try to allocate the number of kernels
                // within the program by guessing a number and cl_malloc/freeing
                // until that number is correct (so long as we get the error code
                // that we are looking for).
                pEnv->numKernels = 1; // assume 1 kernel
#endif
                do {
                    // create the kernels
                    pEnv->kernels = cl_malloc_array(cl_kernel,pEnv->numKernels);
                    pEnv->retCodes = cl_malloc_array(cl_int,pEnv->numKernels);
                    if (pEnv->kernels == NULL || pEnv->retCodes == NULL)
                    {
                        printf("ERROR: Failed to allocate memory for kernel objects and return codes.\n");
                        clDeleteEnvironment(pEnv);
                        pEnv = NULL;
                        break;
                    }
                    else
                    {
#ifdef CL_DEBUG
                        printf("Allocated room for %u kernels\n",pEnv->numKernels);
#endif
                        err = clCreateKernelsInProgram(pEnv->program, pEnv->numKernels, pEnv->kernels, &pEnv->numKernels);
#ifdef CL_DEBUG
                        printf("Queried for %u kernels\n", pEnv->numKernels);
                        CL_ERROR(err, "clCreateKernelsInProgram");
#endif
                        if (err == CL_SUCCESS)
                        {
                            cl_uint j;
                            char function_name[CL_MAX_STRING];
                            size_t len;
                            cl_uint numArgs;
#ifdef CL_DEBUG
                            printf("There were %u kernels in the supplied file!\n",pEnv->numKernels);
#endif
                            for (j = 0; j < pEnv->numKernels; j++)
                            {
                                clGetKernelInfo(pEnv->kernels[j], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &len);
                                clGetKernelInfo(pEnv->kernels[j], CL_KERNEL_NUM_ARGS, sizeof(numArgs), &numArgs, &len);
                                printf("SYMBOL: %s :: %u (%p)\n",function_name, numArgs, pEnv->kernels[j]);
                            }
                        }
                        else if (err == CL_INVALID_KERNEL_DEFINITION)
                        {
                            // something is terribly wrong (NVIDIA!!!!)
                            clDeleteEnvironment(pEnv);
                            return NULL;
                        }
                        else // if (err == CL_INVALID_VALUE)
                        {
                            printf("Only %u values were filled in!\n", pEnv->numKernels);
                            if (pEnv->kernels[0] != NULL)
                                printf("WARNING! Some Kernel Values were filled in!\n");
                            cl_free(pEnv->kernels);
                            cl_free(pEnv->retCodes);
                            pEnv->numKernels++;
                        }
                    }
                } while (err != CL_SUCCESS);
            }
        }
    }
    return pEnv;
}


cl_environment_t *clCreateEnvironment(char *filename,
                                      cl_uint dev_types,
                                      cl_uint numDevices,
                                      clnotifier_f notifier,
                                      char *cl_args)
{
    cl_int err;
    cl_uint i;
    cl_uint numPlatforms = 0;

    cl_environment_t *pEnv = cl_malloc_struct(cl_environment_t);
    if (pEnv == NULL)
        return NULL;
    pEnv->devices = cl_malloc_array(cl_device_id,numDevices);
    pEnv->queues  = cl_malloc_array(cl_command_queue,numDevices);
    if (pEnv->devices == NULL || pEnv->queues == NULL)
    {
        clDeleteEnvironment(pEnv);
        return NULL;
    }
    printf("Environment: %p\n", pEnv);
    err = clGetPlatformIDs(1, &pEnv->platform, &numPlatforms);
    if (err == CL_SUCCESS && pEnv->platform)
    {
        cl_context_properties props[] = {(cl_context_properties)CL_CONTEXT_PLATFORM,
                                         (cl_context_properties)pEnv->platform,
                                         (cl_context_properties)0};
#ifdef CL_DEBUG
        printf("Platform ID %p\n", pEnv->platform);
        clPrintAllPlatformInfo(pEnv->platform);
#endif
        pEnv->numDevices = numDevices;
#ifdef CL_DEBUG
        printf("Requesting %u devices\n",numDevices);
#endif
        err = clGetDeviceIDs(pEnv->platform, dev_types, pEnv->numDevices, pEnv->devices, &numDevices);
        if (err == CL_SUCCESS)
        {
#ifdef CL_DEBUG
            printf("Returned %u devices! (Asked for %u)\n", numDevices, pEnv->numDevices);
            if (numDevices > pEnv->numDevices)
                printf("Using only the requested number of devices!\n");
            for (i = 0; i < pEnv->numDevices; i++)
                printf("Device ID[%02x] = %p\n", i, pEnv->devices[i]);
#endif
            pEnv->context = clCreateContext(props, pEnv->numDevices, pEnv->devices, NULL, NULL, &err);
            if (pEnv->context)
            {
#ifdef CL_DEBUG
                printf("Created context %p about to create %u queues\n",pEnv->context, pEnv->numDevices);
#endif
                // create the command queues
                for (i = 0; i < pEnv->numDevices; i++)
                {
                    pEnv->queues[i] = clCreateCommandQueue(pEnv->context, pEnv->devices[i], CL_QUEUE_PROFILING_ENABLE, &err);
                    if (pEnv->queues[i] == NULL)
                    {
                        clDeleteEnvironment(pEnv);
                        return NULL;
                    }
                }

                // load source from a file
                pEnv->sources = clLoadSources(filename, &pEnv->numLines);
                pEnv->lengths = cl_malloc_array(size_t,pEnv->numLines);
                if (pEnv->lengths && pEnv->sources)
                {
                    for (i = 0; i < pEnv->numLines; i++)
                        pEnv->lengths[i] = strlen(pEnv->sources[i]);
#ifdef CL_DEBUG
                    for (i = 0; i < pEnv->numLines; i++)
                        printf("[%4u] %s",pEnv->lengths[i],pEnv->sources[i]);
#endif
                }
                else
                {
                    clDeleteEnvironment(pEnv);
                    return NULL;
                }

                // create the program from source
                pEnv->program = clCreateProgramWithSource(pEnv->context, pEnv->numLines, (const char **)pEnv->sources, pEnv->lengths, &err);
                if (err != CL_SUCCESS)
                {
                    printf("Failed to create program Error=%d!\n",err);
                    clDeleteEnvironment(pEnv);
                    return NULL;
                }
#ifdef CL_DEBUG
                printf("Created Program %p\n",pEnv->program);
#endif
                // now actually build the program!
                err = clBuildProgram(pEnv->program, pEnv->numDevices, pEnv->devices, cl_args, notifier, NULL);
                if (err != CL_SUCCESS)
                {
                    CL_ERROR(err, "clBuildProgram");
                    switch (err)
                    {
                        case CL_INVALID_VALUE:
                            printf("Invalid value!\n");
                            break;
                        case CL_INVALID_DEVICE:
                            printf("Invalid device!\n");
                            break;
                        case CL_INVALID_BINARY:
                            printf("Invalid binary!\n");
                            break;
                        case CL_INVALID_PROGRAM:
                            printf("Invalid program\n");
                            break;
                        case CL_INVALID_BUILD_OPTIONS:
                            printf("Invalid build options!\n");
                            break;
                        case CL_BUILD_PROGRAM_FAILURE:
                        {
                            size_t logSize = 10*1024;
                            char *log = (char *)cl_malloc(logSize);
                            printf("Build failure!\n");

                            err = clGetProgramBuildInfo(pEnv->program, pEnv->devices[0], CL_PROGRAM_BUILD_LOG, logSize, log, &logSize);
                            if (err == CL_SUCCESS)
                                printf("Log["FMT_SIZE_T"]: %s\n", logSize,log);
                            else
                                printf("Could not get build log! Error=%d\n",err);
                            cl_free(log);
                            break;
                        }
                        default:
                            printf("Unknown\n");
                            break;
                    }
                    clDeleteEnvironment(pEnv);
                    return NULL;
                }

                printf("No build errors\n");

#ifndef CL_ITERATIVE_QUERY
                // do the initial query to find the exact number of kernels
                err = clCreateKernelsInProgram(pEnv->program, 0, NULL, &pEnv->numKernels);
                CL_ERROR(err, "clCreateKernelsInProgram");
                if (err != CL_SUCCESS || pEnv->numKernels == 0)
                {
                    printf("ERROR! Failed to retreived the number of compiled kernels! (%u)\n",pEnv->numKernels);
                    clDeleteEnvironment(pEnv);
                    return NULL;
                }
#else
                // now we're going to try to allocate the number of kernels
                // within the program by guessing a number and cl_malloc/freeing
                // until that number is correct (so long as we get the error code
                // that we are looking for).
                pEnv->numKernels = 1; // assume 1 kernel to start with
#endif

                do {
                    // create the kernels
                    pEnv->kernels = cl_malloc_array(cl_kernel,pEnv->numKernels);
                    pEnv->retCodes = cl_malloc_array(cl_int,pEnv->numKernels);
                    if (pEnv->kernels == NULL || pEnv->retCodes == NULL)
                    {
                        printf("ERROR: Failed to allocate memory for kernel objects and return codes.\n");
                        clDeleteEnvironment(pEnv);
                        pEnv = NULL;
                        break;
                    }
                    else
                    {
#ifdef CL_DEBUG
                        printf("Allocated room for %u kernels\n",pEnv->numKernels);
#endif
                        err = clCreateKernelsInProgram(pEnv->program, pEnv->numKernels, pEnv->kernels, &pEnv->numKernels);
#ifdef CL_DEBUG
                        printf("Queried for %u kernels\n", pEnv->numKernels);
                        CL_ERROR(err, "clCreateKernelsInProgram");
#endif
                        if (err == CL_SUCCESS)
                        {
                            cl_uint j;
                            char function_name[CL_MAX_STRING];
                            size_t len;
                            cl_uint numArgs;
#ifdef CL_DEBUG
                            printf("There were %u kernels in the supplied file!\n",pEnv->numKernels);
#endif
                            for (j = 0; j < pEnv->numKernels; j++)
                            {
                                clGetKernelInfo(pEnv->kernels[j], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &len);
                                clGetKernelInfo(pEnv->kernels[j], CL_KERNEL_NUM_ARGS, sizeof(numArgs), &numArgs, &len);
                                printf("SYMBOL: %s :: %u (%p)\n",function_name, numArgs, pEnv->kernels[j]);
                            }
                        }
                        else if (err == CL_INVALID_KERNEL_DEFINITION)
                        {
                            // something is terribly wrong (NVIDIA!!!!)
                            clDeleteEnvironment(pEnv);
                            return NULL;
                        }
                        else // if (err == CL_INVALID_VALUE)
                        {
                            printf("Only %u kernels were returned!\n", pEnv->numKernels);
                            if (pEnv->kernels[0] != NULL)
                                printf("WARNING! Some Kernel Values were filled in!\n");
                            cl_free(pEnv->kernels);
                            cl_free(pEnv->retCodes);
                            pEnv->numKernels++;
                        }
                    }
                } while (err != CL_SUCCESS);
            }
        }
        else
        {
            printf("ERROR: Failed to acquire devices of type: %u\n", dev_types);
        }
    }
    return pEnv;
}

cl_kernel clGetKernelByName(cl_environment_t *pEnv, char *func_name)
{
    cl_uint i = 0;
    char name[CL_MAX_STRING];
    size_t len = CL_MAX_STRING * sizeof(cl_char);
#ifdef CL_DEBUG
    printf("Looking for kernel %s out of %u\n", func_name, pEnv->numKernels);
#endif
    for (i = 0; i < pEnv->numKernels; i++)
    {
        cl_int err = clGetKernelInfo(pEnv->kernels[i], CL_KERNEL_FUNCTION_NAME, sizeof(name), name, &len);
#ifdef CL_DEBUG
        printf("Found SYMBOL[%06u] (%lu) %s\n", i, len, name);
#endif
        if (err == CL_SUCCESS && strncmp(name, func_name, CL_MAX_STRING) == 0)
        {
            return pEnv->kernels[i];
        }
    }
    return NULL;
}

cl_int clCallKernel(cl_environment_t *pEnv, cl_kernel_call_t *pCall, cl_uint numCalls)
{
    cl_int err = CL_SUCCESS;
    cl_uint i = 0, j = 0, k = 0;

    if (pEnv == NULL || pEnv->kernels == NULL)
        return CL_INVALID_ARG_VALUE;

    for (k = 0; k < numCalls; k++)
    {
        cl_kernel kernel = clGetKernelByName(pEnv, pCall[k].kernel_name);
        if (kernel == NULL)
            return CL_INVALID_KERNEL_NAME;

        for (j = 0; j < pCall[k].numParams; j++)
        {
            if (pCall[k].params[j].type == CL_KPARAM_BUFFER_0D)
            {
                pCall[k].params[j].mem = NULL;
            }
            else if (pCall[k].params[j].type == CL_KPARAM_BUFFER_1D)
            {
                pCall[k].params[j].mem = clCreateBuffer(pEnv->context, pCall[k].params[j].flags, pCall[k].params[j].numBytes, NULL, &err);
        #ifdef CL_DEBUG
                printf("Create Buffer from %p for "FMT_SIZE_T" bytes with 0x%08x flags (mem=%p, err=%d)\n",
                        pCall[k].params[j].data,
                        pCall[k].params[j].numBytes,
                        (cl_uint)pCall[k].params[j].flags,
                        pCall[k].params[j].mem,
                        err);
        #endif
                cl_assert((err == CL_SUCCESS), printf("Failed to create 1D cl_mem object of "FMT_SIZE_T" bytes!\n", pCall[k].params[j].numBytes));
            }
            else if (pCall[k].params[j].type == CL_KPARAM_BUFFER_2D)
            {
                cl_nd_buffer_t *pBuf = (cl_nd_buffer_t *)pCall[k].params[j].data;
                cl_image_format *pIf = &pBuf->format;
                pCall[k].params[j].mem = clCreateImage2D(pEnv->context, pCall[k].params[j].flags, pIf, pBuf->dim[X_DIM], pBuf->dim[Y_DIM], pBuf->strides[Y_DIM], NULL, &err);
        #ifdef CL_DEBUG
                printf("Create Image2D from %p for "FMT_SIZE_T" bytes with 0x%08x flags (mem=%p, err=%d)\n",
                        pCall[k].params[j].data,
                        pCall[k].params[j].numBytes,
                        (cl_uint)pCall[k].params[j].flags,
                        pCall[k].params[j].mem,
                        err);
        #endif
                cl_assert((err == CL_SUCCESS), printf("Failed to create 2D cl_mem object of "FMT_SIZE_T" bytes!\n",pCall[k].params[j].numBytes));
            }
            else if (pCall[k].params[j].type == CL_KPARAM_BUFFER_3D)
            {
                cl_nd_buffer_t *pBuf = (cl_nd_buffer_t *)pCall[k].params[j].data;
                cl_image_format *pIf = &pBuf->format;
                pCall[k].params[j].mem = clCreateImage3D(pEnv->context, pCall[k].params[j].flags, pIf, pBuf->dim[X_DIM], pBuf->dim[Y_DIM], pBuf->dim[Z_DIM], pBuf->strides[Y_DIM], pBuf->strides[Z_DIM], NULL, &err);
        #ifdef CL_DEBUG
                printf("Create Image3D from %p for %lu bytes with 0x%08x flags (mem=%p, err=%d)\n",
                        pCall[k].params[j].data,
                        pCall[k].params[j].numBytes,
                        (cl_uint)pCall[k].params[j].flags,
                        pCall[k].params[j].mem,
                        err);
        #endif
                cl_assert((err == CL_SUCCESS), printf("Failed to create cl_mem object!\n"));
            }
        }

        // enqueue the writes
        for (i = 0; i < pEnv->numDevices; i++)
        {
            for (j = 0; j < pCall[k].numParams; j++)
            {
                if (pCall[k].params[j].flags == CL_MEM_READ_ONLY ||
                    pCall[k].params[j].flags == CL_MEM_READ_WRITE)
                {
    #ifdef CL_DEBUG
                    printf("Copying mem %p from ptr %p for %lu bytes\n", pCall[k].params[j].mem, pCall[k].params[j].data, pCall[k].params[j].numBytes);
    #endif
                    if (pCall[k].params[j].type == CL_KPARAM_BUFFER_1D)
                        err = clEnqueueWriteBuffer(pEnv->queues[i], pCall[k].params[j].mem, CL_TRUE, 0, pCall[k].params[j].numBytes, pCall[k].params[j].data, 0, NULL, &pCall[k].params[j].event);
                    else if (pCall[k].params[j].type == CL_KPARAM_BUFFER_2D || pCall[k].params[j].type == CL_KPARAM_BUFFER_3D)
                    {
                        cl_nd_buffer_t *pBuf = (cl_nd_buffer_t *)pCall[k].params[j].data;
                        err = clEnqueueWriteBuffer(pEnv->queues[i], pCall[k].params[j].mem, CL_TRUE, 0, pBuf->size, pBuf->data[0], 0, NULL, &pCall[k].params[j].event);
                    }
                    cl_assert((err == CL_SUCCESS),printf("ERROR: Write Enqueue Error = %d\n",err));
                }
            }
        }

        // finish
        for (i = 0; i < pEnv->numDevices; i++)
            clFinish(pEnv->queues[i]);

        // enqueue the kernel
        for (i = 0; i < pEnv->numDevices; i++) {
            for (j = 0; j < pCall[k].numParams; j++) {
    #ifdef CL_DEBUG
                printf("ARG[%2u] mem %p (%lu)\n", j, pCall[k].params[j].mem, sizeof(cl_mem));
    #endif
                if (pCall[k].params[j].type == CL_KPARAM_BUFFER_0D)
                    err = clSetKernelArg(kernel, j, pCall[k].params[j].numBytes, pCall[k].params[j].data);
                else if (pCall[k].params[j].type == CL_KPARAM_LOCAL)
                    err = clSetKernelArg(kernel, j, pCall[k].params[j].numBytes, NULL);
                else
                    err = clSetKernelArg(kernel, j, sizeof(cl_mem), &pCall[k].params[j].mem);
                cl_assert((err == CL_SUCCESS),printf("ERROR: Kernel Arg %d is wrong (Error=%d)\n", j, err));
            }
            err = clEnqueueNDRangeKernel(pEnv->queues[i],
                                        kernel,
                                        pCall[k].numDim,
                                        NULL, //pCall[k].global_work_offset,
                                        pCall[k].global_work_size,
                                        pCall[k].local_work_size,
                                        0, NULL, &pCall[k].event);
            cl_assert((err == CL_SUCCESS),printf("ERROR: Work Queue Error = %d\n",err));
        }

        // finish
        for (i = 0; i<pEnv->numDevices; i++)
            clFinish(pEnv->queues[i]);

        err = clGetEventProfilingInfo(pCall[k].event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &pCall[k].start, NULL);
        cl_assert(err == CL_SUCCESS,printf("Error=%d\n",err));
        err = clGetEventProfilingInfo(pCall[k].event, CL_PROFILING_COMMAND_END,   sizeof(cl_ulong), &pCall[k].stop, NULL);
        cl_assert(err == CL_SUCCESS,printf("Error=%d\n",err));
    #ifdef CL_DEBUG
        printf("Executed kernel %s!\n",pCall[k].kernel_name);
    #endif

        // read the result memory
        for (i = 0; i<pEnv->numDevices; i++)
        {
            for (j = 0; j < pCall[k].numParams; j++)
            {
                if (pCall[k].params[j].flags == CL_MEM_WRITE_ONLY ||
                    pCall[k].params[j].flags == CL_MEM_READ_WRITE)
                {
                    err = CL_SUCCESS;
                    if (pCall[k].params[j].type == CL_KPARAM_BUFFER_1D)
                        err = clEnqueueReadBuffer(pEnv->queues[i], pCall[k].params[j].mem, CL_TRUE, 0, pCall[k].params[j].numBytes, pCall[k].params[j].data, 0, NULL, NULL);
                    else if (pCall[k].params[j].type == CL_KPARAM_BUFFER_2D || pCall[k].params[j].type == CL_KPARAM_BUFFER_3D)
                    {
                        cl_nd_buffer_t *pBuf = (cl_nd_buffer_t *)pCall[k].params[j].data;
                        err = clEnqueueReadBuffer(pEnv->queues[i], pCall[k].params[j].mem, CL_TRUE, 0, pBuf->size, pBuf->data[0], 0, NULL, &pCall[k].params[j].event);
                    }
                    cl_assert((err == CL_SUCCESS),printf("ERROR: Read Enqueue Error=%d\n",err));
                }
            }
        }
        // finish
        for (i = 0; i<pEnv->numDevices; i++)
            clFinish(pEnv->queues[i]);

        for (i = 0; i < pEnv->numDevices; i++)
            clPrintAllKernelWorkInfo(kernel, pEnv->devices[i]);

        for (j = 0; j < pCall[k].numParams; j++)
        {
            clReleaseMemObject(pCall[k].params[j].mem);
        }
        pCall[k].err = err;
        if (err != CL_SUCCESS)
            break;
    }
    return err;
}

cl_uint clGetTypeFromString(char *typestring)
{
    if (strncmp(typestring, "all",4) == 0)
        return CL_DEVICE_TYPE_ALL;
    else if (strncmp(typestring,"gpu",4) == 0)
        return CL_DEVICE_TYPE_GPU;
    else if (strncmp(typestring,"cpu",4) == 0)
        return CL_DEVICE_TYPE_CPU;
    else if (strncmp(typestring,"acc",4) == 0 || strncmp(typestring,"accelerator",12) == 0)
        return CL_DEVICE_TYPE_ACCELERATOR;
    else
        return CL_DEVICE_TYPE_DEFAULT;
}

