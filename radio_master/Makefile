# This Makefile builds the code for the (MX2 board based) remote
# control for atmega328_radio_slave boards.

SLOW_REMOTE = 1

TARGET = radio_master
ARDUINO_LIBS = SPI
ISP_PROG = -c ftdi
#OPTIMIZATION = -O2

# clock is prescaled by 8 to save power, so we have to override F_CPU too
ifdef SLOW_REMOTE
  EXTRACPPFLAGS = -DSLOW_REMOTE
  F_CPU=1000000L
else
  ARDUINO_LIBS += SoftwareSerial
endif

include ../misc/Makefile.m328p
