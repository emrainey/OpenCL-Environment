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

#ifndef _CL_IMGFILTER_H_
#define _CL_IMGFILTER_H_

typedef enum _cl_dimensions_e {
    X_DIM = 0,
    Y_DIM,
    Z_DIM,
    DIM_MAX,
} clDimensions_e;

typedef struct _cl_image_t {
    unsigned char *data[4];
    unsigned int   planes;
    size_t         dimensions[3];
    unsigned int   strides[3];
} cl_Image_t;

typedef struct _cl_kernel_t {
    unsigned char     *data;
    unsigned int       x_dim;
    unsigned int       y_dim;
    unsigned int       n_dim;
} cl_Kernel_t;

#endif

