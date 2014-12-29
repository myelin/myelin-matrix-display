MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv6l)
  PLAT = -DPLAT_RPI
else
  PLAT = -DPLAT_DESKTOP
endif
CFLAGS = -Wall -O2 $(PLAT)

CFILES = main.cpp graphics.cpp matrix.cpp bbc_font.cpp
PROJECTS = white epilepsy bounce strobe chase rainbow black twinkle streamers lunacy ripples fade chill test life sweep sea_of_dreams
DISABLED = 
DEPS = matrix.h bbc_font.h bbc_font_data.h

bbc_font_data.h:
	echo "bbc_font_data.h not included in public git repository to keep everything safe to distribute"
	exit 1

all: $(PROJECTS) $(SPECIALS)

clean:
	rm -rf $(PROJECTS) $(SPECIALS) *~ *.dSYM

$(PROJECTS): %: %.cpp $(CFILES) $(DEPS)
	python preprocess.py $<
	g++ $(CFLAGS) -g -o $@ $< $(CFILES)

push:
	make clean
	rsync -vrz . pi:c_client/
	ssh pi 'cd c_client && make'