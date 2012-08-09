CFILES=matrix_daemon.c loopback_test.c
DEPS=matrix_daemon.h

matrix_daemon: $(CFILES) $(DEPS)
	g++ -g -o $@ $(CFILES)

test: matrix_daemon
	./matrix_daemon
