EESchema Schematic File Version 2
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
LIBS:uhr
LIBS:02 Human-Interface-Board-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Human Interface Board ECCO-Uhr"
Date "7 may 2011"
Rev "1"
Comp "ECCO"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	5700 2350 5700 3000
Wire Wire Line
	1800 3100 2200 3100
Wire Notes Line
	5100 1200 800  1200
Wire Notes Line
	5100 5400 800  5400
Wire Wire Line
	1250 2650 1250 2450
Wire Wire Line
	1400 3750 1400 3650
Wire Wire Line
	1050 1500 1400 1500
Wire Wire Line
	2200 2900 2050 2900
Wire Wire Line
	3100 3100 4700 3100
Wire Wire Line
	3900 4650 3900 5050
Connection ~ 3400 3300
Wire Wire Line
	3900 3650 3900 3300
Wire Wire Line
	3900 3300 3100 3300
Wire Wire Line
	3400 3600 3400 3300
Wire Wire Line
	1400 1500 1400 1850
Wire Notes Line
	7500 4400 5400 4400
Wire Notes Line
	7500 4400 7500 6850
Wire Notes Line
	7500 6850 5400 6850
Wire Notes Line
	5400 6850 5400 4400
Wire Wire Line
	7100 4750 7100 4900
Wire Wire Line
	7100 5400 7100 5700
Connection ~ 5700 1550
Wire Wire Line
	5700 1950 5700 1450
Connection ~ 6900 2600
Wire Wire Line
	6300 2600 6900 2600
Connection ~ 9300 2600
Wire Wire Line
	8700 2600 9300 2600
Wire Wire Line
	6900 3750 6900 3850
Wire Wire Line
	9300 3750 9300 3850
Wire Wire Line
	5700 3900 5700 3500
Wire Wire Line
	10500 2750 10500 2450
Wire Wire Line
	8100 2450 8100 2750
Connection ~ 8100 1550
Wire Wire Line
	8100 1550 8100 1950
Wire Wire Line
	10500 1950 10500 1550
Wire Wire Line
	10500 1550 5700 1550
Wire Notes Line
	10850 5000 7800 5000
Wire Notes Line
	7800 6850 10850 6850
Wire Wire Line
	9250 6450 9250 6350
Wire Wire Line
	10000 6100 10000 6350
Wire Wire Line
	8850 5800 8550 5800
Wire Wire Line
	8850 6000 8550 6000
Wire Wire Line
	10000 6100 9650 6100
Wire Wire Line
	9650 6000 10000 6000
Wire Wire Line
	9650 5800 10000 5800
Wire Wire Line
	9650 5700 10000 5700
Wire Wire Line
	9650 5900 10000 5900
Wire Wire Line
	8850 6100 8550 6100
Wire Wire Line
	8850 5900 8550 5900
Wire Wire Line
	8850 5700 8550 5700
Wire Wire Line
	10000 6350 8550 6350
Wire Wire Line
	8550 6350 8550 6100
Connection ~ 9250 6350
Wire Notes Line
	7800 6850 7800 4400
Wire Notes Line
	7800 4400 10850 4400
Wire Notes Line
	10850 4400 10850 6850
Wire Wire Line
	9300 1550 9300 1950
Connection ~ 9300 1550
Wire Wire Line
	6900 1950 6900 1550
Connection ~ 6900 1550
Wire Wire Line
	6900 2750 6900 2450
Wire Wire Line
	9300 2450 9300 2750
Wire Wire Line
	10500 3750 10500 3850
Wire Wire Line
	8100 3750 8100 3850
Wire Wire Line
	9900 2600 10500 2600
Connection ~ 10500 2600
Wire Wire Line
	7500 2600 8100 2600
Connection ~ 8100 2600
Wire Notes Line
	10900 4100 5400 4100
Wire Notes Line
	5400 1200 10900 1200
Wire Notes Line
	10900 4100 10900 700 
Wire Notes Line
	10900 700  5400 700 
Wire Notes Line
	5400 700  5400 4100
Wire Wire Line
	6500 5550 7100 5550
Connection ~ 7100 5550
Wire Wire Line
	7100 6700 7100 6600
Wire Notes Line
	5400 5000 6550 5000
Wire Notes Line
	6550 5000 6550 4400
