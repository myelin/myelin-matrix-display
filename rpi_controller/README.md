Code to run on the Raspberry Pi lives here.

- stop_daughterboard: this brings the ATMEGA328 based daughterboard
  into reset.

- start_daughterboard / release_all_ports: this tristates all relevant
  GPIOs (17, 18, 23, 24, 25) and the SPI port (10-MOSI, 9-MISO,
  11-SCLK, 7-CE1), to bring the daughterboard out of reset.

- output: this sets a single GPIO as an output and sets its value.

- tristate: this tristates a single GPIO.
