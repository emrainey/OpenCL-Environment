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
#include <climgfilter.h>
#include <kernel_imgfilter.h>

#define CL_OPTIONS  ("-I/Users/emrainey/Source/OpenCL/include")

void notify(cl_program program, void *arg)
{
    printf("Program %p Arg %p\n",program, arg);
}

cl_int imgfilter(cl_environment_t *pEnv, cl_Image_t *pImage, cl_int *a, cl_uint a_len, cl_Image_t *pOut)
{
    cl_mem a_mem;
    cl_mem image_mem;
    cl_mem out_mem;
    cl_image_format image_format = {CL_LUMINANCE, CL_UNSIGNED_INT8};
    cl_int err = CL_SUCCESS;
    cl_uint i = 0;

    // create memory arrays
    a_mem = clCreateBuffer(pEnv->context, CL_MEM_READ_ONLY, a_len * sizeof(a[0]), a, &err);
    cl_assert(err == CL_SUCCESS,);
    image_mem = clCreateImage2D(pEnv->context,
                                CL_MEM_READ_ONLY,
                                &image_format,
                                pImage->dimensions[X_DIM],
                                pImage->dimensions[Y_DIM],
                                pImage->strides[Y_DIM],
                                NULL,
                                &err);
    cl_assert(err == CL_SUCCESS,);
    out_mem = clCreateImage2D(pEnv->context,
                              CL_MEM_WRITE_ONLY,
                              &image_format,
                              pImage->dimensions[X_DIM],
                              pImage->dimensions[Y_DIM],
                              pImage->strides[Y_DIM],
                              NULL,
                              &err);
    cl_assert(err == CL_SUCCESS,);

    // enqueue the writes
    for (i = 0; i < pEnv->numDevices; i++) {
        size_t origin[3] = {0,0,0};
        err = 0;
        err |= clEnqueueWriteBuffer(pEnv->queues[i], a_mem, CL_TRUE, 0, a_len * sizeof(a[0]), a, 0, NULL, NULL);
        err |= clEnqueueWriteImage(pEnv->queues[i], image_mem, CL_TRUE, origin, pImage->dimensions, pImage->strides[Y_DIM], 0, pImage->data[0],  0, NULL, NULL);
        printf("Write Enqueue Error = %d\n",err);
    }
    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    // enqueue the kernel
    for (i = 0; i < pEnv->numDevices; i++) {
        clSetKernelArg(pEnv->kernels[0], 0, sizeof(cl_mem), &image_mem);
        clSetKernelArg(pEnv->kernels[0], 1, sizeof(cl_mem), &a_mem);
        clSetKernelArg(pEnv->kernels[0], 2, sizeof(cl_mem), &out_mem);

        err = 0;
        err |= clEnqueueNDRangeKernel(pEnv->queues[i], pEnv->kernels[0], 2, NULL, pImage->dimensions, NULL, 0, NULL, NULL);
        printf("Work Queue Error = %d\n",err);
    }

    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);

    // enqueue the reads
    for (i = 0; i<pEnv->numDevices; i++)
    {
        size_t origin[3] = {0,0,0};
        err = 0;
        err |= clEnqueueReadImage(pEnv->queues[i], out_mem, CL_TRUE, origin, pOut->dimensions, pOut->strides[Y_DIM], 0, pOut->data[0], 0, NULL, NULL);
        printf("Read Enqueue Error = %d\n",err);
    }

    // finish
    for (i = 0; i<pEnv->numDevices; i++)
        clFinish(pEnv->queues[i]);


    // release memory objects
    clReleaseMemObject(a_mem);
    clReleaseMemObject(image_mem);

    return err;

}

int main(int argc, char *argv[])
{
    if (argc > 5)
    {
        FILE *fi = NULL;
        FILE *fo = NULL;
        size_t width = atoi(argv[2]);
        size_t height = atoi(argv[3]);
        cl_int err = CL_SUCCESS;
        cl_environment_t *pEnv = clCreateEnvironment(KDIR"kernel_imgfilter.cl",1,notify, CL_OPTIONS);

        fi = fopen(argv[1], "rb");
        fo = fopen(argv[4], "wb+");
        if (fi && fo)
        {
            cl_uint sobel[2][3][3] = { { {1,1,1}, {0,0,0}, {-1,-1,-1} }, { {-1,0,1}, {-1,0,1}, {-1,0,1} } };
            cl_Image_t input_image;
            cl_Image_t inter_image;
            cl_Image_t output_image;
            size_t numBytes = width * height * sizeof(unsigned char);
            clock_t c_start, c_diff;
            input_image.data[0] = (cl_uchar *)malloc(numBytes);
            inter_image.data[0] = (cl_uchar *)malloc(numBytes);
            output_image.data[0] = (cl_uchar *)malloc(numBytes);

            do {
                numBytes = fread(input_image.data[0], 1, numBytes, fi);
                if (numBytes == 0)
                    break;
                c_start = clock();
                err |= imgfilter(pEnv, &input_image, (cl_int *)&sobel[0], sizeof(cl_uint) * 9, &inter_image);
                err |= imgfilter(pEnv, &inter_image, (cl_int *)&sobel[1], sizeof(cl_uint) * 9, &output_image);
                c_diff = clock() - c_start;
                printf("Sobel took %lu ticks\n", c_diff);
                numBytes = fwrite(output_image.data[0], 1, numBytes, fo);
            } while (err == CL_SUCCESS);

            free(input_image.data[0]);
            free(inter_image.data[0]);
            free(output_image.data[0]);

            fclose(fi);
            fclose(fo);
        }
        clDeleteEnvironment(pEnv);
    }
    else
    {
        printf("%s <filename> <width> <height> <outfile>\n",argv[0]);
    }
    return 0;
}