
IDIRS += $(GLUT_ROOT)/include
ifeq ($(HOST_OS),Windows_NT)
	SYSLDIRS += $(GLUT_ROOT)/lib
	SYS_SHARED_LIBS := glut32 glu32
else ifeq ($(HOST_OS),LINUX)
else ifeq ($(HOST_OS),DARWIN)
endif
