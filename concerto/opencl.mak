# Copyright (C) 2011 Erik Rainey
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

ifeq ($(OPENCL_ROOT),)
$(error OPENCL_ROOT must be defined to use OpenCL)
endif

KDIR?=$(HOST_ROOT)/source/kernels

ifdef BUILD_DEBUG
$(info For _MODULE=$(_MODULE) KDIR=$(KDIR))
endif

ifdef CL_DEBUG
	DEFS+=CL_DEBUG
endif

ifndef CL_USER_DEVICE_TYPE
	CL_USER_DEVICE_TYPE:=all
endif

ifndef CL_USER_DEVICE_COUNT
	CL_USER_DEVICE_COUNT:=1
endif

IDIRS += $(HOST_ROOT)/source/kernels
IDIRS += $($(_MODULE)_ODIR) 

CL_USER_DEVICE_COUNT=1
ifeq ($(HOST_OS),Windows_NT)
	CL_USER_DEVICE_TYPE=gpu
	SYSIDIRS += $(OPENCL_ROOT)/inc
	ifeq ($(HOST_CPU),X86)
		SYSLDIRS += $(OPENCL_ROOT)/lib/Win32
	else ifeq ($(HOST_CPU),X64)
		SYSLDIRS += $(OPENCL_ROOT)/lib/x64
	endif
	SYS_SHARED_LIBS += OpenCL
else ifeq ($(HOST_OS),LINUX)
	CL_USER_DEVICE_TYPE=gpu
	SYSIDIRS += $(OPENCL_ROOT)/inc
	ifeq ($(HOST_CPU),X86)
		SYSLDIRS += $(OPENCL_ROOT)/lib/Linux32
	else ifeq ($(HOST_CPU),X64)
		SYSLDIRS += $(OPENCL_ROOT)/lib/Linux64
	endif
	SYS_SHARED_LIBS += OpenCL
else ifeq ($(HOST_OS),DARWIN)
	CL_USER_DEVICE_TYPE=cpu
	SYSIDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/OpenCL.framework/Headers
	LDFLAGS+=-framework OpenCL 
	CFLAGS+=-fPIC
	DEFS+=__APPLE__
	ifdef CL_DEBUG
		DEFS+=VECLIB DEBUG _GLIBCXX_DEBUG=1 _GLIBCXX_DEBUG_PEDANTIC=1
	endif
endif

# OpenCL-Environment Compiler Support
ifeq ($(HOST_OS),Windows_NT)
CL:=$($(_MODULE)_TDIR)/clcompiler.exe
else ifeq ($(HOST_OS),CYGWIN)
CL:=$($(_MODULE)_TDIR)/clcompiler.exe
else
CL:=$($(_MODULE)_TDIR)/clcompiler
endif

# OpenCL-Environment Defines
ifeq ($(HOST_OS),CYGWIN)
DEFS+=KDIR="\"$(KDIR)\"" CL_USER_DEVICE_COUNT=$(CL_USER_DEVICE_COUNT) CL_USER_DEVICE_TYPE="\"$(CL_USER_DEVICE_TYPE)\""
else ifeq ($(HOST_OS),Windows_NT)
DEFS+=KDIR="$(call PATH_CONV,$(KDIR))\\" CL_USER_DEVICE_COUNT=$(CL_USER_DEVICE_COUNT) CL_USER_DEVICE_TYPE="$(CL_USER_DEVICE_TYPE)"
else
DEFS+=KDIR="$(KDIR)" CL_USER_DEVICE_COUNT=$(CL_USER_DEVICE_COUNT) CL_USER_DEVICE_TYPE="$(CL_USER_DEVICE_TYPE)"
endif

ifeq ($(HOST_OS),Windows_NT)
CL_DEFINE_HEADER:=$(call PATH_CONV, $($(_MODULE)_ODIR))\clDefines.h
else
CL_DEFINE_HEADER:=$($(_MODULE)_ODIR)/clDefines.h
endif

