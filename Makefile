LOCAL_ROOT?=$(abspath .)
DIRS=math query devicequery environment compiler imgfilter yuv pid nbody spacetime

.PHONY: todo all debug error

all %:
	@(for d in $(DIRS); do\
		cd sources/$$d;\
		$(MAKE) $@ LOCAL_ROOT=$(LOCAL_ROOT);\
		cd ../..;\
	done)

scrub:
	-@rm out/obj/*.*
	-@rm out/*.*
	
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
	@fgrep -HnR -e TODO sources/ include/ TODO README
