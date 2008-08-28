MAKEFLAGS=-r --no-print-directory

SUBDIRS=$(sort $(dir $(wildcard */)))
CLEANDIRS=$(patsubst %/,%.clean,$(SUBDIRS))
DEPCLEANDIRS=$(patsubst %/,%.depclean,$(SUBDIRS))
OBJFILES=$(patsubst %.cpp,%.obj,$(wildcard *.cpp))
OBJDEPS=$(patsubst %.cpp,%.objdep,$(wildcard *.cpp))
BINFILES=$(patsubst %.cc,%.bin,$(wildcard *.cc))
BINDEPS=$(patsubst %.cc,%.bdeps,$(wildcard *.cc))

.PHONY : Always

all :
	@$(foreach file, $(SUBDIRS) $(OBJFILES) $(BINFILES), $(MAKE) $(file) && ) echo ''

%.obj : Always
	@$(MAKE) -C ../ $(notdir $(CURDIR))/$@

%.bin	: Always
	@$(MAKE) -C ../ $(notdir $(CURDIR))/$@

%.dobj : Always
	@$(MAKE) -C ../ $(notdir $(CURDIR))/$@

%.dbin : Always
	@$(MAKE) -C ../ $(notdir $(CURDIR))/$@

%.dd : Always
	@$(MAKE) -C ../ $(notdir $(CURDIR))/$@

clean:	$(CLEANDIRS)
	@echo 'Cleaning $(notdir $(CURDIR))'
	@rm -f *.obj
	@rm -f *.bin
	@rm -f *.dobj *.dd *.dbin

%.clean	:
	@$(MAKE) -C $* clean

.PHONY	:	$(SUBDIRS)
$(SUBDIRS)	:	;
	$(MAKE) -C $@

