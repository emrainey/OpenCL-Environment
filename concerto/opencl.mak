
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
