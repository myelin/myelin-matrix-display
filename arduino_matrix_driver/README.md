Arduino code for Want It! matrix display
========================================

Main tasks
----------

- Push frames out from video memory to WS2801s

- Programmatically generate frames (if in a programmed mode)

- Read frames from SD card (if in streaming mode)

- Read frames from serial port and write to SD card (intermittently)

Pixel layout
------------

Convenience of wiring means addressing individual pixels is going to
be a little odd.  The Arduino driver has to map raster input into the
actual format.  Some possible formats, where (0,0) is the top
left-hand corner of the display and (24,11) is the bottom right hand
corner, and pixels along the string of lights are numbered [0] -
[299], are:

- Alternate lines reversed.  (0,0) - (24,0) map to [0] - [24], (0,1) -
  (24,1) map to [49] - [25] (backwards), (0,2) - (24,2) map to [50] -
  [74], etc.

- Six 5x6 blocks.  (0,0) - (

Port usage
----------

- Communication with NSLU2: serial (TX/RX)

-