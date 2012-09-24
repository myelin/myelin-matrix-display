CFILES=main.c
PROJECTS=fade white epilepsy bounce chill strobe chase sweep test rainbow
DEPS=matrix.h

all: $(PROJECTS)

clean:
	rm -rf $(PROJECTS) *~ *.dSYM

$(PROJECTS): %: %.c $(CFILES) $(DEPS)
	g++ -Wall -O2 -g -o $@ $< $(CFILES)
