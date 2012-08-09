Arduino code for Want It! matrix display
========================================

Main tasks
----------

- Read frames from serial port

- Push frames out from video memory to WS2801s

- Programmatically generate frames (if none available from the serial
  port -- host computer not connected or not sending frames)

Pixel layout
------------

Convenience of wiring means addressing individual pixels is slightly
odd.  The Want It! matrix display has alternate lines reversed.  (0,0)
- (24,0) map to [0] - [24], (0,1) - (24,1) map to [49] - [25]
(backwards), (0,2) - (24,2) map to [50] - [74], etc.

Port usage
----------

- Communication with Linux box: serial (TX/RX)

TODO
----

- Play videos from microSD card

- Support nRF24L01+ RF comms

- Add IR receiver for remote control

- Support DMX control
