Myelin Matrix Display
=====================

http://mx.myel.in/

Copyright (C) 2012-2016 Phillip Pearson

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Introduction
------------

The [Myelin Matrix Display](http://www.myelin.co.nz/matrix-display/)
is a 25x12 LED matrix, that I built as my project for Burning Man
2012.  I installed it at my camp, Want It! (9:15 and Esplanade).  It
consists of 300 WS2801-controlled 5050 LEDs, mounted on a roughly 6' x
3' board.

It supports the UDP protocol used by Disorient's Art Car Wash code,
allowing a Processing app, or any of the targets in the c_client
folder, to control the system from anywhere on the network.

Current version (2)
-------------------

A Raspberry Pi B (the original version, with 256 MB of RAM) is
connected via an MX2 board (see MX2_remote_control_pcb for schematics
and layout) to an nRF24L01+ radio module and to the WS2801 string.

On powerup, the ATMEGA328P on the MX2 board runs an internal
animation.  Once the RPi boots, it takes over, resetting the AVR and
then setting /SS low, to signal it to release the LED output lines.
The RPi then outputs directly to the LEDs.

The code in rpi_controller/proxy talks to the radio module and
receives display frames via UDP, passing them on to the LED string.

Animation code lives in c_client.  In production, this sends frames to
the local UDP receiver, but it can run on another host and send frames
to an RPi elsewhere on the network.

Old versions
------------

Version 1 had a Freetronics Etherten, running the code in the
arduino_matrix_driver folder -- doing everything the AVR on the MX2
board does, plus listening on UDP on its builtin ethernet port, and
also listening on its serial port.

Connecting a laptop running the code in the linux_daemon folder and
sending video frames via the USB serial port will override this,
allowing more complex patterns.

Programming patterns
--------------------

If you'd like to program patterns for the display:

- Open and run matrix_simulator/matrix_simulator.pde with Processing

- cd python_client; python matrix.py

- Mess with the examples in python_client and c_client
