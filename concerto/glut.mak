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

ifeq ($(GLUT_ROOT),)
$(error GLUT_ROOT must be defined to use GLUT)
endif

IDIRS += $(GLUT_ROOT)/include
ifeq ($(HOST_OS),Windows_NT)
	SYSLDIRS += $(GLUT_ROOT)/lib
	SYS_SHARED_LIBS := glut32 glu32
else ifeq ($(HOST_OS),LINUX)
else ifeq ($(HOST_OS),DARWIN)
	SYSIDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/OpenGL.framework/Headers
	SYSIDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/GLUT.framework/Headers
	LDFLAGS+=-framework OpenGL -framework GLUT
endif