Wire Wire Line
	1400 2550 1400 2250
Wire Wire Line
	3400 4100 3400 4350
Wire Wire Line
	3900 4050 3900 4450
Wire Wire Line
	3400 5050 3400 4750
Wire Wire Line
	4700 4050 4700 5050
Wire Wire Line
	2200 3300 2050 3300
Wire Wire Line
	1250 3550 1250 3750
Wire Notes Line
	5100 5400 5100 700 
Wire Notes Line
	5100 700  800  700 
Wire Notes Line
	800  700  800  5400
Wire Wire Line
	4700 3650 4700 2950
Connection ~ 4700 3100
$Comp
L VDD #PWR01
U 1 1 4DC54F52
P 4700 2950
F 0 "#PWR01" H 4700 3050 30  0001 C CNN
F 1 "VDD" H 4700 3060 30  0000 C CNN
F 2 "" H 4700 2950 60  0001 C CNN
F 3 "" H 4700 2950 60  0001 C CNN
	1    4700 2950
	1    0    0    -1  
$EndComp
Text Notes 1500 1000 0    120  ~ 0
Endstufe Lautsprecher
NoConn ~ 3100 2900
NoConn ~ 3100 2700
NoConn ~ 2200 2700
$Comp
L VSS #PWR02
U 1 1 4DC50282
P 1250 2450
F 0 "#PWR02" H 1250 2450 30  0001 C CNN
F 1 "VSS" H 1250 2380 30  0000 C CNN
F 2 "" H 1250 2450 60  0001 C CNN
F 3 "" H 1250 2450 60  0001 C CNN
	1    1250 2450
	-1   0    0    1   
$EndComp
$Comp
L VSS #PWR03
U 1 1 4DC5027E
P 1250 3750
F 0 "#PWR03" H 1250 3750 30  0001 C CNN
F 1 "VSS" H 1250 3680 30  0000 C CNN
F 2 "" H 1250 3750 60  0001 C CNN
F 3 "" H 1250 3750 60  0001 C CNN
	1    1250 3750
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR04
U 1 1 4DC5027B
P 1400 3750
F 0 "#PWR04" H 1400 3750 30  0001 C CNN
F 1 "VSS" H 1400 3680 30  0000 C CNN
F 2 "" H 1400 3750 60  0001 C CNN
F 3 "" H 1400 3750 60  0001 C CNN
	1    1400 3750
	1    0    0    -1  
$EndComp
Text Label 1050 1500 0    60   ~ 0
Speaker
$Comp
L VSS #PWR05
U 1 1 4DC501BC
P 2050 2900
F 0 "#PWR05" H 2050 2900 30  0001 C CNN
F 1 "VSS" H 2050 2830 30  0000 C CNN
F 2 "" H 2050 2900 60  0001 C CNN
F 3 "" H 2050 2900 60  0001 C CNN
	1    2050 2900
	0    1    1    0   
$EndComp
$Comp
L VSS #PWR06
U 1 1 4DC501B7
P 2050 3300
F 0 "#PWR06" H 2050 3300 30  0001 C CNN
F 1 "VSS" H 2050 3230 30  0000 C CNN
F 2 "" H 2050 3300 60  0001 C CNN
F 3 "" H 2050 3300 60  0001 C CNN
	1    2050 3300
	0    1    1    0   
$EndComp
$Comp
L VSS #PWR07
U 1 1 4DC501A6
P 4700 5050
F 0 "#PWR07" H 4700 5050 30  0001 C CNN
F 1 "VSS" H 4700 4980 30  0000 C CNN
F 2 "" H 4700 5050 60  0001 C CNN
F 3 "" H 4700 5050 60  0001 C CNN
	1    4700 5050
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 4DC50190
P 4700 3850
F 0 "C3" H 4900 3800 50  0000 L CNN
F 1 "100n" H 4850 3900 50  0000 L CNN
F 2 "" H 4700 3850 60  0001 C CNN
F 3 "" H 4700 3850 60  0001 C CNN
	1    4700 3850
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR08
U 1 1 4DC50159
P 3900 5050
F 0 "#PWR08" H 3900 5050 30  0001 C CNN
F 1 "VSS" H 3900 4980 30  0000 C CNN
F 2 "" H 3900 5050 60  0001 C CNN
F 3 "" H 3900 5050 60  0001 C CNN
	1    3900 5050
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR09
U 1 1 4DC50156
P 3400 5050
F 0 "#PWR09" H 3400 5050 30  0001 C CNN
F 1 "VSS" H 3400 4980 30  0000 C CNN
F 2 "" H 3400 5050 60  0001 C CNN
F 3 "" H 3400 5050 60  0001 C CNN
	1    3400 5050
	1    0    0    -1  
