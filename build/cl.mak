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

UNIX_CONV= $(subst \,/,$(1))
PATH_CONV= $(subst /,\,$(1))
ESCAPE_CONV= $(subst /,\\,$(1))

# We need to convert all the paths over to the UNIX format so that the rules work right
# however, the command line in Windows won't work right with those types, so we'll rework 
# those commands are the last minute.
ODIR  := $(call UNIX_CONV,$(ODIR))
TDIR  := $(call UNIX_CONV,$(TDIR))
IDIRS := $(call UNIX_CONV,$(IDIRS))
LDIRS := $(call UNIX_CONV,$(LDIRS))
KDIR  := $(call UNIX_CONV,$(KDIR))

CC = CL
CP = CL
AS = ASM
AR = LIB
LD = LINK
CL = $(TDIR)\clcompiler.exe

ifdef LOGFILE
LOGFILE := $(call PATH_CONV,$(LOGFILE))
LOGGING=>$(LOGFILE)
endif

ifdef DEBUG
Q=
else
Q=@
endif

ifeq ($(strip $(TARGETTYPE)),library)
	BIN_PRE=
	BIN_EXT=.lib
else ifeq ($(strip $(TARGETTYPE)),dsmo)
	BIN_PRE=
	BIN_EXT=.dll
else ifeq ($(strip $(TARGETTYPE)),exe)
	BIN_PRE=
	BIN_EXT=.exe
endif

TARGET_BIN=$(TDIR)/$(BIN_PRE)$(TARGET)$(BIN_EXT)
TARGET_PDB=$(ODIR)/$(TARGET).pdb
ifdef INSTALL_DIR
	TARGET_INSTALLED=$(INSTALL_DIR)/$(BIN_PRE)$(TARGET)$(BIN_EXT)
endif

# C/C++ Defines
DEFS+=KDIR="\"$(call ESCAPE_CONV,$(KDIR))\"" CL_USER_DEVICE_COUNT=$(CL_USER_DEVICE_COUNT) CL_USER_DEVICE_TYPE="\"$(CL_USER_DEVICE_TYPE)\""

OBJECTS=$(ASSEMBLY:%.S=$(ODIR)/%.obj) $(CPPSOURCES:%.cpp=$(ODIR)/%.obj) $(CSOURCES:%.c=$(ODIR)/%.obj)
ifdef CL_BUILD_RUNTIME
DEFS+=CL_BUILD_RUNTIME
else ifdef CLSOURCES
KERNELS=$(CLSOURCES:%.cl=%.h)
KOPTIONS=$(CLSOURCES:%.cl=%.clopt)
KFLAGS=$(foreach inc,$(KIDIRS),-I$(call PATH_CONV,$(inc))) $(foreach def,$(KDEFS),-D$(def))
endif
INCLUDES=$(foreach inc,$(IDIRS),/I$(call PATH_CONV,$(inc)))
DEFINES=$(foreach def,$(DEFS),/D$(def))
LDFLAGS=/INCREMENTAL /nologo /DEBUG /PDB:$(call PATH_CONV,$(TARGET_PDB)) /SUBSYSTEM:CONSOLE /MACHINE:$(HOST_CPU)
ARFLAGS=/nologo /MACHINE:$(HOST_CPU)
LIBRARIES=$(foreach ldir,$(LDIRS),/LIBPATH:"$(ldir)") $(foreach lib,$(LIBS),$(lib).lib)
AFLAGS=$(INCLUDES)
CFLAGS+=/c $(INCLUDES) $(DEFINES) $(COPT) /nologo /Wp64 /favor:AMD64 /MDd /EHsc /Gm /W3 /Zi /TP /Fd$(call PATH_CONV,$(TARGET_PDB))

ifdef DEFFILE
DEF=/DEF:$(DEFFILE)
else
DEF=
endif

all: $(SOURCES) $(TARGET_BIN)

ifdef TESTCASE
test: clean install
	$(TESTCASE)
endif

ifeq ($(TARGETTYPE),library)

$(TARGET_BIN): $(OBJECTS)
	@echo Archiving $@
	$(Q)$(AR) $(ARFLAGS) /OUT:$(call PATH_CONV,$@) $(call PATH_CONV,$<) $(LOGGING)

