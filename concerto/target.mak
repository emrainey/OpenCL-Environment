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

ifeq ($(TARGET_PLATFORM),)
	TARGET_PLATFORM=PC
endif

ifeq ($(TARGET_PLATFORM),PC)
	TARGET_OS=$(HOST_OS)
	TARGET_CPU=$(HOST_CPU)
endif

ifeq ($(TARGET_PLATFORM),MAC)
     TARGET_OS=$(HOST_OS)
     TARGET_CPU=i386
endif

SYSDEFS += $(TARGET_CPU)
SYSDEFS += $(TARGET_OS)

$(info TARGET_OS=$(TARGET_OS))
$(info TARGET_CPU=$(TARGET_CPU))
$(info TARGET_PLATFORM=$(TARGET_PLATFORM))

