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
	ifndef BOOST_SCI_PKG_ROOT
		ERROR_STRING=You must defined BOOST_SCI_PGK_ROOT!
	endif
endif

TARGETPATH=$(LOCAL_ROOT)/out
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
    IDIRS+=$(OPENCL_ROOT)$/include
    LIBS+=opencl
    LDIRS+=$(OPENCL_ROOT)$/lib
    DEFS+=
    CFLAGS+=-fPIC
else ifeq ($(HOST_OS),Windows_NT)
    IDIRS+=$(BOOST_SCI_PKG_ROOT)$\include
    LIBS+=opencl
	LDIRS+=$(BOOST_SCI_PKG_ROOT)$\lib
	DEFS+=_MSC_VER
endif

ifndef ERROR_STRING
    include $(LOCAL_ROOT)/build/gcc.mak
else
error:
	@echo $(ERROR_STRING)
endif
