CFILES=matrix_daemon.c loopback_test.c udp.c
DEPS=matrix_daemon.h

matrix_daemon: $(CFILES) $(DEPS)
	g++ -Wall -O2 -g -o $@ $(CFILES)

test: matrix_daemon
	./matrix_daemon
