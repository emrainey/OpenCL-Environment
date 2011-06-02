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

# Define all but don't do anything with it yet.
.PHONY: all
all:

# Define our pathing
HOST_ROOT?=$(abspath .)

$(info HOST_ROOT=$(HOST_ROOT))

ifndef BUILD_FOLDER
BUILD_FOLDER=concerto
endif

include $(HOST_ROOT)/$(BUILD_FOLDER)/os.mak
include $(HOST_ROOT)/$(BUILD_FOLDER)/machine.mak
include $(HOST_ROOT)/$(BUILD_FOLDER)/target.mak
include $(HOST_ROOT)/$(BUILD_FOLDER)/shell.mak

# Define the prelude and finale files so that SUBMAKEFILEs know what they are
# And if the users go and make -f concerto.mak then it will not work right.
PRELUDE := $(HOST_ROOT)/$(BUILD_FOLDER)/prelude.mak
FINALE  := $(HOST_ROOT)/$(BUILD_FOLDER)/finale.mak
SUBMAKEFILE := concerto.mak

# Remove the implicit rules for compiling.
.SUFFIXES:

# Allows the commands to be printed out when invoked
ifeq ($(BUILD_DEBUG),1)
Q=
else
Q=@
endif

# If no directories were specified, then assume "source"
ifeq ($(DIRECTORIES),)
DIRECTORIES=source
endif

# Find all the Makfiles in the subfolders, these will be pulled in to make 
ifeq ($(HOST_OS),Windows_NT)
    TARGET_MAKEFILES:=$(foreach dir,$(DIRECTORIES),$(shell cd $(dir) && dir /b /s $(SUBMAKEFILE)))
else
    TARGET_MAKEFILES:=$(foreach dir,$(DIRECTORIES),$(shell find $(dir)/ -name $(SUBMAKEFILE)))
endif
#$(info TARGET_MAKEFILES=$(TARGET_MAKEFILES))

# Now determine the MODULE names from this list
ifeq ($(HOST_OS),Windows_NT)
    MODULES:=$(foreach target,$(TARGET_MAKEFILES),$(lastword $(filter-out $(SUBMAKEFILE),$(subst \, ,$(target)))))
else
    MODULES:=$(foreach target,$(TARGET_MAKEFILES),$(lastword $(filter-out $(SUBMAKEFILE),$(subst /, ,$(target)))))
endif
#$(info MODULES=$(MODULES))

PRETARGET_MODULES :=

include $(TARGET_MAKEFILES)

.PHONY: all dir depend build install uninstall clean preprocessor pretarget targets scrub vars test

depend: $(foreach mod, $(MODULES), $(mod)_depend)

all: build

build: pretarget $(foreach mod, $(MODULES), $(mod)_build)
	@echo $< stage complete

pretarget: dir depend preprocessor $(foreach mod, $(PRETARGET_MODULES), $(mod)_pretarget)
	@echo $< stage complete

preprocessor: $(foreach mod, $(MODULES), $(mod)_preprocessor)
	@echo $< stage complete

install: build $(foreach mod, $(MODULES), $(mod)_install)
	@echo $< stage complete

uninstall: $(foreach mod, $(MODULES), $(mod)_uninstall)
	@echo $< stage complete
	
clean: $(foreach mod, $(MODULES), $(mod)_clean)
	@echo $< stage complete
	
targets:
	@echo all, build, install, uninstall, scrub, test, vars, clean
	@echo TARGETS=$(MODULES)

scrub:
ifeq ($(HOST_OS),Windows_NT)
ifdef DEBUG
	@echo [ROOT] Deleting $(HOST_ROOT)\out\$(TARGET_CPU)\debug
	-@$(CLEAN) $(call PATH_CONV,$(HOST_ROOT))\out\$(TARGET_CPU)\debug
else
	@echo [ROOT] Deleting $(HOST_ROOT)\out\$(TARGET_CPU)\release
	-@$(CLEAN) $(call PATH_CONV,$(HOST_ROOT))\out\$(TARGET_CPU)\release
endif
else
ifdef DEBUG
	@echo [ROOT] Deleting $(HOST_ROOT)/out/$(TARGET_CPU)/debug
	-@$(CLEANDIR) $(HOST_ROOT)/out/$(TARGET_CPU)/debug
else
	@echo [ROOT] Deleting $(HOST_ROOT)/out/$(TARGET_CPU)/release
	-@$(CLEANDIR) $(HOST_ROOT)/out/$(TARGET_CPU)/release
endif
endif

vars: $(foreach mod,$(MODULES),$(mod)_vars)
	@echo HOST_ROOT=$(HOST_ROOT)
	@echo HOST_OS=$(HOST_OS)
	@echo HOST_COMPILER=$(HOST_COMPILER)
	@echo TARGET_CPU=$(TARGET_CPU)
	@echo MAKEFILE_LIST=$(MAKEFILE_LIST)
	@echo TARGET_MAKEFILES=$(TARGET_MAKEFILES)

test: $(foreach mod,$(MODULES),$(mod)_test)
	@echo Executing Unit tests

