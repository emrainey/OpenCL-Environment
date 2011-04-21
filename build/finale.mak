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

# Apple platforms don't need a root definition
ifneq ($(HOST_OS),DARWIN)
	ifndef OPENCL_ROOT
		ERROR_STRING=You must define OPENCL_ROOT!
	endif
endif

ifeq ($(HOST_OS),DARWIN)
	IDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/OpenCL.framework/Headers
	IDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/OpenGL.framework/Headers
	IDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/GLUT.framework/Headers
	LDFLAGS+=-framework OpenCL -framework OpenGL -framework GLUT
	CFLAGS+=-fPIC
	DEFS+=__APPLE__
	ifdef CL_DEBUG
		DEFS+=VECLIB DEBUG _GLIBCXX_DEBUG=1 _GLIBCXX_DEBUG_PEDANTIC=1
	endif
else ifeq ($(HOST_OS),LINUX)
	IDIRS+=$(OPENCL_ROOT)/inc
	LIBS+=OpenCL
	ifeq ($(HOST_CPU),x86_64)
		LDIRS+=$(OPENCL_ROOT)/lib/Linux64
	else
		LDIRS+=$(OPENCL_ROOT)/lib/Linux32
	endif
	DEFS+=LINUX
else ifeq ($(HOST_OS),CYGWIN)
	IDIRS+=$(OPENCL_ROOT)/inc
	LIBS+=OpenCL
	#LDIRS+=$(OPENCL_ROOT)/lib/x64 
	LDIRS+=$(WINSYSDIR)
	# Assume VS 2005 _MSC_VER number.
	DEFS+=_MSC_VER=1400 
else ifeq ($(HOST_OS),Windows_NT)
	LIBS+=OpenCL
	IDIRS+=$(OPENCL_ROOT)\inc
	LDIRS+=$(OPENCL_ROOT)\lib\x64
	DEFS+=_MSC_VER=1400 _DEBUG _CONSOLE _WIN32 _CRT_SECURE_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS _WIN32_WINNT=0x0501 WINVER=0x0501
endif

ifdef CL_DEBUG
	DEFS+=CL_DEBUG
endif

ifndef CL_DEVICE_TYPE
	CL_DEVICE_TYPE=all
endif

ifndef CL_DEVICE_COUNT
	CL_DEVICE_COUNT=1
endif

ifndef ERROR_STRING
	ifeq ($(HOST_COMPILER),GCC)
		include $(LOCAL_ROOT)/build/gcc.mak
	else ifeq ($(HOST_COMPILER),CL)
		include $(LOCAL_ROOT)/build/cl.mak
	endif
else
$(info $(ERROR_STRING))
error:
	@echo $(ERROR_STRING)
endif
