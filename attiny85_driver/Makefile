OFILES=main.o ws2801.o effects.o
BIN=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin
DBIN=../misc

.PHONY: build program default clean

default: build

clean:
	rm -f $(OFILES) tinydriver.out tinydriver.hex

$(OFILES): %.o: %.c
	$(BIN)/avr-g++ -Wall -DF_CPU=8000000UL -mmcu=attiny85 -Os -o $@ -c $<

build: $(OFILES)
	$(BIN)/avr-gcc -Os -Wl,--gc-sections -mmcu=attiny85 -o tinydriver.out $(OFILES) -lm -lc
	$(BIN)/avr-objcopy -O ihex -R .eeprom tinydriver.out tinydriver.hex

program: build
	-sudo kextunload /System/Library/Extensions/FTDIUSBSerialDriver.kext
	avrdude -c ftdi -P ft0 -v -e -p attiny85 -B 1 -Uefuse:w:0xFF:m -Uhfuse:w:0xDA:m -Ulfuse:w:0xF2:m
	avrdude -c ftdi -P ft0 -v -e -p attiny85 -Uflash:w:tinydriver.hex
	sudo kextload /System/Library/Extensions/FTDIUSBSerialDriver.kext
