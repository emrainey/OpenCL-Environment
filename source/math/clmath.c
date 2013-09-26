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
#include <math.h>
#include <clenvironment.h>

int ipow(int x, int n)
{
    if (n == 0)
        return 1;
    else if (n == 1)
        return x;
    else
        return x * ipow(x, n - 1);
}

cl_uint isqrt(cl_int x)
{
    float xp = (float)x;
    return (cl_uint)sqrt(xp);
}

float frand(void)
{
    return (float)(rand()%1000)/999;
}

float frrand(float low, float hi)
{
    float r = hi + low;
    int p = 5; // arbitrary precision
    int n = ipow(10,p);
    float v = (float)(rand()%n)/(n-1);
    float x = (v * r) + low;
    //printf("frand [%lf, %lf] n=%d, v=%lf, x=%lf\n", low, hi, n, v,x);
    return x;
}

float fsum(float *a, cl_uint num)
{
    float b = 0.0;
    cl_uint i = 0;
    for (i = 0; i < num; i++)
        b += a[i];
    return b;
}

int rrand(int low, int hi)
{
    int r = hi - low;
    return (rand() % r) + low;
}

void frand4(cl_float4 f, cl_int l, cl_int h)
{
#if 0 //defined(DARWIN)
    f[0] = frand() * ipow(10, rrand(l,h));
    f[1] = frand() * ipow(10, rrand(l,h));
    f[2] = frand() * ipow(10, rrand(l,h));
    f[3] = frand() * ipow(10, rrand(l,h));
#else
    f.s[0] = frand() * ipow(10, rrand(l,h));
    f.s[1] = frand() * ipow(10, rrand(l,h));
    f.s[2] = frand() * ipow(10, rrand(l,h));
    f.s[3] = frand() * ipow(10, rrand(l,h));
#endif
}

void normalize_float(float *a, cl_int low, cl_int hi, cl_uchar *b, cl_uint numPixels)
{
    cl_uint i = 0;
    cl_uint range = hi - low;
    //printf("Range for %p is %d to %d (%d)\n", a, low, hi, range);
    for (i = 0; i < numPixels; i++)
    {
        cl_int c = (cl_int)((range * a[i]) + low);
        //printf("a = %lf, c = %d\n",a[i], c);
        if (c > hi)
            b[i] = (cl_uchar)hi;
        else if (c < low)
            b[i] = (cl_uchar)low;
        else
            b[i] = (cl_uchar)c;
    }
}
