CFILES=main.c
PROJECTS=fade white epilepsy bounce chill
DEPS=

all: $(PROJECTS)

clean:
	rm -v $(PROJECTS)

$(PROJECTS): %: %.c $(CFILES) $(DEPS)
	g++ -Wall -O2 -g -o $@ $< $(CFILES)

test: acw_client
	./acw_client
