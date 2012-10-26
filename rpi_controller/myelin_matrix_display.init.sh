#!/bin/bash

stop_matrix() {
	killall matrix_proxy
	killall ripples
	killall bounce
	killall chase
	killall epilepsy
	killall strobe
	killall rainbow
	kill `ps auxf | grep 'python rpi_controller/supervisor' | grep -v grep | awk '{print $2}'`
	/home/pi/rpi_controller/restart_daughterboard
}

start_matrix() {
	rm /var/run/matrix_proxy/cmd
	/usr/sbin/matrix_proxy </dev/null 2>/dev/null >/dev/null &
	su - pi -c 'python rpi_controller/supervisor.py' </dev/null 2>/dev/null >/dev/null &
}

case $1 in
    start)
	stop_matrix
	start_matrix
	;;

    stop)
	stop_matrix
	;;
esac
