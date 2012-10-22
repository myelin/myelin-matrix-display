#!/bin/bash

case $1 in
    start)
	#/home/pi/rpi_controller/stop_daughterboard
	;;

    stop)
	/home/pi/rpi_controller/restart_daughterboard
	;;
esac
