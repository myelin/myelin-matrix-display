UDP client for Want It! matrix display
======================================

Copyright (C) 2012 Phillip Pearson

You may use this under a CC Attribution license.

What is this?
-------------

This code enables prototyping patterns in C++ for the Want It! matrix
display, by sending frames over UDP in the [Disorient Art Car
Wash](http://artcarwash.net/) format.

Installation
------------

- Install [Processing](http://processing.org/) and [the Want It! fork of the ACW emulator](https://github.com/myelin/acw_emulator).

- Run make && ./white; you should see all the pixels on the ACW emulator go white.

Adding a new animation
----------------------

- Pick a name (foo) and create a .c file (foo.c) for your new animation.

- Edit Makefile and add your animation to the PROJECTS variable.

- Run make, then run your application's executable (./foo).

- With any luck, it should start sending frames to the acw_emulator.

- Hack and enjoy!