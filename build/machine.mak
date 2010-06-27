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

ifeq ($(HOST_OS),Windows_NT)
	HOST_CPU=X64
else
	HOST_CPU=$(shell uname -m)
	ifeq ($(HOST_CPU),Power Macintosh)
		HOST_CPU=PPC
	else ifeq ($(HOST_CPU),i686)
		HOST_CPU=X86
	else ifeq ($(HOST_CPU),i586)
		HOST_CPU=X86
	else ifeq ($(HOST_CPU),i486)
		HOST_CPU=X86
	else ifeq ($(HOST_CPU),i386)
		HOST_CPU=X86
	else ifeq ($(HOST_CPU),ARM)
		HOST_CPU=ARM
	endif
endif

