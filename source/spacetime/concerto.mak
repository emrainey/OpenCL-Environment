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

_MODULE := kernel_spacetime
include $(PRELUDE)
TARGET  := kernel_spacetime
TARGETTYPE := opencl_kernel
CLSOURCES := kernel_spacetime.cl
ifeq ($(NO_DOUBLE),1)
DEFS+=GPGPU_NO_DOUBLE_SUPPORT
endif
include $(FINALE)

_MODULE := clspacetime
include $(PRELUDE)
TARGET := clspacetime
TARGETTYPE := exe
CSOURCES := spacetime.c
HEADERS  := kernel_spacetime
STATIC_LIBS := clenvironment clquery
USE_OPENCL := true
ifeq ($(NO_DOUBLE),1)
DEFS+=GPGPU_NO_DOUBLE_SUPPORT
endif
USE_GLUT := true
include $(FINALE)
