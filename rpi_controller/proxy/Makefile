.PHONY: default build

default: build

build:
	g++ -Wall -o matrix_proxy main.c rf24.c udp.c -lwiringPi

remote_build:
	ssh pi 'sudo killall matrix_proxy; sudo rpi_controller/release_all_ports; cd matrix_proxy && make -C WiringPi/wiringPi && sudo make -C WiringPi/wiringPi install && make build'

test: push remote_build
	ssh pi sudo matrix_proxy/matrix_proxy

freerun: push
	ssh pi 'sudo killall matrix_proxy; cd matrix_proxy && make && sudo ./matrix_proxy </dev/null >/dev/null 2>/dev/null &'

push:
	rsync -vrzu --exclude=.git --exclude=*~ Makefile *.c *.h WiringPi pi:matrix_proxy/

reset:
	ssh pi 'sudo rpi_controller/release_all_ports'

install: remote_build
	ssh pi 'sudo cp matrix_proxy/matrix_proxy /usr/sbin/'

start:
	ssh pi sudo /etc/init.d/myelin_matrix_display start

stop:
	ssh pi sudo /etc/init.d/myelin_matrix_display stop
