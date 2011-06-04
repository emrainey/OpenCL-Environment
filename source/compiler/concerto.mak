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

TARGET		:= clcompiler
TARGETTYPE	:= exe
CSOURCES    := clcompiler.c
STATIC_LIBS := clenvironment clquery 
SHARED_LIBS :=

include $(HOST_ROOT)/$(BUILD_FOLDER)/opencl.mak

-include $(FINALE)
