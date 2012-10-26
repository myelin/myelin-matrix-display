UNAME := $(shell uname)

# This Makefile builds the code for the daughterboard used in the
# Raspberry Pi based matrix.  This has a slightly different pinout
# from the Etherten version, which is selected with -DMATRIX_V2.

ifeq ($(UNAME), Darwin)
  ARDUINO_DIR = /Applications/Arduino.app/Contents/Resources/Java
  AVR_TOOLS_PATH = $(ARDUINO_DIR)/hardware/tools/avr/bin
  ARD_PARSE_BOARDS = ./ard-parse-boards
  AVRDUDE = ssh pi sudo avrdude
endif
ifeq ($(UNAME), Linux)
  ARDUINO_DIR = /usr/share/arduino
  AVRDUDE_USE_SUDO = 1
endif
TARGET = arduino_matrix_driver
ARDUINO_LIBS = SPI
BOARD_TAG = pro328
EXTRACPPFLAGS = -DMATRIX_V2
ISP_PROG = -c myelin
AVRDUDE_VERBOSE = 1
#OPTIMIZATION = -O2
include ../misc/Arduino.mk

disassemble: $(TARGET_ELF)
	$(AVR_TOOLS_PATH)/avr-objdump -S $(TARGET_ELF)

push: $(TARGET_HEX)
	scp $(TARGET_HEX) pi:$(OBJDIR)/

preparepi:
	ssh pi sudo /etc/init.d/myelin_matrix_display stop

install: push preparepi ispload
