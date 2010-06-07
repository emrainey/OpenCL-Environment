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

void cl_free(void *ptr)
{
#ifdef CL_DEBUG
    printf("Freeing %p\n",ptr);
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
    printf("Malloc'd %p for %lu bytes\n",ptr,numBytes);
#endif
    if (ptr)
        memset(ptr, 0, numBytes);
    return ptr;
}

#define cl_malloc_struct(type)  (type *)cl_malloc(sizeof(type))
#define cl_malloc_array(type, num) (type *)cl_malloc(sizeof(type) * num)

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
        bin = cl_malloc(numBytes);
        if (bin)
        {
            fread(bin, 1, numBytes, fp);
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
        cl_uint offset = 0;
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
    size_t numDevices = 1;
    cl_uint i, offset = 0;
    cl_kernel_bin_t *bins = NULL;

    // minimum size
    if (numBytes <= (2* sizeof(size_t)))
        return NULL;

    memcpy(&numDevices, &bin[offset], sizeof(size_t));
    offset += sizeof(size_t);
    printf("There are %lu device kernels in the flat binary\n", numDevices);
    bins = cl_create_kernel_bin(numDevices);
    if (bins)
    {
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
    if (filename[0] == '\0')
        return;
    cl_byte *bin = cl_serialize_kernels(bins, &numBytes);
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


    err = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof(size_t), &numDevices, NULL);
    if (err == CL_SUCCESS)
    {
#ifdef CL_DEBUG
        printf("There are %lu devices associated with this program %p.\n", numDevices, program);
#endif
        // get the devices
        clGetProgramInfo(program, CL_PROGRAM_DEVICES, sizeof(devices), devices, NULL);

        // did it compile successfully?
        clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_STATUS, sizeof(cl_int), &err, NULL);
        if (err != CL_SUCCESS)
        {
            printf("ERROR: Build was not successful! (Error=%d)\n",err);
            return NULL;
        }

        bins = cl_create_kernel_bin(numDevices);

        err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, bins->numBytesSizes, bins->sizes, &numBinaries);
        if (err != CL_SUCCESS)
        {
            printf("Error: When requesting the number of binary outputs, error %d was returned\n", err);
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
            cl_assert(err == CL_SUCCESS,);
            numBinaries2 /= sizeof(unsigned char *);
        }
    }
    return bins;
}



