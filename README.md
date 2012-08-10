Want It! matrix display
=======================

Copyright (C) 2012 Phillip Pearson

You may use this code under a CC Attribution license.

Introduction
------------

The Want It! matrix display is a 25x12 LED matrix, built for Camp Want
It! at Burning Man 2012 (9:15 and Esplanade).  It consists of 300
WS2801-controlled 5050 LEDs, mounted on a roughly 6' x 3' board.

An Arduino Duemilanove, running the code in the arduino_matrix_driver
folder, is mounted on the board, and is programmed with a number of
simple lighting programs, which will run if nothing is connected.

Connecting a laptop running the code in the linux_daemon folder and
sending video frames via the USB serial port will override this,
allowing more complex patterns.

For extra flexibility, the linux_daemon code also supports the UDP
protocol used by Disorient's Art Car Wash code, allowing a Processing
app, or any of the targets in the acw_c_client folder, to control the
system anywhere on the network.

Programming patterns
--------------------

If you'd like to program patterns for the display, the easiest way is
to use the acw / acw_emulator code.  Clone the
[acw](https://github.com/myelin/acw) and
[acw_emulator](https://github.com/myelin/acw_emulator) repositories
and install all the required dependencies, then run both, and you
should see the default animation show in the acw_emulator window.