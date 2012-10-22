# This Makefile builds the code for the daughterboard used in the
# Raspberry Pi based matrix.  This has a slightly different pinout
# from the Etherten version, which is selected with -DMATRIX_V2.

ARDUINO_DIR = /usr/share/arduino
TARGET = arduino_matrix_driver
ARDUINO_LIBS = SPI
BOARD_TAG = pro328
EXTRACPPFLAGS = -DMATRIX_V2
ISP_PROG = -c myelin
AVRDUDE_VERBOSE = 1
AVRDUDE_USE_SUDO = 1
include Arduino.mk
