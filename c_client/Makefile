CFILES=main.c
PROJECTS=fade white epilepsy bounce chill strobe chase sweep test rainbow life black twinkle
SPECIALS=ripples
DEPS=matrix.h

all: $(PROJECTS) $(SPECIALS)

clean:
	rm -rf $(PROJECTS) $(SPECIALS) *~ *.dSYM

ripples: ripples.c $(CFILES) $(DEPS)
	g++ -o ripples -DOVERSAMPLING=4 main.c ripples.c

$(PROJECTS): %: %.c $(CFILES) $(DEPS)
	g++ -Wall -O2 -g -o $@ $< $(CFILES)