$EndComp
Text Notes 5550 4750 0    120  ~ 0
Mikrofon
Text Label 6500 5550 0    60   ~ 0
MIC
$Comp
L VSS #PWR010
U 1 1 4DC4EDAA
P 7100 6700
F 0 "#PWR010" H 7100 6700 30  0001 C CNN
F 1 "VSS" H 7100 6630 30  0000 C CNN
F 2 "" H 7100 6700 60  0001 C CNN
F 3 "" H 7100 6700 60  0001 C CNN
	1    7100 6700
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR011
U 1 1 4DC4ED87
P 7100 4750
F 0 "#PWR011" H 7100 4850 30  0001 C CNN
F 1 "VDD" H 7100 4860 30  0000 C CNN
F 2 "" H 7100 4750 60  0001 C CNN
F 3 "" H 7100 4750 60  0001 C CNN
	1    7100 4750
	1    0    0    -1  
$EndComp
Text Notes 5500 950  0    120  ~ 0
Tasten & LED die anzeigt dass mindestens\nein Frequenzdecoder aktiv ist
$Comp
L VDD #PWR012
U 1 1 4DC4EBC2
P 5700 1450
F 0 "#PWR012" H 5700 1550 30  0001 C CNN
F 1 "VDD" H 5700 1560 30  0000 C CNN
F 2 "" H 5700 1450 60  0001 C CNN
F 3 "" H 5700 1450 60  0001 C CNN
	1    5700 1450
	1    0    0    -1  
$EndComp
Text Label 9900 2600 0    60   ~ 0
Taste3
Text Label 8700 2600 0    60   ~ 0
Taste2
Text Label 7500 2600 0    60   ~ 0
Taste1
Text Label 6300 2600 0    60   ~ 0
Taste0
Text Label 5700 3900 1    60   ~ 0
LED_Freq
$Comp
L VSS #PWR013
U 1 1 4DC4E90E
P 10500 3850
F 0 "#PWR013" H 10500 3850 30  0001 C CNN
F 1 "VSS" H 10500 3780 30  0000 C CNN
F 2 "" H 10500 3850 60  0001 C CNN
F 3 "" H 10500 3850 60  0001 C CNN
	1    10500 3850
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR014
U 1 1 4DC4E909
P 9300 3850
F 0 "#PWR014" H 9300 3850 30  0001 C CNN
F 1 "VSS" H 9300 3780 30  0000 C CNN
F 2 "" H 9300 3850 60  0001 C CNN
F 3 "" H 9300 3850 60  0001 C CNN
	1    9300 3850
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR015
U 1 1 4DC4E904
P 8100 3850
F 0 "#PWR015" H 8100 3850 30  0001 C CNN
F 1 "VSS" H 8100 3780 30  0000 C CNN
F 2 "" H 8100 3850 60  0001 C CNN
F 3 "" H 8100 3850 60  0001 C CNN
	1    8100 3850
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR016
U 1 1 4DC4E8FF
P 6900 3850
F 0 "#PWR016" H 6900 3850 30  0001 C CNN
F 1 "VSS" H 6900 3780 30  0000 C CNN
F 2 "" H 6900 3850 60  0001 C CNN
F 3 "" H 6900 3850 60  0001 C CNN
	1    6900 3850
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 4DC4E790
P 8100 2200
F 0 "R3" H 8300 2150 50  0000 C CNN
F 1 "4k7" H 8300 2250 50  0000 C CNN
F 2 "" H 8100 2200 60  0001 C CNN
F 3 "" H 8100 2200 60  0001 C CNN
	1    8100 2200
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 4DC4E78F
P 10500 2200
F 0 "R5" H 10700 2150 50  0000 C CNN
F 1 "4k7" H 10700 2250 50  0000 C CNN
F 2 "" H 10500 2200 60  0001 C CNN
F 3 "" H 10500 2200 60  0001 C CNN
	1    10500 2200
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 4DC4E789
P 9300 2200
F 0 "R4" H 9500 2150 50  0000 C CNN
F 1 "4k7" H 9500 2250 50  0000 C CNN
F 2 "" H 9300 2200 60  0001 C CNN
F 3 "" H 9300 2200 60  0001 C CNN
	1    9300 2200
	1    0    0    -1  
