#!/bin/bash

case $1 in
    start)
	rm /var/run/matrix_proxy/cmd
	/usr/sbin/matrix_proxy </dev/null 2>/dev/null >/dev/null &
	su - pi -c 'python rpi_controller/supervisor.py' </dev/null 2>/dev/null >/dev/null &
	;;

    stop)
	killall matrix_proxy
	killall ripples
	killall bounce
	killall chase
	killall epilepsy
	killall strobe
	killall rainbow
	killall supervisor.py
	/home/pi/rpi_controller/restart_daughterboard
	;;
esac
