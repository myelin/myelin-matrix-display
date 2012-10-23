EESchema Schematic File Version 2  date 10/2/2012 10:27:55 PM
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:CD40109
LIBS:nRF24L01+
LIBS:led-kicad-cache
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title ""
Date "3 oct 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 6300 1750 0    60   Input ~ 0
3v3
Text GLabel 6300 1250 0    60   Input ~ 0
3v3
$Comp
L CONN_1 P16
U 1 1 506BB768
P 6850 4800
F 0 "P16" H 6930 4800 40  0000 L CNN
F 1 "CONN_1" H 6850 4855 30  0001 C CNN
	1    6850 4800
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P12
U 1 1 506BB71F
P 6850 4900
F 0 "P12" H 6930 4900 40  0000 L CNN
F 1 "CONN_1" H 6850 4955 30  0001 C CNN
	1    6850 4900
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P14
U 1 1 506BB714
P 6850 5100
F 0 "P14" H 6930 5100 40  0000 L CNN
F 1 "CONN_1" H 6850 5155 30  0001 C CNN
	1    6850 5100
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P13
U 1 1 506BB710
P 6850 5000
F 0 "P13" H 6930 5000 40  0000 L CNN
F 1 "CONN_1" H 6850 5055 30  0001 C CNN
	1    6850 5000
	1    0    0    -1  
$EndComp
Text GLabel 1850 1950 0    60   Input ~ 0
led_clk_5v
Text GLabel 1850 1850 0    60   Input ~ 0
led_data_5v
Text GLabel 1850 1650 0    60   Input ~ 0
gnd
$Comp
L CONN_1 P8
U 1 1 5067996C
P 2000 1650
F 0 "P8" H 2080 1650 40  0000 L CNN
F 1 "CONN_1" H 2000 1705 30  0001 C CNN
	1    2000 1650
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P10
U 1 1 50679968
P 2000 1850
F 0 "P10" H 2080 1850 40  0000 L CNN
F 1 "CONN_1" H 2000 1905 30  0001 C CNN
	1    2000 1850
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P11
U 1 1 50679962
P 2000 1950
F 0 "P11" H 2080 1950 40  0000 L CNN
F 1 "CONN_1" H 2000 2005 30  0001 C CNN
	1    2000 1950
	1    0    0    -1  
$EndComp
Text GLabel 2800 1800 0    60   Input ~ 0
kpad_7
Text GLabel 2800 1700 0    60   Input ~ 0
kpad_6
Text GLabel 2800 1600 0    60   Input ~ 0
kpad_5
Text GLabel 2800 1500 0    60   Input ~ 0
kpad_4
Text GLabel 2800 1400 0    60   Input ~ 0
kpad_3
Text GLabel 2800 1300 0    60   Input ~ 0
kpad_2
Text GLabel 2800 1200 0    60   Input ~ 0
kpad_1
Text GLabel 6700 4700 2    60   Input ~ 0
kpad_7
Text GLabel 6700 4600 2    60   Input ~ 0
kpad_6
Text GLabel 6700 4500 2    60   Input ~ 0
kpad_5
Text GLabel 6700 4400 2    60   Input ~ 0
kpad_4
Text GLabel 6700 3950 2    60   Input ~ 0
kpad_3
Text GLabel 6700 4050 2    60   Input ~ 0
kpad_2
Text GLabel 6700 4150 2    60   Input ~ 0
kpad_1
$Comp
L CONN_7 P7
U 1 1 50675694
P 3150 1500
F 0 "P7" V 3120 1500 60  0000 C CNN
F 1 "keypad" V 3220 1500 60  0000 C CNN
	1    3150 1500
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P1
U 1 1 50675421
P 2000 2250
F 0 "P1" H 2080 2250 40  0000 L CNN
F 1 "CONN_1" H 2000 2305 30  0001 C CNN
	1    2000 2250
	1    0    0    -1  
