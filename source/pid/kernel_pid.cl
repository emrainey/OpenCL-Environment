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

#include <clpid.h>

__kernel void kernel_pid(__global cl_pid_t *pids)
{
    int i = get_global_id(0);
    int j;

    // new error
    float e = pids[i].sv - pids[i].pv;

    // "gain"
    pids[i].p_out = pids[i].kp * e;

    // "reset"
    pids[i].em[pids[i].idx] = e;
    pids[i].idx = (pids[i].idx + 1)%PCT_INT_WINDOW; // wrap around
    pids[i].iv = 0.0;
    for (j = 0; j < PCT_INT_WINDOW; j++)
        pids[i].iv += pids[i].em[j];
    pids[i].i_out = pids[i].ki * pids[i].iv;

    // "rate"
    pids[i].dp = pids[i].e - e;
    pids[i].d_out = pids[i].kd * pids[i].dp;

    // output
    pids[i].mv = pids[i].p_out + pids[i].i_out; + pids[i].d_out;

    // remember the last error
    pids[i].e = e;

}

