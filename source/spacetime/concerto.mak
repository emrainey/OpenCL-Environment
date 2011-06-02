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

TARGET     := spacetime
TARGETTYPE := exe
CSOURCES   := camera.c display.c init.c keyboard.c main.c movement.c physics.c utils.c vector.c
STATIC_LIBS := clenvironment

include $(HOST_ROOT)/$(BUILD_FOLDER)/opencl.mak
include $(HOST_ROOT)/$(BUILD_FOLDER)/glut.mak

-include $(FINALE)