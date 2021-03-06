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

#ifndef _CL_QUERY_H_
#define _CL_QUERY_H_

void clPrintAllPlatformInfo(cl_platform_id platform);
void clPrintAllDeviceInfo(cl_device_id device);
void clPrintAllContextInfo(cl_context context);
void clPrintAllQueueInfo(cl_command_queue queue);
void clPrintAllKernelInfo(cl_kernel kernel);
void clPrintAllEventInfo(cl_event event);
void clPrintAllKernelWorkInfo(cl_kernel kernel, cl_device_id device);

#endif
