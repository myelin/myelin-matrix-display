Arduino code for Myelin Matrix Display
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

Matrix display V2 has a similar "snake" pattern, but runs vertically
instead of horizontally.

Port usage
----------

- Communication with Linux box: serial (TX/RX)

- Ethernet (UDP)

- LED output (D2 and D4 on mx V1, C1/data and C2/clock on mx V2)
