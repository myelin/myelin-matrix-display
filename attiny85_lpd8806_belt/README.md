ATTINY85 LED belt driver
========================

This is the code for my LED belt for Burning Man -- similar to the [Adafruit digital programmable LED belt](http://adafruit.com/products/332), except using [this WS2801 strip from Aliexpress/Shenzhen Rita Lighting](http://us.aliexpress.com/snapshot/110553435.html) and an ATTINY85.

* Install the patched version of avrdude: `(cd ../avrdude; sudo make install)`
* Connect a SparkFun FTDI Basic Breakout board to your ATTINY85 (TX=MOSI, RX=MISO, SCK=CTS, RESET=DTR)
* Run: `make program`
