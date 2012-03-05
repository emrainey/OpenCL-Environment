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

#$(info Including Definitions)
all-type-files = $(notdir $(wildcard $($(_MODULE)_SDIR)/$(1)))
all-java-files = $(call all-type-files,*.java)
all-c-files    = $(call all-type-files,*.c)
all-cpp-files  = $(call all-type-files,*.cpp)
all-h-files    = $(call all-type-files,include/*.h)
ifeq ($(HOST_OS),Windows_NT)
PATH_CONV=$(subst /,\,$(1))
else
PATH_CONV=$(subst \,/,$(1))
endif
P2W_CONV=$(patsubst \cygdrive\c\%,c:\%,$(subst /,\,$(1)))
W2P_CONV=$(subst \,/,$(patsubst C:\%,\cygdrive\c\% $(1)))
