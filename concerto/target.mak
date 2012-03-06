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

ifndef TARGET_PLATFORM
    TARGET_PLATFORM=PC
endif

ifneq ($(HOST_OS),DARWIN)
ifeq ($(OPENCL_ROOT),)
    $(warning OPENCL_ROOT must be defined to use OpenCL)
endif
endif

SYSIDIRS := $(HOST_ROOT)/include

ifdef CL_DEBUG
SYSDEFS += CL_DEBUG
endif

CL_USER_DEVICE_TYPE  ?= all
CL_USER_DEVICE_COUNT ?= 1

ifeq ($(TARGET_PLATFORM),PC)
    TARGET_OS=$(HOST_OS)
    TARGET_CPU=$(HOST_CPU)
    ifeq ($(TARGET_OS),LINUX)
        INSTALL_LIB := /usr/lib
        INSTALL_BIN := /usr/bin
        INSTALL_INC := /usr/include
        TARGET_NUM_CORES:=$(shell cat /proc/cpuinfo | grep processor | wc -l)
        SYSIDIRS += /usr/include
        SYSLDIRS += /usr/lib
        SYSDEFS+=_XOPEN_SOURCE=700 _BSD_SOURCE=1 _GNU_SOURCE=1
        ifdef OPENCL_ROOT
            CL_USER_DEVICE_TYPE ?= gpu
            SYSIDIRS += $(OPENCL_ROOT)/inc
            ifeq ($(TARGET_CPU),X86)
                SYSLDIRS += $(OPENCL_ROOT)/lib/Linux32
            else ifeq ($(TARGET_CPU),x86_64)
                SYSLDIRS += $(OPENCL_ROOT)/lib/Linux64
            endif
        endif
    else ifeq ($(TARGET_OS),CYGWIN)
        TARGET_NUM_CORES=1
        SYSDEFS+=_XOPEN_SOURCE=700 _BSD_SOURCE=1 _GNU_SOURCE=1 WINVER=0x501
    else ifeq ($(TARGET_OS),Windows_NT)
        TARGET_NUM_CORES := $(NUMBER_OF_PROCESSORS)
        SYSDEFS+=WIN32_LEAN_AND_MEAN WIN32 _WIN32 _CRT_SECURE_NO_DEPRECATE WINVER=0x0501 _WIN32_WINNT=0x0501
        ifdef OPENCL_ROOT
            CL_USER_DEVICE_TYPE ?= gpu
            SYSIDIRS += $(OPENCL_ROOT)/inc
            ifeq ($(HOST_CPU),X86)
                SYSLDIRS += $(OPENCL_ROOT)/lib/Win32
            else ifeq ($(HOST_CPU),X64)
                SYSLDIRS += $(OPENCL_ROOT)/lib/x64
            endif
        endif
    else ifeq ($(TARGET_OS),DARWIN)
         CL_USER_DEVICE_TYPE ?= cpu
         SYSIDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/OpenCL.framework/Headers
         #SYSDEFS+=__APPLE__
         ifdef CL_DEBUG
             SYSDEFS+=VECLIB DEBUG _GLIBCXX_DEBUG=1 _GLIBCXX_DEBUG_PEDANTIC=1
         endif
     endif
else
endif


SYSDEFS += $(TARGET_OS) $(TARGET_CPU) $(TARGET_PLATFORM) TARGET_NUM_CORES=$(TARGET_NUM_CORES)

ifeq ($(TARGET_CPU),X86)
    TARGET_ARCH=32
else ifeq ($(TARGET_CPU),X64)
    TARGET_ARCH=64
else ifeq ($(TARGET_CPU),x86_64)
    TARGET_ARCH=64
else ifeq ($(TARGET_CPU),ARM)
    TARGET_ARCH=32
else ifeq ($(TARGET_CPU),i386)
    TARGET_ARCH=32
endif

ifndef TARGET_ARCH
TARGET_ARCH=32
endif

SYSDEFS+=ARCH_$(TARGET_ARCH)

$(info TARGET_OS=$(TARGET_OS))
$(info TARGET_CPU=$(TARGET_CPU))
$(info TARGET_PLATFORM=$(TARGET_PLATFORM))

