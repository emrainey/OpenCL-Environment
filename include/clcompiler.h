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

#ifndef _CL_COMPILER_H_
#define _CL_COMPILER_H_

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

#endif
