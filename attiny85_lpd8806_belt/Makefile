TARGET=beltdriver
OFILES=main.o ../attiny85_ws2801/ws2801.o effects.o
BIN=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin
DBIN=../misc

.PHONY: build program default clean

default: build

clean:
	rm -f $(OFILES) $(TARGET).out $(TARGET).hex

$(OFILES): %.o: %.cpp
	$(BIN)/avr-g++ -Wall -I. -I../attiny85_ws2801 -DLPD8806 -DF_CPU=8000000UL -mmcu=attiny85 -Os -o $@ -c $<

build: $(OFILES)
	$(BIN)/avr-gcc -Os -Wl,--gc-sections -mmcu=attiny85 -o $(TARGET).out $(OFILES) -lm -lc
	$(BIN)/avr-objcopy -O ihex -R .eeprom $(TARGET).out $(TARGET).hex

program: build
	-sudo kextunload /System/Library/Extensions/FTDIUSBSerialDriver.kext
	avrdude -c ftdi -P ft0 -v -e -p attiny85 -B 1 -Uefuse:w:0xFF:m -Uhfuse:w:0xDA:m -Ulfuse:w:0xF2:m
	avrdude -c ftdi -P ft0 -v -e -p attiny85 -Uflash:w:$(TARGET).hex
	sudo kextload /System/Library/Extensions/FTDIUSBSerialDriver.kext