char **clLoadSources(char *filename, size_t *pNumLines)
{
    FILE *fp = fopen(filename, "r");
    cl_uint i, numLines = 0;
    char **lines = NULL;
    cl_uint lineSize = sizeof(char) * CL_MAX_LINESIZE;
#ifdef CL_DEBUG
    printf("Reading file: %s (%p)\n",filename,fp);
#endif
    if (fp != NULL)
    {
        numLines = flines(fp);
        *pNumLines = numLines;
        lines = (char **)cl_malloc(numLines * sizeof(char *));
        if (lines != NULL)
        {
            for (i = 0; i < numLines; i++)
            {
                lines[i] = (char *)cl_malloc(lineSize);
                if (lines[i])
                {
                    fgets(lines[i], lineSize, fp);
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
    }
    return lines;
}

void cl_precompiled_header(char *filename, cl_kernel_bin_t *bins)
{
    FILE *fo = NULL;
    if (filename[0] == '\0')
        return;
    fo = fopen(filename, "w+");
    if (fo != NULL)
    {
        cl_uint d, b;
        fprintf(fo, "// PRECOMPILED HEADER, DO NOT MODIFY!\n");
        fprintf(fo, "#include <clenvironment.h>\n");
        fprintf(fo, "static size_t gKernelBinarySizes[%lu] = {\n",bins->numDevices);
        for (d = 0; d < bins->numDevices; d++)
            fprintf(fo, "\t%lu,\n", bins->sizes[d]);
        fprintf(fo, "};\n");
        for (d = 0; d < bins->numDevices; d++)
        {
            fprintf(fo, "static cl_byte gKernelBinaryData%09u[%lu] = {\n", d, bins->sizes[d]);
            for (b= 0 ; b < bins->sizes[d]; b++)
            {
                fprintf(fo, "\t0x%02x,\n", bins->data[d][b]);
            }
            fprintf(fo, "};\n");
        }
        fprintf(fo, "static cl_byte *gKernelBinaries[%lu] = {\n", bins->numDevices);
        for (d = 0; d < bins->numDevices; d++)
        {
            fprintf(fo, "\t(cl_byte *)&gKernelBinaryData%09u,\n", d);
        }
        fprintf(fo, "};\n");
        fprintf(fo, "static cl_kernel_bin_t gKernelBins = { %luL, %luL, %luL, (size_t *)&gKernelBinarySizes, (cl_byte **)&gKernelBinaries };\n", bins->numDevices, bins->numBytesSizes, bins->numBytesData);
        fclose(fo);
    }
}

cl_environment_t *clCreateEnvironmentFromBins(cl_kernel_bin_t *bins,
                                              clnotifier_f notifier,
                                              char *cl_args)
{
    cl_int err = CL_SUCCESS;
    cl_uint i,numPlatforms;
    cl_environment_t *pEnv = cl_malloc_struct(cl_environment_t);
    if (pEnv == NULL)
        return NULL;
    pEnv->numDevices = bins->numDevices;
    pEnv->devices = cl_malloc_array(cl_device_id, pEnv->numDevices);
    pEnv->queues  = cl_malloc_array(cl_command_queue, pEnv->numDevices);
    if (pEnv->devices == NULL || pEnv->queues == NULL)
    {
        clDeleteEnvironment(pEnv);
        return NULL;
    }
    err = clGetPlatformIDs(1, &pEnv->platform, &numPlatforms);
    if (err != CL_SUCCESS)
    {
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
        err = clGetDeviceIDs(pEnv->platform, CL_DEVICE_TYPE_DEFAULT, pEnv->numDevices, pEnv->devices, &pEnv->numDevices);
        if (err == CL_SUCCESS)
        {
#ifdef CL_DEBUG
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
                    pEnv->queues[i] = clCreateCommandQueue(pEnv->context, pEnv->devices[i], 0, &err);
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
                                printf("Log[%04lu]: %s\n", logSize,log);
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

                // now we're going to try to allocate the number of kernels
                // within the program by guessing a number and cl_malloc/freeing
                // until that number is correct (so long as we get the error code
                // that we are looking for).

                // do the initial query to find the exact number of kernels
                err = clCreateKernelsInProgram(pEnv->program, 0, NULL, &pEnv->numKernels);
#ifdef CL_DEBUG
                printf("Querying for num kernels = %u, Error = %d\n", pEnv->numKernels, err);
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
                        else if (err == CL_INVALID_VALUE)
                        {
                            if (pEnv->kernels[0] != NULL)
                                printf("WARNING! Some Kernel Values were filled in!\n");
                            cl_free(pEnv->kernels);
                            cl_free(pEnv->retCodes);
                        }
                    }
                } while (err == CL_INVALID_VALUE);
            }
        }
    }
    return pEnv;
}


cl_environment_t *clCreateEnvironment(char *filename,
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
    err = clGetPlatformIDs(1, &pEnv->platform, &numPlatforms);
    if (err == CL_SUCCESS && pEnv->platform)
    {
        cl_context_properties props[] = {(cl_context_properties)CL_CONTEXT_PLATFORM,
                                         (cl_context_properties)pEnv->platform,
                                         (cl_context_properties)0};
#ifdef CL_DEBUG
        printf("Platform ID %p\n", pEnv->platform);
#endif
        pEnv->numDevices = numDevices;
        err = clGetDeviceIDs(pEnv->platform, CL_DEVICE_TYPE_DEFAULT, pEnv->numDevices, pEnv->devices, &pEnv->numDevices);
        if (err == CL_SUCCESS)
        {
#ifdef CL_DEBUG
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
                    pEnv->queues[i] = clCreateCommandQueue(pEnv->context, pEnv->devices[i], 0, &err);
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
                                printf("Log[%04lu]: %s\n", logSize,log);
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

                // now we're going to try to allocate the number of kernels
                // within the program by guessing a number and cl_malloc/freeing
                // until that number is correct (so long as we get the error code
                // that we are looking for).

                // do the initial query to find the exact number of kernels
                err = clCreateKernelsInProgram(pEnv->program, 0, NULL, &pEnv->numKernels);
#ifdef CL_DEBUG
                printf("Querying for num kernels = %u, Error = %d\n", pEnv->numKernels, err);
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
                        else if (err == CL_INVALID_VALUE)
                        {
                            if (pEnv->kernels[0] != NULL)
                                printf("WARNING! Some Kernel Values were filled in!\n");
                            cl_free(pEnv->kernels);
                            cl_free(pEnv->retCodes);
                        }
                    }
                } while (err == CL_INVALID_VALUE);
            }
        }
    }
    return pEnv;
}

