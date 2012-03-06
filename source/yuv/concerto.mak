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

-include $(PRELUDE)
TARGET     := clyuv
TARGETTYPE := exe
CSOURCES   := yuv.c
HEADERS    := kernel_yuv
STATIC_LIBS := clenvironment clmath clquery
TESTCASE   := $(TARGET)
ifeq ($(TARGET_OS),DARWIN)
LDFLAGS+=-framework OpenCL
else
SYS_SHARED_LIBS := OpenCL
endif
-include $(FINALE)

_MODULE = kernel_yuv
-include $(PRELUDE)
TARGET      := kernel_yuv
TARGETTYPE  := opencl_kernel
CLSOURCES   := kernel_yuv.cl
-include $(FINALE)


