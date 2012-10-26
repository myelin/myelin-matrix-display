#!/bin/bash

case $1 in
    start)
	/usr/sbin/matrix_proxy </dev/null 2>/dev/null >/dev/null &
	;;

    stop)
	killall matrix_proxy
	/home/pi/rpi_controller/restart_daughterboard
	;;
esac
