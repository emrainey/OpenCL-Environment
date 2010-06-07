ifndef LOCAL_ROOT
LOCAL_ROOT=$(shell pwd)
endif

DIRS=environment compiler query imgfilter yuv pid nbody

all %:
	@(for d in $(DIRS); do\
		cd sources/$$d;\
		$(MAKE) $@ LOCAL_ROOT=$(LOCAL_ROOT);\
		cd ../..;\
	done)

debug:
	@(for d in $(DIRS); do\
		cd sources/$$d;\
		$(MAKE) rebuild DEBUG=1 CL_DEBUG=1;\
		cd ../..;\
	done)

error:
	@echo DIRS=$(DIRS)
	@echo SHELL=$(SHELL)
	@echo LOCAL_ROOT=$(LOCAL_ROOT)

todo:
	@fgrep -HnR -e TODO sources/ README

