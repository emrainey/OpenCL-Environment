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

CC = $(CROSS_COMPILE)gcc
CP = $(CROSS_COMPILE)g++
AS = $(CROSS_COMPILE)as
AR = $(CROSS_COMPILE)ar
CL = $(TDIR)/clcompiler

ifdef DEBUG
Q=
else
Q=@
endif

ifeq ($(OS),Windows_NT)
PATH_CONV=$(subst /,\,$(1))
else
PATH_CONV=$(subst \,/,$(1))
endif

ifdef LOGFILE
LOGGING=&>>$(LOGFILE)
else
LOGGING=
endif

ifdef CL_DEBUG
DEFS+=CL_DEBUG
endif

ifndef NUM_KERNELS
NUM_KERNELS=1
endif

ifeq ($(strip $(TARGETTYPE)),library)
	BIN_PRE=lib
	BIN_EXT=.a
else ifeq ($(strip $(TARGETTYPE)),dsmo)
	BIN_PRE=lib
	BIN_EXT=.so
else ifeq ($(strip $(TARGETTYPE)),exe)
	BIN_PRE=
	BIN_EXT=
endif

TARGET_BIN=$(TDIR)/$(BIN_PRE)$(TARGET)$(BIN_EXT)
ifdef INSTALL_DIR
TARGET_INSTALLED=$(INSTALL_DIR)/$(BIN_PRE)$(TARGET)$(BIN_EXT)
endif

OBJECTS=$(ASSEMBLY:%.S=$(ODIR)/%.o) $(CPPSOURCES:%.cpp=$(ODIR)/%.o) $(CSOURCES:%.c=$(ODIR)/%.o)
KERNELS=$(CLSOURCES:%.cl=%.h)
KOPTIONS=$(CLSOURCES:%.cl=$(ODIR)/%.clopt)
INCLUDES=$(foreach inc,$(IDIRS),-I$(call PATH_CONV,$(inc)))
DEFINES=$(foreach def,$(DEFS),-D$(def))
LDFLAGS+=-Wall
LIBRARIES=$(foreach ldir,$(LDIRS),-L$(call PATH_CONV,$(ldir))) $(foreach lib,$(LIBS),-l$(lib))
AFLAGS=-ahlms=$(ODIR)/listing.S $(INCLUDES)
CFLAGS+=-Wall -ggdb -c $(INCLUDES) $(DEFINES) -DKDIR="\"$(KDIR)\""

all: Makefile $(SOURCES) $(TARGET_BIN)

kernels: $(KOPTIONS) $(KERNELS)

ifdef TESTCASE
test: clean install
	$(TESTCASE)
endif

ifeq ($(strip $(TARGETTYPE)),library)

$(TARGET_BIN): Makefile $(OBJECTS) $(KERNELS)
	@echo Linking $@
	$(Q)$(AR) -rcsu $@ $(OBJECTS) $(LOGGING)

install: $(TARGET_BIN)
	@echo No install step for $(TARGET)

else ifeq ($(strip $(TARGETTYPE)),exe)

$(TARGET_BIN): Makefile $(OBJECTS) $(KERNELS) $(KOPTIONS)
	@echo Linking $@
	$(Q)$(CP) $(LDFLAGS) $(OBJECTS) $(LIBRARIES) -o $(call PATH_CONV,$@) $(LOGGING)

install: $(TARGET_BIN)
ifdef INSTALL_DIR
	-$(Q)chmod a+x $(TARGET_BIN)
	-$(Q)cp $(TARGET_BIN) $(INSTALL_DIR)
else
	@echo No INSTALL_DIR specified for target $(TARGET)
endif

else ifeq ($(strip $(TARGETTYPE)),dsmo)

$(TARGET_BIN): $(OBJECTS) $(KERNELS)
	@echo Linking $(OBJECTS) into $@
	$(Q)$(CP) -shared -Wl,-soname,$(TARGET_BIN) -o $(TARGET) $(call PATH_CONV,$(OBJECTS))  $(LOGGING)

install: $(TARGET)
ifdef INSTALL_DIR
	-$(Q)cp -Rfp $(TARGET_BIN) $(INSTALL_DIR)
	-$(Q)ln -s $(TARGET_INSTALLED) $(TARGET_INSTALLED).1.0
else
	@echo No INSTALL_DIR specified for target $(TARGET)
endif

endif

ifdef INSTALL_DIR
rebuild: clean install
else
rebuild: clean all
endif

uninstall:
ifdef INSTALL_DIR
	-$(Q)$(CLEAN) $(TARGET_INSTALLED)
else
	@echo No INSTALL_DIR for $(TARGET)
endif

clean: clean_kernels
	@echo Cleaning $(OBJECTS) $(TARGET_BIN) $(LOGFILE)
	-$(Q)$(CLEAN) $(call PATH_CONV,$(OBJECTS))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(TARGET_BIN))
ifdef LOGFILE
 	-$(Q)$(CLEAN) $(call PATH_CONV,$(LOGFILE))
endif

clean_kernels:
ifneq ($(KOPTIONS),)
	-$(Q)$(CLEAN) $(call PATH_CONV,$(KOPTIONS))
endif
ifneq ($(KERNELS),)
	-$(Q)$(CLEAN) $(call PATH_CONV,$(KERNELS))
endif


$(ODIR)/%.o: %.c $(KERNELS)
	@echo Compiling C $<
	$(Q)$(CC) $(CFLAGS) $< -o $@  $(LOGGING)

$(ODIR)/%.o: %.cpp $(KERNELS)
	@echo Compiling C++ $<
	$(Q)$(CP) $(CFLAGS) $< -o $@  $(LOGGING)

$(ODIR)/%.o: %.S
	@echo Assembling $<
	$(Q)$(AS) $(AFLAGS) $< -o $@  $(LOGGING)

%.h: $(KDIR)/%.cl
	@echo Compiling OpenCL Kernel $<
	-$(Q)$(CL) -n -f $< -d 1 -t cpu -h $@ -W "$(DEFINES) $(INCLUDES)"

$(ODIR)/%.clopt: Makefile
	@echo Building local options file $@ for building OpenCL kernel dependencies.
	$(Q)echo $(DEFINES) $(INCLUDES) > $@

info:
	@echo OS=$(OS)
	@echo TARGET=$(TARGET)
	@echo TARGETTYPE=$(TARGETTYPE)
	@echo TARGET_BIN=$(TARGET_BIN)
	@echo TARGET_INSTALLED=$(TARGET_INSTALLED)
	@echo CC=$(CC)
	@echo CP=$(CP)
	@echo AR=$(AR)
	@echo AS=$(AS)
	@echo OBJECTS=$(OBJECTS)
	@echo LDFLAGS=$(LDFLAGS)
	@echo CFLAGS=$(CFLAGS)
	@echo DEFS=$(DEFS)
	@echo KERNELS=$(KERNELS)
	@echo KOPTIONS=$(KOPTIONS)

