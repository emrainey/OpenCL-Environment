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

ifeq ($(DEPRECATED),true)
-include $(PRELUDE)
SKIPBUILD:=1
TARGET     := spacetime
TARGETTYPE := exe
CSOURCES   := camera.c display.c init.c keyboard.c main.c movement.c physics.c utils.c vector.c
STATIC_LIBS := clenvironment
ifeq ($(TARGET_OS),DARWIN)
LDFLAGS+=-framework OpenCL
else
SYS_SHARED_LIBS := $(OCL_LIB)
endif
HEADERS := kernel_spacetime
include $(HOST_ROOT)/$(BUILD_FOLDER)/glut.mak
-include $(FINALE)
endif

_MODULE := kernel_spacetime
include $(PRELUDE)
SKIPBUILD:=1
TARGET  := kernel_spacetime
TARGETTYPE := opencl_kernel
CLSOURCES := kernel_spacetime.cl
ifeq ($(NO_DOUBLE),1)
DEFS+=GPGPU_NO_DOUBLE_SUPPORT
endif
include $(FINALE)

_MODULE := clspacetime
include $(PRELUDE)
SKIPBUILD:=1
TARGET := clspacetime
TARGETTYPE := exe
CSOURCES := spacetime.c
HEADERS  := kernel_spacetime
STATIC_LIBS := clenvironment clquery
ifeq ($(TARGET_OS),DARWIN)
LDFLAGS+=-framework OpenCL
else
SYS_SHARED_LIBS := OpenCL
endif
ifeq ($(NO_DOUBLE),1)
DEFS+=GPGPU_NO_DOUBLE_SUPPORT
endif
include $(HOST_ROOT)/$(BUILD_FOLDER)/glut.mak
include $(FINALE)
