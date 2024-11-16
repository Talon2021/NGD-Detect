#!/usr/bin/make -f

MODS:=minini mahony ip

.PHONY:release clean

RELEASE = cd $(mod) && $(MAKE); cd ..;
CLEAN = cd $(mod) && $(MAKE) -w clean; cd ..;


release:
	$(foreach mod, $(MODS), $(RELEASE))

clean:
	$(foreach mod, $(MODS), $(CLEAN))