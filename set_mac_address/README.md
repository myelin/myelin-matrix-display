nRF24L01+ address setter
========================

This sets the receiver address for boards that will run the
atmega328_radio_slave code.

Run random_address.py, add a note to the new address that it adds to
addresses.txt, then run the program to save the address into the first
five bytes of EEPROM in your ATMEGA328.