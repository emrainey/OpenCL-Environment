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

ifeq ($(HOST_OS),CYGWIN)
	ifndef OPENCL_ROOT
		ERROR_STRING=You must defined OPENCL_ROOT!
	endif
else ifeq ($(HOST_OS),Windows_NT)
	ifndef OPENCL_ROOT
		ERROR_STRING=You must defined OPENCL_ROOT!
	endif
endif

TDIR=$(LOCAL_ROOT)/out
ODIR=$(LOCAL_ROOT)/out/obj
IDIRS+=$(LOCAL_ROOT)/include .
LDIRS+=$(LOCAL_ROOT)/out
KDIR=$(LOCAL_ROOT)/sources/kernels/
IDIRS+=$(KDIR)

ifeq ($(HOST_OS),DARWIN)
    IDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/OpenCL.framework/Headers
    LDFLAGS+=-framework OpenCL
    CFLAGS+=-fPIC
    DEFS+=__APPLE__
    ifdef CL_DEBUG
		DEFS+=VECLIB DEBUG _GLIBCXX_DEBUG=1 _GLIBCXX_DEBUG_PEDANTIC=1
	endif
else ifeq ($(HOST_OS),CYGWIN)
    IDIRS+=$(OPENCL_ROOT)/inc
    LIBS+=OpenCL
    #LDIRS+=$(OPENCL_ROOT)/lib/x64 
	LDIRS+=$(WINSYSDIR)
    DEFS+=_MSC_VER=1500
else ifeq ($(HOST_OS),Windows_NT)
    LIBS+=OpenCL
	IDIRS+=$(OPENCL_ROOT)\inc
	LDIRS+=$(OPENCL_ROOT)\lib\x64
	DEFS+=_DEBUG _CONSOLE
endif

ifndef ERROR_STRING
	ifeq ($(HOST_COMPILER),GCC)
		include $(LOCAL_ROOT)/build/gcc.mak
	else ifeq ($(HOST_COMPILER),CL)
		include $(LOCAL_ROOT)/build/cl.mak
	endif
else
error:
	@echo $(ERROR_STRING)
endif