$EndComp
Text Notes 8050 4750 0    120  ~ 0
Verbindung zum Mainboard
$Comp
L VDD #PWR017
U 1 1 4DC4E70A
P 8550 6000
F 0 "#PWR017" H 8550 6100 30  0001 C CNN
F 1 "VDD" H 8550 6110 30  0000 C CNN
F 2 "" H 8550 6000 60  0001 C CNN
F 3 "" H 8550 6000 60  0001 C CNN
	1    8550 6000
	0    -1   -1   0   
$EndComp
$Comp
L VSS #PWR018
U 1 1 4DC4E6F0
P 9250 6450
F 0 "#PWR018" H 9250 6450 30  0001 C CNN
F 1 "VSS" H 9250 6380 30  0000 C CNN
F 2 "" H 9250 6450 60  0001 C CNN
F 3 "" H 9250 6450 60  0001 C CNN
	1    9250 6450
	1    0    0    -1  
$EndComp
Text Label 10000 5700 0    60   ~ 0
Taste2
Text Label 10000 5800 0    60   ~ 0
Taste3
Text Label 10000 5900 0    60   ~ 0
MIC
Text Label 10000 6000 0    60   ~ 0
Speaker
Text Label 8550 5900 2    60   ~ 0
Taste0
Text Label 8550 5800 2    60   ~ 0
Taste1
Text Label 8550 5700 2    60   ~ 0
LED_Freq
$Comp
L CONN_5X2 P2
U 1 1 4DC4E5F9
P 9250 5900
F 0 "P2" H 9250 6200 60  0000 C CNN
F 1 "CONN_5X2" V 9250 5900 50  0000 C CNN
F 2 "" H 9250 5900 60  0001 C CNN
F 3 "" H 9250 5900 60  0001 C CNN
	1    9250 5900
	1    0    0    -1  
$EndComp
$Comp
L LED D1
U 1 1 4DC4E5D0
P 5700 2150
F 0 "D1" H 5700 2250 50  0000 C CNN
F 1 "LED" H 5700 2050 50  0000 C CNN
F 2 "" H 5700 2150 60  0001 C CNN
F 3 "" H 5700 2150 60  0001 C CNN
	1    5700 2150
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 4DC4E5C1
P 6900 2200
F 0 "R2" H 7100 2150 50  0000 C CNN
F 1 "4k7" H 7100 2250 50  0000 C CNN
F 2 "" H 6900 2200 60  0001 C CNN
F 3 "" H 6900 2200 60  0001 C CNN
	1    6900 2200
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 4DC4E5B3
P 5700 3250
F 0 "R1" H 5900 3200 50  0000 C CNN
F 1 "150" H 5900 3300 50  0000 C CNN
F 2 "" H 5700 3250 60  0001 C CNN
F 3 "" H 5700 3250 60  0001 C CNN
	1    5700 3250
	1    0    0    -1  
$EndComp
$Comp
L R R7
U 1 1 4DC4E5AC
P 7100 5150
F 0 "R7" H 6900 5100 50  0000 C CNN
F 1 "10k" H 6900 5200 50  0000 C CNN
F 2 "" H 7100 5150 60  0001 C CNN
F 3 "" H 7100 5150 60  0001 C CNN
	1    7100 5150
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 P1
U 1 1 4DC4E5A1
P 4250 4550
F 0 "P1" H 4500 4500 40  0000 C CNN
F 1 "Speaker" H 4500 4600 40  0000 C CNN
F 2 "" H 4250 4550 60  0001 C CNN
F 3 "" H 4250 4550 60  0001 C CNN
	1    4250 4550
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 4DC4E575
P 3400 3850
F 0 "R6" H 3550 3800 50  0000 C CNN
F 1 "10" H 3550 3900 50  0000 C CNN
F 2 "" H 3400 3850 60  0001 C CNN
F 3 "" H 3400 3850 60  0001 C CNN
	1    3400 3850
	1    0    0    -1  
