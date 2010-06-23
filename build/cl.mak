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

CC = CL
CP = CL
AS = ASM
AR = LIB
LD = LINK
CL = $(TDIR)/clcompiler

PATH_CONV= $(subst /,\,$(1))

ifdef LOGFILE
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

TARGET_BIN=$(TDIR)\$(BIN_PRE)$(TARGET)$(BIN_EXT)
ifdef INSTALL_DIR
TARGET_INSTALLED=$(INSTALL_DIR)/$(BIN_PRE)$(TARGET)$(BIN_EXT)
endif
OBJECTS=$(ASSEMBLY:%.S=$(ODIR)/%.obj) $(CPPSOURCES:%.cpp=$(ODIR)/%.obj) $(CSOURCES:%.c=$(ODIR)/%.obj)
KERNELS=$(CLSOURCES:%.cl=%.h)
KOPTIONS=$(CLSOURCES:%.cl=$(ODIR)/%.clopt)
INCLUDES=$(foreach inc,$(call PATH_CONV, $(IDIRS)),/I$(inc))
DEFINES=$(foreach def,$(DEFS),/D$(def)) /D_CRT_SECURE_NO_DEPRECATE /DWIN32 /D_WIN32_WINNT=0x0501 /DWINVER=0x0501 /D_MSC_VER=1500
LDFLAGS=-Wall
LIBRARIES=$(foreach ldir,$(LDIRS),/LIBPATH:$(ldir)) $(foreach lib,$(LIBS),$(lib).lib)
AFLAGS=$(INCLUDES)
CFLAGS+=/c $(INCLUDES) $(DEFINES) $(COPT) /nologo /Wp64 /MT
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
	$(Q)$(AR) /NOLOGO /MACHINE:$(HOST_CPU) /OUT:$@ $(call PATH_CONV,$(OBJECTS)) $(LOGGING)

install: $(TARGET_BIN)
	@echo No install step for $(TARGET_BIN)

else ifeq ($(TARGETTYPE),exe)

$(TARGET_BIN): $(OBJECTS)
	@echo Linking $@
	$(Q)$(LD) /nologo /OUT:$(call PATH_CONV,$@) $(LIBRARIES) $(call PATH_CONV,$(OBJECTS)) $(LOGGING)

install: $(TARGET_BIN)
	@echo No install step for $(TARGET_BIN)

else ifeq ($(TARGETTYPE),dsmo)

$(TARGET_BIN): $(OBJECTS)
	@echo Linking $@
	$(Q)$(LD) /nologo /OUT:$(call PATH_CONV,$@) $(LIBRARIES) $(call PATH_CONV,$(OBJECTS)) /DLL $(DEF) $(LOGGING)

install: $(TARGET_BIN)
	@echo No install step for $(TARGET_BIN)

else ifeq ($(TARGETTYPE),objects)

$(TARGET_BIN): $(OBJECTS)

endif

clean: clean_kernels
	@echo Cleaning Objects
	-$(Q)$(CLEAN) $(call PATH_CONV,$(OBJECTS))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(OBJECTS:%.obj=%.lib))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(OBJECTS:%.obj=%.exp))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(TARGET_BIN))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(TARGET_BIN:%.dll=%.exp))
	-$(Q)$(CLEAN) $(call PATH_CONV,$(TARGET_BIN:%.dll=%.lib))
ifdef LOGFILE
	-$(Q)$(CLEAN) $(call PATH_CONV, $(LOGFILE))
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
	-$(Q)$(CLEAN) $(call PATH_CONV,$(KOPTIONS))
endif
ifneq ($(KERNELS),)
	-$(Q)$(CLEAN) $(call PATH_CONV,$(KERNELS))
endif

$(ODIR)/%.obj: %.c $(KERNELS)
	@echo Compiling C $<
	$(Q)$(CC) $(CFLAGS) $(call PATH_CONV,$<) /Fo$(call PATH_CONV,$@)

$(ODIR)/%.obj: %.cpp $(KERNELS)
	@echo Compiling C++ $<
	$(Q)$(CP) $(CFLAGS) $(call PATH_CONV,$<) /Fo$(call PATH_CONV,$@)  $(LOGGING)

$(ODIR)/%.obj: %.S
	@echo Assembling $<
	$(Q)$(AS) $(AFLAGS) $(call PATH_CONV,$<) /Fo$(call PATH_CONV,$@)  $(LOGGING)

%.h: $(KDIR)/%.cl
	@echo Compiling OpenCL Kernel $<
	-$(Q)$(CL) -n -f $< -d 1 -h $@ -W "$(DEFINES) $(INCLUDES)"

$(ODIR)/%.clopt: Makefile
	@echo Building local options file $@ for building OpenCL kernel dependencies.
	$(Q)echo $(DEFINES) $(INCLUDES) > $@

info:
	@echo HOST_OS=$(OS)
	@echo HOST_COMPILER=$(HOST_COMPILER)
	@echo HOST_CPU=$(HOST_CPU)
	@echo HOST_VARIANT=$(HOST_VARIANT)
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

