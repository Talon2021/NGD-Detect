#!/usr/bin/make -f

MODS:=AVL MPU common LOG NET MSG

.PHONY:release clean

RELEASE = cd $(mod) && $(MAKE) || exit 1; cd ..;
CLEAN = cd $(mod) && $(MAKE) -w clean || exit 1; cd ..;


release:
	$(foreach mod, $(MODS), $(RELEASE))

clean:
	$(foreach mod, $(MODS), $(CLEAN))