$EndComp
Text GLabel 1850 2250 0    60   Input ~ 0
led_clk_3v
Text GLabel 1850 2350 0    60   Input ~ 0
led_data_3v
Text GLabel 1850 2450 0    60   Input ~ 0
rf_csn
Text GLabel 1850 2550 0    60   Input ~ 0
rf_irq
Text GLabel 1850 2650 0    60   Input ~ 0
rf_ce
Text GLabel 1850 2750 0    60   Input ~ 0
5v
$Comp
L CONN_1 P2
U 1 1 506753C3
P 2000 2350
F 0 "P2" H 2080 2350 40  0000 L CNN
F 1 "CONN_1" H 2000 2405 30  0001 C CNN
	1    2000 2350
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P3
U 1 1 506753C2
P 2000 2450
F 0 "P3" H 2080 2450 40  0000 L CNN
F 1 "CONN_1" H 2000 2505 30  0001 C CNN
	1    2000 2450
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P4
U 1 1 506753C0
P 2000 2550
F 0 "P4" H 2080 2550 40  0000 L CNN
F 1 "CONN_1" H 2000 2605 30  0001 C CNN
	1    2000 2550
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P5
U 1 1 506753BC
P 2000 2650
F 0 "P5" H 2080 2650 40  0000 L CNN
F 1 "CONN_1" H 2000 2705 30  0001 C CNN
	1    2000 2650
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P6
U 1 1 50675378
P 2000 2750
F 0 "P6" H 2080 2750 40  0000 L CNN
F 1 "CONN_1" H 2000 2805 30  0001 C CNN
	1    2000 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 3100 1875 3100
Wire Wire Line
	2200 3300 1875 3300
Wire Wire Line
	1300 3200 1625 3200
Connection ~ 7750 3700
Wire Wire Line
	7850 3700 7500 3700
Wire Wire Line
	7500 3700 7500 3500
Wire Wire Line
	7500 3500 6700 3500
Wire Wire Line
	6700 3400 7500 3400
Wire Wire Line
	7500 3400 7500 3100
Wire Wire Line
	7500 3100 7850 3100
Connection ~ 7750 3100
Wire Wire Line
	1300 3100 1625 3100
Wire Wire Line
	1625 3300 1300 3300
Wire Wire Line
	1875 3200 2200 3200
Wire Wire Line
	7875 4250 6700 4250
Text GLabel 7875 4250 3    60   Input ~ 0
avr_reset
Text GLabel 1300 3100 0    60   Input ~ 0
miso
Text GLabel 1300 3200 0    60   Input ~ 0
sck
Text GLabel 1300 3300 0    60   Input ~ 0
avr_reset
Text GLabel 2200 3300 2    60   Input ~ 0
gnd
Text GLabel 2200 3200 2    60   Input ~ 0
mosi
Text GLabel 2200 3100 2    60   Input ~ 0
3v3
$Comp
L AVR-ISP-6 CON1
U 1 1 50673A36
P 1750 3200
F 0 "CON1" H 1670 3440 50  0000 C CNN
F 1 "AVR-ISP-6" H 1510 2970 50  0000 L BNN
F 2 "AVR-ISP-6" V 1230 3240 50  0001 C CNN
	1    1750 3200
	1    0    0    -1  
$EndComp
Text GLabel 4250 3250 1    60   Input ~ 0
3v3
Text GLabel 4250 3650 3    60   Input ~ 0
gnd
$Comp
L C C4
U 1 1 5067365D
P 4250 3450
F 0 "C4" H 4300 3550 50  0000 L CNN
F 1 "100n" H 4300 3350 50  0000 L CNN
	1    4250 3450
	-1   0    0    1   
$EndComp
$Comp
L C C5
U 1 1 50673647
P 5450 1450
F 0 "C5" H 5500 1550 50  0000 L CNN
F 1 "100n" H 5500 1350 50  0000 L CNN
	1    5450 1450
	-1   0    0    1   
$EndComp
Text GLabel 5450 1650 3    60   Input ~ 0
gnd
Text GLabel 5450 1250 1    60   Input ~ 0
3v3
Text GLabel 8750 2550 2    60   Input ~ 0
gnd
$Comp
L LED D1
U 1 1 5067360A
P 8550 2550
F 0 "D1" H 8550 2650 50  0000 C CNN
F 1 "LED" H 8550 2450 50  0000 C CNN
	1    8550 2550
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 506735FF
P 8100 2550
F 0 "R2" V 8180 2550 50  0000 C CNN
F 1 "R" V 8100 2550 50  0000 C CNN
	1    8100 2550
	0    -1   -1   0   
