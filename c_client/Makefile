MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv6l)
  PLAT = -DPLAT_RPI
else
  PLAT = -DPLAT_DESKTOP
endif
CFLAGS = -Wall -O2 $(PLAT)

CFILES = main.c
PROJECTS = white epilepsy bounce strobe chase rainbow black twinkle streamers
DISABLED = fade chill test life sweep sweep
SPECIALS = ripples
DEPS = matrix.h

all: $(PROJECTS) $(SPECIALS)

clean:
	rm -rf $(PROJECTS) $(SPECIALS) *~ *.dSYM

ripples: ripples.c $(CFILES) $(DEPS)
	g++ $(CFLAGS) -o ripples -DOVERSAMPLING=4 main.c ripples.c

$(PROJECTS): %: %.c $(CFILES) $(DEPS)
	g++ $(CFLAGS) -g -o $@ $< $(CFILES)

push:
	make clean
	rsync -vrz . pi:c_client/
	ssh pi 'cd c_client && make'