cl_kernel clGetKernelByName(cl_environment_t *pEnv, char *func_name)
{
    cl_uint i = 0;
    char name[CL_MAX_STRING];
    size_t len = CL_MAX_STRING * sizeof(cl_char);
    for (i = 0; i < pEnv->numKernels; i++)
    {
        cl_int err = clGetKernelInfo(pEnv->kernels[i], CL_KERNEL_FUNCTION_NAME, len, name, &len);
        if (err == CL_SUCCESS && strncmp(name, func_name, CL_MAX_STRING) == 0)
        {
            return pEnv->kernels[i];
        }
    }
    return NULL;
}

cl_int clCallKernel(cl_environment_t *pEnv, cl_kernel_call_t *pCall)
{
    cl_int err = CL_SUCCESS;
    cl_uint i = 0, j = 0;

    cl_kernel kernel = clGetKernelByName(pEnv, pCall->kernel_name);
    if (kernel == NULL)
        return CL_INVALID_KERNEL_NAME;

    for (j = 0; j < pCall->numParams; j++)
    {
        pCall->params[j].mem = clCreateBuffer(pEnv->context, pCall->params[j].flags, pCall->params[j].numBytes, NULL, &err);
#ifdef CL_DEBUG
        printf("Create Buffer from %p for %lu bytes with 0x%08x flags (mem=%p, err=%d)\n",
                pCall->params[j].data,
                pCall->params[j].numBytes,
                (cl_uint)pCall->params[j].flags,
                pCall->params[j].mem,
                err);
#endif
        cl_assert((err == CL_SUCCESS), printf("Failed to create cl_mem object!\n"));
    }

    // enqueue the writes
    for (i = 0; i < pEnv->numDevices; i++)
    {
        for (j = 0; j < pCall->numParams; j++)
        {
            if (pCall->params[j].flags == CL_MEM_READ_ONLY ||
                pCall->params[j].flags == CL_MEM_READ_WRITE)
            {
#ifdef CL_DEBUG
                printf("Copying mem %p from ptr %p for %lu bytes\n", pCall->params[j].mem, pCall->params[j].data, pCall->params[j].numBytes);
#endif
                err = clEnqueueWriteBuffer(pEnv->queues[i], pCall->params[j].mem, CL_TRUE, 0, pCall->params[j].numBytes, pCall->params[j].data, 0, NULL, NULL);
                cl_assert((err == CL_SUCCESS),printf("ERROR: Write Enqueue Error = %d\n",err));
            }
        }
    }

    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    // enqueue the kernel
    for (i = 0; i < pEnv->numDevices; i++) {
        for (j = 0; j < pCall->numParams; j++) {
#ifdef CL_DEBUG
            printf("ARG[%2u] mem %p (%lu)\n", j, pCall->params[j].mem, sizeof(cl_mem));
#endif
            err = clSetKernelArg(kernel, j, sizeof(cl_mem), &pCall->params[j].mem);
            cl_assert((err == CL_SUCCESS),printf("ERROR: Kernel Arg %d is wrong (Error=%d)\n", j, err));
        }
        err = clEnqueueNDRangeKernel(pEnv->queues[i], kernel, pCall->numDim, NULL, pCall->global_work_size, NULL, 0, NULL, NULL);
        cl_assert((err == CL_SUCCESS),printf("ERROR: Work Queue Error = %d\n",err));
    }

    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);
#ifdef CL_DEBUG
    printf("Executed kernel %s!\n",pCall->kernel_name);
#endif
    // read the result memory
    for (i = 0; i<pEnv->numDevices; i++) {
        for (j = 0; j < pCall->numParams; j++)
        {
            if (pCall->params[j].flags == CL_MEM_WRITE_ONLY ||
                pCall->params[j].flags == CL_MEM_READ_WRITE)
            {
                err = clEnqueueReadBuffer(pEnv->queues[i], pCall->params[j].mem, CL_TRUE, 0, pCall->params[j].numBytes, pCall->params[j].data, 0, NULL, NULL);
                cl_assert((err == CL_SUCCESS),printf("ERROR: Read Enqueue Error=%d\n",err));
            }
        }
    }
    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    for (j = 0; j < pCall->numParams; j++)
    {
        clReleaseMemObject(pCall->params[j].mem);
    }
    pCall->err = err;
    return err;
}