$EndComp
$Comp
L POTIALPS RV1
U 1 1 4DC4E556
P 1400 3100
F 0 "RV1" H 1550 3400 60  0000 C CNN
F 1 "POTIALPS 10k" H 1400 2850 60  0000 C CNN
F 2 "" H 1400 3100 60  0001 C CNN
F 3 "" H 1400 3100 60  0001 C CNN
	1    1400 3100
	0    1    1    0   
$EndComp
$Comp
L CP1 C2
U 1 1 4DC4E535
P 3900 3850
F 0 "C2" H 4100 3800 50  0000 L CNN
F 1 "220µ" H 4050 3900 50  0000 L CNN
F 2 "" H 3900 3850 60  0001 C CNN
F 3 "" H 3900 3850 60  0001 C CNN
	1    3900 3850
	1    0    0    -1  
$EndComp
$Comp
L LM386_ U1
U 1 1 4DC4E4E8
P 2650 3000
F 0 "U1" H 2650 2600 60  0000 C CNN
F 1 "LM386_" H 2650 3400 60  0000 C CNN
F 2 "" H 2650 3000 60  0001 C CNN
F 3 "" H 2650 3000 60  0001 C CNN
	1    2650 3000
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 4DC4E4BD
P 3400 4550
F 0 "C4" H 3550 4500 50  0000 L CNN
F 1 "47n" H 3550 4600 50  0000 L CNN
F 2 "" H 3400 4550 60  0001 C CNN
F 3 "" H 3400 4550 60  0001 C CNN
	1    3400 4550
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 4DC4E4AF
P 1400 2050
F 0 "C1" H 1650 2000 50  0000 L CNN
F 1 "220n" H 1600 2100 50  0000 L CNN
F 2 "" H 1400 2050 60  0001 C CNN
F 3 "" H 1400 2050 60  0001 C CNN
	1    1400 2050
	1    0    0    -1  
$EndComp
$Comp
L SPST SW3
U 1 1 4DC4E311
P 9300 3250
F 0 "SW3" V 9200 3550 70  0000 C CNN
F 1 "Select" V 9400 3550 70  0000 C CNN
F 2 "" H 9300 3250 60  0001 C CNN
F 3 "" H 9300 3250 60  0001 C CNN
	1    9300 3250
	0    -1   -1   0   
$EndComp
$Comp
L SPST SW4
U 1 1 4DC4E30F
P 10500 3250
F 0 "SW4" V 10400 3550 70  0000 C CNN
F 1 "Save" V 10600 3550 70  0000 C CNN
F 2 "" H 10500 3250 60  0001 C CNN
F 3 "" H 10500 3250 60  0001 C CNN
	1    10500 3250
	0    -1   -1   0   
$EndComp
$Comp
L SPST SW2
U 1 1 4DC4E30B
P 8100 3250
F 0 "SW2" V 8000 3550 70  0000 C CNN
F 1 "Minus" V 8200 3550 70  0000 C CNN
F 2 "" H 8100 3250 60  0001 C CNN
F 3 "" H 8100 3250 60  0001 C CNN
	1    8100 3250
	0    -1   -1   0   
$EndComp
$Comp
L SPST SW1
U 1 1 4DC4E306
P 6900 3250
F 0 "SW1" V 6800 3550 70  0000 C CNN
F 1 "Plus" V 7000 3550 70  0000 C CNN
F 2 "" H 6900 3250 60  0001 C CNN
F 3 "" H 6900 3250 60  0001 C CNN
	1    6900 3250
	0    -1   -1   0   
$EndComp
$Comp
L ELEKTRETMIC Q1
U 1 1 4DC4E27A
P 7100 6150
F 0 "Q1" H 7100 6150 60  0000 C CNN
F 1 "ELEKTRETMIC" V 6800 6150 60  0000 C CNN
F 2 "" H 7100 6150 60  0001 C CNN
F 3 "" H 7100 6150 60  0001 C CNN
	1    7100 6150
	1    0    0    -1  
$EndComp
$EndSCHEMATC