$EndComp
Text GLabel 7850 2550 0    60   Input ~ 0
sck
Text GLabel 6700 3850 2    60   Input ~ 0
led_clk_3v
Text GLabel 6700 3750 2    60   Input ~ 0
led_data_3v
Text GLabel 6700 2900 2    60   Input ~ 0
rf_irq
Text GLabel 6700 3000 2    60   Input ~ 0
rf_csn
Text GLabel 6700 2800 2    60   Input ~ 0
rf_ce
Text GLabel 1650 5000 0    60   Input ~ 0
gnd
Text GLabel 1650 5150 0    60   Input ~ 0
3v3
Text GLabel 1650 5300 0    60   Input ~ 0
rf_ce
Text GLabel 1650 5450 0    60   Input ~ 0
rf_csn
Text GLabel 3150 5450 2    60   Input ~ 0
rf_irq
Text GLabel 3150 5300 2    60   Input ~ 0
miso
Text GLabel 3150 5150 2    60   Input ~ 0
mosi
Text GLabel 3150 5000 2    60   Input ~ 0
sck
$Comp
L NRF24L01+ U1
U 1 1 506732D4
P 2400 5200
F 0 "U1" H 2400 5100 50  0000 C CNN
F 1 "NRF24L01+" H 2400 5300 50  0000 C CNN
F 2 "MODULE" H 2400 5200 50  0001 C CNN
F 3 "DOCUMENTATION" H 2400 5200 50  0001 C CNN
	1    2400 5200
	1    0    0    -1  
$EndComp
Text GLabel 7800 1750 2    60   Input ~ 0
gnd
Text GLabel 7800 1350 2    60   Input ~ 0
gnd
Text GLabel 7800 1250 2    60   Input ~ 0
gnd
Text GLabel 7800 1850 2    60   Input ~ 0
gnd
Text GLabel 6300 1650 0    60   Input ~ 0
led_clk_3v
Text GLabel 6300 1550 0    60   Input ~ 0
led_clk_5v
Text GLabel 6300 1450 0    60   Input ~ 0
led_data_5v
Text GLabel 6300 1350 0    60   Input ~ 0
led_data_3v
Text GLabel 6300 1850 0    60   Input ~ 0
gnd
Text GLabel 6300 1150 0    60   Input ~ 0
3v3
Text GLabel 7800 1150 2    60   Input ~ 0
5v
$Comp
L CD40109 U2
U 1 1 506630CC
P 7050 1500
F 0 "U2" H 7050 1400 50  0000 C CNN
F 1 "CD40109" H 7050 1600 50  0000 C CNN
F 2 "MODULE" H 7050 1500 50  0001 C CNN
F 3 "DOCUMENTATION" H 7050 1500 50  0001 C CNN
	1    7050 1500
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 50662270
P 8050 3700
F 0 "C2" H 8100 3800 50  0000 L CNN
F 1 "22p" H 8100 3600 50  0000 L CNN
	1    8050 3700
	0    -1   -1   0   
$EndComp
$Comp
L C C1
U 1 1 50662268
P 8050 3100
F 0 "C1" H 8100 3200 50  0000 L CNN
F 1 "22p" H 8100 3000 50  0000 L CNN
	1    8050 3100
	0    -1   -1   0   
$EndComp
Text GLabel 8250 3700 2    60   Input ~ 0
gnd
Text GLabel 8250 3100 2    60   Input ~ 0
gnd
$Comp
L CRYSTAL X1
U 1 1 506621DE
P 7750 3400
F 0 "X1" H 7750 3550 60  0000 C CNN
F 1 "8MHz" H 7750 3250 60  0000 C CNN
	1    7750 3400
	0    1    1    0   
$EndComp
Text GLabel 6700 3300 2    60   Input ~ 0
sck
Text GLabel 6700 3200 2    60   Input ~ 0
miso
Text GLabel 6700 3100 2    60   Input ~ 0
mosi
Text GLabel 4800 5100 0    60   Input ~ 0
gnd
Text GLabel 4800 5000 0    60   Input ~ 0
gnd
Text GLabel 4800 3100 0    60   Input ~ 0
3v3
Text GLabel 4800 2800 0    60   Input ~ 0
3v3
Text GLabel 8375 4250 2    60   Input ~ 0
3v3
$Comp
L R R1
U 1 1 50662128
P 8125 4250
F 0 "R1" V 8205 4250 50  0000 C CNN
F 1 "R" V 8125 4250 50  0000 C CNN
	1    8125 4250
	0    1    1    0   
$EndComp
$Comp
L ATMEGA328P-P IC1
U 1 1 50662096
P 5700 3900
F 0 "IC1" H 5000 5150 50  0000 L BNN
F 1 "ATMEGA328P-P" H 5900 2500 50  0000 L BNN
F 2 "DIL28" H 5100 2550 50  0001 C CNN
	1    5700 3900
	1    0    0    -1  
$EndComp
$EndSCHEMATC
