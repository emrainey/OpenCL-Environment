
KDIR?=$(HOST_ROOT)/source/kernels

ifeq ($(HOST_OS),Windows_NT)
PATH_CONV=$(subst /,\,$(1))
else
PATH_CONV=$(subst \,/,$(1))
endif
P2W_CONV=$(patsubst \cygdrive\c\%,c:\%,$(subst /,\,$(1)))
W2P_CONV=$(subst \,/,$(patsubst C:\%,\cygdrive\c\% $(1)))

ifdef CL_DEBUG
	DEFS+=CL_DEBUG
endif

ifndef CL_DEVICE_TYPE
	CL_DEVICE_TYPE=all
endif

ifndef CL_DEVICE_COUNT
	CL_DEVICE_COUNT=1
endif

IDIRS += $(HOST_ROOT)/source/kernels
DEFS += CL_BUILD_RUNTIME
CL_USER_DEVICE_COUNT=1
ifeq ($(HOST_OS),Windows_NT)
	CL_USER_DEVICE_TYPE=gpu
	SYSIDIRS += $(OPENCL_ROOT)/inc
	ifeq ($(HOST_CPU),X86)
		SYSLDIRS += $(OPENCL_ROOT)/lib/Win32
	else ifeq ($(HOST_CPU),X64)
		SYSLDIRS += $(OPENCL_ROOT)/lib/x64
	endif
	SYS_SHARED_LIBS += OpenCL
else ifeq ($(HOST_OS),LINUX)
	CL_USER_DEVICE_TYPE=gpu
	SYSDIRS += $(OPENCL_ROOT)/inc
	ifeq ($(HOST_CPU),X86)
		SYSLDIRS += $(OPENCL_ROOT)/lib/Linux32
	else ifeq ($(HOST_CPU),X64)
		SYSLDIRS += $(OPENCL_ROOT)/lib/Linux64
	endif
	SYS_SHARED_LIBS += OpenCL
else ifeq ($(HOST_OS),DARWIN)
	CL_USER_DEVICE_TYPE=cpu
	IDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/OpenCL.framework/Headers
	IDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/OpenGL.framework/Headers
	IDIRS+=/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/GLUT.framework/Headers
	LDFLAGS+=-framework OpenCL -framework OpenGL -framework GLUT
	CFLAGS+=-fPIC
	DEFS+=__APPLE__
	ifdef CL_DEBUG
		DEFS+=VECLIB DEBUG _GLIBCXX_DEBUG=1 _GLIBCXX_DEBUG_PEDANTIC=1
	endif
endif

# OpenCL-Environment Compiler Support
ifdef DEBUG
CL=$(HOST_ROOT)/out/$(TARGET_CPU)/debug/clcompiler.exe
else
CL=$(HOST_ROOT)/out/$(TARGET_CPU)/release/clcompiler.exe
endif

ifeq ($(HOST_OS),CYGWIN)
DEFS+=KDIR="\"$(KDIR)\"" CL_USER_DEVICE_COUNT=$(CL_USER_DEVICE_COUNT) CL_USER_DEVICE_TYPE="\"$(CL_USER_DEVICE_TYPE)\""
else
DEFS+=KDIR="$(KDIR)" CL_USER_DEVICE_COUNT=$(CL_USER_DEVICE_COUNT) CL_USER_DEVICE_TYPE="$(CL_USER_DEVICE_TYPE)"
endif

ifdef CL_BUILD_RUNTIME
DEFS+=CL_BUILD_RUNTIME
else ifdef CLSOURCES
KERNELS=$(CLSOURCES:%.cl=$(_MODPATH)/%.h)
KOPTIONS=$(CLSOURCES:%.cl=$(_MODPATH)/%.clopt)
ifeq ($(HOST_OS),CYGWIN)
# The Clang/LLVM is a Windows Path Compiler
KFLAGS+=$(foreach inc,$(KIDIRS),-I$(call P2W_CONV,$(inc))) $(foreach def,$(KDEFS),-D$(def))
else
KFLAGS+=$(foreach inc,$(KIDIRS),-I$(inc)) $(foreach def,$(KDEFS),-D$(def))
endif
endif

ifdef KERNELS
build:: kernels

kernels:: $(foreach kern, $(KERNELS), $(_MODPATH)/$(kern))

%.h: $(KDIR)/%.cl $(CL)
	@echo [PURE] Compiling OpenCL Kernel $<
	-$(Q)$(CL) -n -f $< -d $(CL_USER_DEVICE_COUNT) -t $(CL_USER_DEVICE_TYPE) -h $@ -W "$(KFLAGS)"
endef