install: $(TARGET_BIN)
	@echo No install step for $(TARGET_BIN)

else ifeq ($(TARGETTYPE),exe)

$(TARGET_BIN): $(OBJECTS) $(foreach lib,$(LIBS),$(lib).lib)
	@echo Linking $@
	$(Q)$(LD) $(LDFLAGS) /OUT:$(call PATH_CONV,$@) $(LIBRARIES) $(call PATH_CONV,$<) $(LOGGING)

%.lib: force_look
	
install: $(TARGET_BIN)
	@echo No install step for $(TARGET_BIN)

else ifeq ($(TARGETTYPE),dsmo)

$(TARGET_BIN): $(OBJECTS)
	@echo Linking $@
	$(Q)$(LD) $(LDFLAGS) /OUT:$(call PATH_CONV,$@) $(LIBRARIES) $(call PATH_CONV,$<) /DLL $(DEF) $(LOGGING)

install: $(TARGET_BIN)
	@echo No install step for $(TARGET_BIN)

else ifeq ($(TARGETTYPE),objects)

$(TARGET_BIN): $(OBJECTS)

endif

.PRECIOUS: $(KERNELS)

force_look:

clean: clean_kernels
	@echo Cleaning Objects
	-$(Q)$(CLEAN) $(call PATH_CONV,$(OBJECTS))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(OBJECTS:%.obj=%.lib))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(OBJECTS:%.obj=%.exp))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(TARGET_BIN))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(TARGET_BIN:%.dll=%.exp))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(TARGET_BIN:%.dll=%.lib))
ifdef LOGFILE
	-$(Q)$(CLEAN) $(LOGFILE)
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

clean_kernels:
ifneq ($(KOPTIONS),) 
	@echo Cleaning Kernel Option File $(KOPTIONS)
	-$(Q)$(CLEAN) $(KOPTIONS)
endif
ifneq ($(KERNELS),)
	@echo Cleaning Precompiled Kernels $(KERNELS)
	-$(Q)$(CLEAN) $(KERNELS)
endif

$(ODIR)/%.obj: %.c $(KERNELS)
	@echo Compiling C $<
	$(Q)$(CC) $(CFLAGS) $(call PATH_CONV,$<) /Fo$(call PATH_CONV,$@)

$(ODIR)/%.obj: %.cpp $(KERNELS)
	@echo Compiling C++ $<
	$(Q)$(CP) $(CFLAGS) $(call PATH_CONV,$<) /Fo$(call PATH_CONV,$@) $(LOGGING)

$(ODIR)/%.obj: %.S
	@echo Assembling $<
	$(Q)$(AS) $(AFLAGS) $(call PATH_CONV,$<) /Fo$(call PATH_CONV,$@) $(LOGGING)

%.h: $(KDIR)%.cl
	@echo Compiling OpenCL Kernel $<
	$(Q)$(CL) -v -n -f $(call PATH_CONV,$<) -d $(CL_USER_DEVICE_COUNT) -t $(CL_USER_DEVICE_TYPE) -h $(call PATH_CONV,$@) -W "$(KFLAGS)"

%.clopt: Makefile
	@echo Building local options file $@ for building OpenCL kernel dependencies.
	$(Q)echo $(DEFINES) $(INCLUDES) > $(call PATH_CONV,$@)

info:
	@echo HOST_OS=$(OS)
	@echo HOST_COMPILER=$(HOST_COMPILER)
	@echo HOST_CPU=$(HOST_CPU)
	@echo HOST_VARIANT=$(HOST_VARIANT)
	@echo TARGET=$(TARGET)
	@echo TARGETTYPE=$(TARGETTYPE)
	@echo TARGET_BIN=$(TARGET_BIN)
	@echo TARGET_INSTALLED=$(TARGET_INSTALLED)
	@echo CC=$(CC) CP=$(CP) AR=$(AR) AS=$(AS) LD=$(LD)
	@echo CSOURCES=$(CSOURCES)
	@echo CLSOURCES=$(CLSOURCES)
	@echo OBJECTS=$(OBJECTS)
	@echo LDFLAGS=$(LDFLAGS)
	@echo CFLAGS=$(CFLAGS)
	@echo DEFS=$(DEFS)
	@echo KERNELS=$(KERNELS)
	@echo KOPTIONS=$(KOPTIONS)

