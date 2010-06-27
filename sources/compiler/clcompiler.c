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

typedef enum _cl_opt_typee {
    CL_OPTION_TYPE_STRING,
    CL_OPTION_TYPE_INT,
    CL_OPTION_TYPE_HEX,
    CL_OPTION_TYPE_BOOL,
} clOptionType_e;

typedef struct _cl_opt_t {
    char *arg;
    clOptionType_e type;
    size_t size;
    void *ptr;
    char *description;
} clOpt;

char filename[CL_MAX_PATHSIZE];
char outfile[CL_MAX_PATHSIZE];
cl_uint numDevices;
cl_bool no_print_logo;
cl_bool verbose;
char cl_args[CL_MAX_STRING];
char precomp[CL_MAX_PATHSIZE];
char cl_device_types[CL_MAX_STRING];
clOpt options[] = {
    {"-f", CL_OPTION_TYPE_STRING, CL_MAX_PATHSIZE, filename,     "The input kernel file."},
    {"-d", CL_OPTION_TYPE_INT,    sizeof(cl_uint), &numDevices,  "The number of devices to target. Max of 10."},
    {"-n", CL_OPTION_TYPE_BOOL,   sizeof(cl_bool), &no_print_logo,"Disables printing of the logo."},
    {"-v", CL_OPTION_TYPE_BOOL,   sizeof(cl_bool), &verbose,     "Enables debuging statements."},
    {"-o", CL_OPTION_TYPE_STRING, CL_MAX_PATHSIZE, outfile,      "The output binary file"},
    {"-h", CL_OPTION_TYPE_STRING, CL_MAX_PATHSIZE, precomp,      "The precompiled header output file"},
    {"-W", CL_OPTION_TYPE_STRING, CL_MAX_STRING,   cl_args,      "Arguments to pass the OpenCL compiler for the kernels"},
    {"-t", CL_OPTION_TYPE_STRING, CL_MAX_STRING,   cl_device_types, "Specifies the device types supported [cpu|gpu|acc|all]"},
};
cl_uint numOpts = dimof(options);

void clOptions(int argc, char *argv[])
{
    cl_uint i = 0;
    cl_uint j = 0;
    for (i = 1; i < argc; i++)
    {
        if (verbose)
            printf("ARGV[%u]: \"%s\"\n",i,argv[i]);
        for (j = 0; j < numOpts; j++)
        {
            if (strcmp(argv[i], options[j].arg) == 0)
            {
                switch (options[j].type)
                {
                    case CL_OPTION_TYPE_STRING:
                        strncpy(options[j].ptr, argv[i+1], options[j].size);
                        break;
                    case CL_OPTION_TYPE_INT:
                        sscanf(argv[i+1], "%d", (cl_int *)options[j].ptr);
                        break;
                    case CL_OPTION_TYPE_HEX:
                        sscanf(argv[i+1], "%x", (cl_uint *)options[j].ptr);
                        break;
                    case CL_OPTION_TYPE_BOOL:
                        *(cl_bool *)options[j].ptr = CL_TRUE;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void notify(cl_program program, void *arg)
{
    cl_kernel_bin_t *bins = cl_extract_kernels(program);
    if (bins)
    {
        cl_dump_kernels(outfile, bins);
        cl_precompiled_header(precomp, bins);
        cl_delete_kernel_bin(bins);
    }
}

void print_logo(void)
{
    if (no_print_logo == CL_FALSE) {
       printf("OpenCL Test Compiler by Erik Rainey\n");
       printf("Version %lf Compiled on %s at %s\n",0.5,__DATE__,__TIME__);
    }
}

int main(int argc, char *argv[])
{
    // initialize the default values
    numDevices = 1;
    verbose = CL_FALSE;
    no_print_logo = CL_FALSE;
    strncpy(filename, "kernels.cl", CL_MAX_PATHSIZE);
    //strncpy(outfile,  "a.out", CL_MAX_PATHSIZE);
    memset(outfile, 0, sizeof(outfile));
    //strncpy(precomp,  "kernels.h", CL_MAX_PATHSIZE);
    memset(precomp, 0, sizeof(precomp));
    if (argc > 1)
    {
        cl_environment_t *pEnv = NULL;
		cl_uint dev_type = CL_DEVICE_TYPE_DEFAULT;
        clOptions(argc, argv);
        print_logo();
        if (verbose)
            printf("Loading file %s\n", filename);

        if (verbose && cl_args[0] != '\0')
            printf("CL ARGS: %s\n",cl_args);

		if (strncmp("cpu", cl_device_types, CL_MAX_STRING) == 0)
			dev_type = CL_DEVICE_TYPE_CPU;
		else if (strncmp("gpu", cl_device_types, CL_MAX_STRING) == 0)
			dev_type = CL_DEVICE_TYPE_GPU;
		else if (strncmp("acc", cl_device_types, CL_MAX_STRING) == 0 || strncmp("accelator", cl_device_types, CL_MAX_STRING) == 0)
			dev_type = CL_DEVICE_TYPE_ACCELERATOR;
		else if (strncmp("all", cl_device_types, CL_MAX_STRING) == 0)
			dev_type = CL_DEVICE_TYPE_ALL;

        // process the kernel
        pEnv = clCreateEnvironment(filename, dev_type, numDevices, notify, cl_args);
        clDeleteEnvironment(pEnv);
    }
    else
    {
		int i;
        print_logo();
        printf("Usage:\n$ %s [OPTIONS] \n", argv[0]);
		for (i = 0; i < numOpts; i++)
		{
			printf("Option %s\t\t%s\n",options[i].arg, options[i].description);
		}
        printf("Example:\n$ %s -f %s -d %u -o %s\n",argv[0],filename,numDevices,outfile);
    }
    return 0;
}
