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

#ifndef _CL_MATH_H_
#define _CL_MATH_H_

int ipow(int x, int n);
float frand(void);
float frrand(float low, float hi);
float fsum(float *a, cl_uint num);
int rrand(int low, int hi);
void frand4(cl_float4 f, cl_int l, cl_int h);
void normalize_float(float *a, cl_int low, cl_int hi, cl_uchar *b, cl_uint numPixels);

#endif
