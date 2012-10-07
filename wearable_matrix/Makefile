# we can't actually do the build ourselves; just build in the Arduino IDE and copy the .hex file from /var/tmp.
TARGET=wearable_matrix.cpp
DBIN=../misc

.PHONY: build program default clean

default: build

build: wearable_matrix.cpp.hex

program: build
	-sudo kextunload /System/Library/Extensions/FTDIUSBSerialDriver.kext
	$(DBIN)/avrdude -C $(DBIN)/avrdude.conf -c ftdi -P ft0 -v -e -p m328p -B 1 -Uefuse:w:0x07:m -Uhfuse:w:0xD9:m -Ulfuse:w:0xE2:m
	$(DBIN)/avrdude -C $(DBIN)/avrdude.conf -c ftdi -P ft0 -v -e -p m328p -Uflash:w:$(TARGET).hex
	sudo kextload /System/Library/Extensions/FTDIUSBSerialDriver.kext
