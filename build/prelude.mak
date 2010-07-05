# Copyright (C) 2010 Erik Rainey
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include $(LOCAL_ROOT)/build/os.mak
include $(LOCAL_ROOT)/build/machine.mak

# Normal Make Paths and Defines
TDIR=$(LOCAL_ROOT)/out
ODIR=$(LOCAL_ROOT)/out/obj
IDIRS=$(LOCAL_ROOT)/include .
LDIRS=$(LOCAL_ROOT)/out

# OpenCL Kernel Make Paths and Defines
# This has to have a trailing / due to it's inclusion in the C/C++ files
KDIR=$(LOCAL_ROOT)/sources/kernels/
KIDIRS=$(LOCAL_ROOT)/include .
KDEFS=
KFLAGS=-w -cl-mad-enable
#-cl-opt-disable
