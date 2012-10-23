.PHONY: default build

default: build

build:
	g++ -g -Wall -o rf24 rf24.c udp.c -lwiringPi

test: push
	ssh pi 'sudo killall rf24; sudo rpi_controller/release_all_ports; cd pi_rf24 && make -C WiringPi/wiringPi && sudo make -C WiringPi/wiringPi install && make && sudo gdb -ex run ./rf24'

push:
	rsync -vrzu --exclude=.git --exclude=*~ Makefile *.c *.h WiringPi pi:pi_rf24/

reset:
	ssh pi 'sudo rpi_controller/release_all_ports'