#$(info CL_DEFINE_HEADER=$(CL_DEFINE_HEADER))

define $(_MODULE)_PREPROC
$(_MODULE)_preprocessor: $(CL_DEFINE_HEADER)

$(CL_DEFINE_HEADER):
	@echo Creating Local OpenCL-Environment Header File
	-$(Q)$(PRINT) #ifndef _CLDEFINES_H_										>  $(CL_DEFINE_HEADER)
	-$(Q)$(PRINT) #define _CLDEFINES_H_ 									>> $(CL_DEFINE_HEADER)
	-$(Q)$(PRINT) #define KDIR $(KDIR) 										>> $(CL_DEFINE_HEADER)
	-$(Q)$(PRINT) #define CL_USER_DEVICE_COUNT $(CL_USER_DEVICE_COUNT) 		>> $(CL_DEFINE_HEADER)
	-$(Q)$(PRINT) #define CL_USER_DEVICE_TYPE  "$(CL_USER_DEVICE_TYPE)" 	>> $(CL_DEFINE_HEADER)
	-$(Q)$(PRINT) #endif													>> $(CL_DEFINE_HEADER)
endef

$(eval $(call $(_MODULE)_PREPROC))

ifdef CL_BUILD_RUNTIME

DEFS+=CL_BUILD_RUNTIME

else ifneq ($(CLSOURCES),)

# $(info Module $(_MODULE) has OpenCL Kernels $(CLSOURCES))

$(_MODULE)_KERNELS := $(CLSOURCES:%.cl=$($(_MODULE)_ODIR)/%.h)
$(_MODULE)_KIDIRS  := $(KIDIRS) $(HOST_ROOT)/include
$(_MODULE)_KDEFS   := $(KDEFS)

ifeq ($(HOST_OS),CYGWIN)
# The Clang/LLVM is a Windows Path Compiler
$(_MODULE)_KFLAGS+=$(foreach inc,$($(_MODULE)_KIDIRS),-I$(call P2W_CONV,$(inc))) $(foreach def,$($(_MODULE)_KDEFS),-D$(def))
else
$(_MODULE)_KFLAGS+=$(foreach inc,$($(_MODULE)_KIDIRS),-I$(inc)) $(foreach def,$($(_MODULE)_KDEFS),-D$(def))
endif

# Add ourselves to the pretarget modules
PRETARGET_MODULES += $(_MODULE)

define $(_MODULE)_KERNEL_BUILDS
$(_MODULE)_pretarget: $($(_MODULE)_KERNELS) 
	
$(_MODULE)_clean_pretarget:
	@echo Cleaning $($(_MODULE)_KERNELS)
	-$(Q)$(CLEAN) $(call PATH_CONV,$($(_MODULE)_KERNELS))
	
endef

define $(_MODULE)_COMPILE_CL
$($(_MODULE)_ODIR)/$(1).h: $(KDIR)/$(1).cl $(CL)
	@echo [PURE] Compiling OpenCL Kernel $$(notdir $$<)
	$$(call PATH_CONV,$(CL)) -n -f $$(call PATH_CONV,$$<) -d $(CL_USER_DEVICE_COUNT) -t $(CL_USER_DEVICE_TYPE) -h $$(call PATH_CONV,$$@) -W "$($(_MODULE)_KFLAGS)"
endef

$(eval $(call $(_MODULE)_KERNEL_BUILDS))

# Define explicit rules for each CL Sources.
$(foreach cl, $(basename $(CLSOURCES)), $(eval $(call $(_MODULE)_COMPILE_CL,$(cl))))

else # No CLSOURCES

# This is a support library for OpenCL-Environment.
# $(info Module $(_MODULE) is a support library)

endif # Runtime Builds

# Clean out the variables
CLSOURCES :=
KDEFS :=
KIDIRS :=
