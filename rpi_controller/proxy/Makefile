.PHONY: default build

default: build

build:
	g++ -Wall -o matrix_proxy main.c rf24.c udp.c -lwiringPi

test: push
	ssh pi 'sudo killall matrix_proxy; sudo rpi_controller/release_all_ports; cd matrix_proxy && make -C WiringPi/wiringPi && sudo make -C WiringPi/wiringPi install && make && sudo ./matrix_proxy'
#gdb -ex run ./matrix_proxy

freerun: push
	ssh pi 'sudo killall matrix_proxy; cd matrix_proxy && make && sudo ./matrix_proxy </dev/null >/dev/null 2>/dev/null &'

push:
	rsync -vrzu --exclude=.git --exclude=*~ Makefile *.c *.h WiringPi pi:matrix_proxy/

reset:
	ssh pi 'sudo rpi_controller/release_all_ports'
