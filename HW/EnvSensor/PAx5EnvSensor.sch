EESchema Schematic File Version 4
LIBS:calin
LIBS:power
LIBS:device
LIBS:74xx
LIBS:audio
LIBS:interface
LIBS:PAx5EnvSensor-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L EnvSensor:STM32L051x-P U1
U 1 1 55FAAE7E
P 3350 2150
F 0 "U1" H 3350 2250 60  0000 C CNN
F 1 "STM32L051x-P" H 3350 2150 39  0000 C CNN
F 2 "EnvSensor:cLQFP32-7-v2" H 3550 1950 60  0001 C CNN
F 3 "" H 3550 1950 60  0000 C CNN
	1    3350 2150
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:RFM69HW U20
U 1 1 55FAAFDD
P 6450 3200
F 0 "U20" H 6550 3650 60  0000 C CNN
F 1 "RFM69HW" H 6450 2750 60  0000 C CNN
F 2 "EnvSensor:RFM69HW" H 6450 3200 60  0001 C CNN
F 3 "" H 6450 3200 60  0000 C CNN
	1    6450 3200
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:VDD #PWR01
U 1 1 55FAB3EC
P 700 2200
F 0 "#PWR01" H 700 2050 50  0001 C CNN
F 1 "VDD" H 700 2350 50  0000 C CNN
F 2 "" H 700 2200 60  0000 C CNN
F 3 "" H 700 2200 60  0000 C CNN
	1    700  2200
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:VDD #PWR02
U 1 1 55FAB41C
P 950 2200
F 0 "#PWR02" H 950 2050 50  0001 C CNN
F 1 "VDD" H 950 2350 50  0000 C CNN
F 2 "" H 950 2200 60  0000 C CNN
F 3 "" H 950 2200 60  0000 C CNN
	1    950  2200
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:VDD #PWR03
U 1 1 55FAB445
P 1200 2200
F 0 "#PWR03" H 1200 2050 50  0001 C CNN
F 1 "VDD" H 1200 2350 50  0000 C CNN
F 2 "" H 1200 2200 60  0000 C CNN
F 3 "" H 1200 2200 60  0000 C CNN
	1    1200 2200
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:VDD #PWR04
U 1 1 55FAB4C0
P 1800 900
F 0 "#PWR04" H 1800 750 50  0001 C CNN
F 1 "VDD" H 1800 1050 50  0000 C CNN
F 2 "" H 1800 900 60  0000 C CNN
F 3 "" H 1800 900 60  0000 C CNN
	1    1800 900 
	1    0    0    -1  
$EndComp
Text Label 4700 1800 2    60   ~ 0
SWCLK
Text Label 4700 1900 2    60   ~ 0
SWDIO
Text Label 2000 2100 0    60   ~ 0
NRST
Text Label 1500 1100 0    60   ~ 0
SWCLK
Text Label 1500 1300 0    60   ~ 0
SWDIO
$Comp
L EnvSensor:VDD #PWR05
U 1 1 55FBD0FB
P 900 1850
F 0 "#PWR05" H 900 1700 50  0001 C CNN
F 1 "VDD" V 900 2050 50  0000 C CNN
F 2 "" H 900 1850 60  0000 C CNN
F 3 "" H 900 1850 60  0000 C CNN
	1    900  1850
	0    -1   -1   0   
$EndComp
$Comp
L EnvSensor:VDD #PWR06
U 1 1 55FBD709
P 2350 1800
F 0 "#PWR06" H 2350 1650 50  0001 C CNN
F 1 "VDD" H 2350 1950 50  0000 C CNN
F 2 "" H 2350 1800 60  0000 C CNN
F 3 "" H 2350 1800 60  0000 C CNN
	1    2350 1800
	0    -1   -1   0   
$EndComp
$Comp
L EnvSensor:VDD #PWR07
U 1 1 55FBD73B
P 4350 2500
F 0 "#PWR07" H 4350 2350 50  0001 C CNN
F 1 "VDD" H 4350 2650 50  0000 C CNN
F 2 "" H 4350 2500 60  0000 C CNN
F 3 "" H 4350 2500 60  0000 C CNN
	1    4350 2500
	0    1    1    0   
$EndComp
Text Label 4700 650  2    60   ~ 0
SDA
Text Label 4700 750  2    60   ~ 0
SCL
NoConn ~ 5850 3050
NoConn ~ 5850 3150
NoConn ~ 5850 3250
NoConn ~ 5850 3350
NoConn ~ 5850 3450
Text Label 1650 3000 0    60   ~ 0
An0
Text Label 1650 3100 0    60   ~ 0
An1
Text Label 4700 2300 2    60   ~ 0
GP1
Text Label 4700 2200 2    60   ~ 0
GP2
Text Label 4200 4500 0    60   ~ 0
GP1
Text Label 4200 4600 0    60   ~ 0
GP2
NoConn ~ 2450 1900
NoConn ~ 2450 2000
Text Label 3200 3650 1    60   ~ 0
MSCK
Text Label 3300 3650 1    60   ~ 0
MISO
Text Label 3400 3650 1    60   ~ 0
MOSI
$Comp
L EnvSensor:M25P10-A U30
U 1 1 560C3C81
P 6450 1500
F 0 "U30" H 6450 1800 60  0000 C CNN
F 1 "M25P10-A" H 6450 1200 60  0000 C CNN
F 2 "EnvSensor:SOIC-8-N" H 6500 1300 60  0001 C CNN
F 3 "" H 6500 1300 60  0000 C CNN
	1    6450 1500
	1    0    0    -1  
$EndComp
Text Label 7300 1550 2    60   ~ 0
MSCK
Text Label 5850 1450 2    60   ~ 0
MISO
Text Label 7300 1650 2    60   ~ 0
MOSI
Text Label 6100 2650 0    60   ~ 0
RFint
Text Label 3600 3650 1    60   ~ 0
RFint
Text Label 3100 3650 1    60   ~ 0
CSM
Text Label 5600 1350 0    60   ~ 0
CSM
Text Label 7700 3250 2    60   ~ 0
MSCK
Text Label 7700 3150 2    60   ~ 0
MISO
Text Label 7700 3050 2    60   ~ 0
MOSI
Text Label 7700 2950 2    60   ~ 0
MRFSel
Text Label 3500 3650 1    60   ~ 0
MRFSel
$Comp
L EnvSensor:INDUCTOR L1
U 1 1 563D2C0F
P 1350 1850
F 0 "L1" V 1300 1850 40  0000 C CNN
F 1 "INDUCTOR" V 1450 1850 40  0000 C CNN
F 2 "EnvSensor:R_1206_HandSoldering" H 1350 1850 60  0001 C CNN
F 3 "" H 1350 1850 60  0000 C CNN
	1    1350 1850
	0    -1   -1   0   
$EndComp
$Comp
L EnvSensor:C-PAx5 C2
U 1 1 56463B6D
P 950 2450
F 0 "C2" V 1000 2550 40  0000 L CNN
F 1 "100nF" V 1000 2200 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 988 2300 30  0001 C CNN
F 3 "" H 950 2450 60  0000 C CNN
	1    950  2450
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR08
U 1 1 56463C74
P 5450 3700
F 0 "#PWR08" H 5450 3700 30  0001 C CNN
F 1 "GND" H 5450 3630 30  0001 C CNN
F 2 "" H 5450 3700 60  0000 C CNN
F 3 "" H 5450 3700 60  0000 C CNN
	1    5450 3700
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C3
U 1 1 56463D53
P 1200 2450
F 0 "C3" V 1250 2550 40  0000 L CNN
F 1 "100nF" V 1250 2200 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 1238 2300 30  0001 C CNN
F 3 "" H 1200 2450 60  0000 C CNN
	1    1200 2450
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C5
U 1 1 56463DA4
P 1500 2450
F 0 "C5" V 1550 2550 40  0000 L CNN
F 1 "100nF" V 1550 2200 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 1538 2300 30  0001 C CNN
F 3 "" H 1500 2450 60  0000 C CNN
	1    1500 2450
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C4
U 1 1 56463DF8
P 1800 2450
F 0 "C4" V 1850 2550 40  0000 L CNN
F 1 "100nF" V 1850 2200 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 1838 2300 30  0001 C CNN
F 3 "" H 1800 2450 60  0000 C CNN
	1    1800 2450
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C1
U 1 1 56463E4D
P 700 2450
F 0 "C1" V 750 2550 40  0000 L CNN
F 1 "10uF" V 750 2200 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 738 2300 30  0001 C CNN
F 3 "" H 700 2450 60  0000 C CNN
	1    700  2450
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C20
U 1 1 564647D8
P 5200 3150
F 0 "C20" V 5250 3250 40  0000 L CNN
F 1 "100nF" V 5250 2900 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 5238 3000 30  0001 C CNN
F 3 "" H 5200 3150 60  0000 C CNN
	1    5200 3150
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C21
U 1 1 56464837
P 5450 3150
F 0 "C21" V 5500 3250 40  0000 L CNN
F 1 "100nF" V 5500 2900 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 5488 3000 30  0001 C CNN
F 3 "" H 5450 3150 60  0000 C CNN
	1    5450 3150
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR09
U 1 1 56464CC8
P 1800 2750
F 0 "#PWR09" H 1800 2750 30  0001 C CNN
F 1 "GND" H 1800 2680 30  0001 C CNN
F 2 "" H 1800 2750 60  0000 C CNN
F 3 "" H 1800 2750 60  0000 C CNN
	1    1800 2750
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR010
U 1 1 56464D0C
P 1500 2750
F 0 "#PWR010" H 1500 2750 30  0001 C CNN
F 1 "GND" H 1500 2680 30  0001 C CNN
F 2 "" H 1500 2750 60  0000 C CNN
F 3 "" H 1500 2750 60  0000 C CNN
	1    1500 2750
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR011
U 1 1 56464D50
P 1200 2750
F 0 "#PWR011" H 1200 2750 30  0001 C CNN
F 1 "GND" H 1200 2680 30  0001 C CNN
F 2 "" H 1200 2750 60  0000 C CNN
F 3 "" H 1200 2750 60  0000 C CNN
	1    1200 2750
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR012
U 1 1 56464DD7
P 950 2750
F 0 "#PWR012" H 950 2750 30  0001 C CNN
F 1 "GND" H 950 2680 30  0001 C CNN
F 2 "" H 950 2750 60  0000 C CNN
F 3 "" H 950 2750 60  0000 C CNN
	1    950  2750
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR013
U 1 1 56464E1B
P 700 2750
F 0 "#PWR013" H 700 2750 30  0001 C CNN
F 1 "GND" H 700 2680 30  0001 C CNN
F 2 "" H 700 2750 60  0000 C CNN
F 3 "" H 700 2750 60  0000 C CNN
	1    700  2750
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR014
U 1 1 56465835
P 5200 3700
F 0 "#PWR014" H 5200 3700 30  0001 C CNN
F 1 "GND" H 5200 3630 30  0001 C CNN
F 2 "" H 5200 3700 60  0000 C CNN
F 3 "" H 5200 3700 60  0000 C CNN
	1    5200 3700
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR015
U 1 1 56465873
P 7150 3700
F 0 "#PWR015" H 7150 3700 30  0001 C CNN
F 1 "GND" H 7150 3630 30  0001 C CNN
F 2 "" H 7150 3700 60  0000 C CNN
F 3 "" H 7150 3700 60  0000 C CNN
	1    7150 3700
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR016
U 1 1 56465A75
P 3700 3200
F 0 "#PWR016" H 3700 3200 30  0001 C CNN
F 1 "GND" H 3700 3130 30  0001 C CNN
F 2 "" H 3700 3200 60  0000 C CNN
F 3 "" H 3700 3200 60  0000 C CNN
	1    3700 3200
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C30
U 1 1 56465AE6
P 5200 1500
F 0 "C30" V 5250 1600 40  0000 L CNN
F 1 "100nF" V 5250 1250 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 5238 1350 30  0001 C CNN
F 3 "" H 5200 1500 60  0000 C CNN
	1    5200 1500
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR017
U 1 1 56465B5C
P 5200 1800
F 0 "#PWR017" H 5200 1800 30  0001 C CNN
F 1 "GND" H 5200 1730 30  0001 C CNN
F 2 "" H 5200 1800 60  0000 C CNN
F 3 "" H 5200 1800 60  0000 C CNN
	1    5200 1800
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR018
U 1 1 56465B9D
P 5900 1800
F 0 "#PWR018" H 5900 1800 30  0001 C CNN
F 1 "GND" H 5900 1730 30  0001 C CNN
F 2 "" H 5900 1800 60  0000 C CNN
F 3 "" H 5900 1800 60  0000 C CNN
	1    5900 1800
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR019
U 1 1 56465C1F
P 1800 1450
F 0 "#PWR019" H 1800 1450 30  0001 C CNN
F 1 "GND" H 1800 1380 30  0001 C CNN
F 2 "" H 1800 1450 60  0000 C CNN
F 3 "" H 1800 1450 60  0000 C CNN
	1    1800 1450
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR020
U 1 1 564663A4
P 3100 1100
F 0 "#PWR020" H 3100 1100 30  0001 C CNN
F 1 "GND" H 3100 1030 30  0001 C CNN
F 2 "" H 3100 1100 60  0000 C CNN
F 3 "" H 3100 1100 60  0000 C CNN
	1    3100 1100
	-1   0    0    1   
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR021
U 1 1 564664BA
P 3000 1100
F 0 "#PWR021" H 3000 1100 30  0001 C CNN
F 1 "GND" H 3000 1030 30  0001 C CNN
F 2 "" H 3000 1100 60  0000 C CNN
F 3 "" H 3000 1100 60  0000 C CNN
	1    3000 1100
	-1   0    0    1   
$EndComp
Text Label 4700 2400 2    60   ~ 0
GP0/USW
$Comp
L EnvSensor:GND-PAx5 #PWR022
U 1 1 56465B56
P 6200 4850
F 0 "#PWR022" H 6200 4850 30  0001 C CNN
F 1 "GND" H 6200 4780 30  0001 C CNN
F 2 "" H 6200 4850 60  0000 C CNN
F 3 "" H 6200 4850 60  0000 C CNN
	1    6200 4850
	1    0    0    -1  
$EndComp
Text Label 5550 750  0    60   ~ 0
ULED
$Comp
L EnvSensor:LED-PAx5 D90
U 1 1 56466190
P 6750 750
F 0 "D90" H 6750 850 50  0000 C CNN
F 1 "LED" H 6750 650 50  0000 C CNN
F 2 "EnvSensor:LED_1206_HandSoldering" H 6750 750 60  0001 C CNN
F 3 "" H 6750 750 60  0000 C CNN
	1    6750 750 
	-1   0    0    1   
$EndComp
$Comp
L EnvSensor:RR R90
U 1 1 5646612D
P 6200 750
F 0 "R90" V 6280 750 40  0000 C CNN
F 1 "330" V 6200 750 40  0000 C CNN
F 2 "EnvSensor:R_1206_HandSoldering" V 6130 750 30  0001 C CNN
F 3 "" H 6200 750 30  0000 C CNN
	1    6200 750 
	0    1    1    0   
$EndComp
Text Label 4700 1150 2    60   ~ 0
ULED
$Comp
L EnvSensor:RR R1
U 1 1 5648B47E
P 7100 2250
F 0 "R1" V 7180 2250 40  0000 C CNN
F 1 "47k" V 7100 2250 40  0000 C CNN
F 2 "EnvSensor:R_0805_HandSoldering" V 7030 2250 30  0001 C CNN
F 3 "" H 7100 2250 30  0000 C CNN
	1    7100 2250
	0    1    1    0   
$EndComp
$Comp
L EnvSensor:RR R2
U 1 1 5648B5D9
P 7100 2450
F 0 "R2" V 7180 2450 40  0000 C CNN
F 1 "47k" V 7100 2450 40  0000 C CNN
F 2 "EnvSensor:R_0805_HandSoldering" V 7030 2450 30  0001 C CNN
F 3 "" H 7100 2450 30  0000 C CNN
	1    7100 2450
	0    1    1    0   
$EndComp
Text Label 6500 2250 0    60   ~ 0
CSM
Text Label 6500 2450 0    60   ~ 0
MRFSel
$Comp
L EnvSensor:VDD #PWR023
U 1 1 564A2CEF
P 7050 750
F 0 "#PWR023" H 7050 600 50  0001 C CNN
F 1 "VDD" V 7050 950 50  0000 C CNN
F 2 "" H 7050 750 60  0000 C CNN
F 3 "" H 7050 750 60  0000 C CNN
	1    7050 750 
	0    1    1    0   
$EndComp
$Comp
L EnvSensor:HIH8000_I2C U50
U 1 1 564BAB29
P 9650 1500
F 0 "U50" H 9500 1800 60  0000 C CNN
F 1 "HIH8000_I2C" H 9650 1200 60  0000 C CNN
F 2 "EnvSensor:HIH8000" H 9650 1500 60  0001 C CNN
F 3 "" H 9650 1500 60  0000 C CNN
	1    9650 1500
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C51
U 1 1 564BB0C0
P 8950 1950
F 0 "C51" V 9000 2000 40  0000 L CNN
F 1 "100nF" V 9000 1700 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 8988 1800 30  0001 C CNN
F 3 "" H 8950 1950 60  0000 C CNN
	1    8950 1950
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C50
U 1 1 564BB1FA
P 10450 1350
F 0 "C50" V 10500 1450 40  0000 L CNN
F 1 "100nF" V 10500 1100 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 10488 1200 30  0001 C CNN
F 3 "" H 10450 1350 60  0000 C CNN
	1    10450 1350
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:RR R50
U 1 1 5663FEC3
P 8650 1200
F 0 "R50" V 8550 1200 40  0000 C CNN
F 1 "1k8" V 8650 1200 40  0000 C CNN
F 2 "EnvSensor:R_0805_HandSoldering" V 8580 1200 30  0001 C CNN
F 3 "" H 8650 1200 30  0000 C CNN
	1    8650 1200
	-1   0    0    -1  
$EndComp
$Comp
L EnvSensor:RR R51
U 1 1 566400F9
P 8500 1200
F 0 "R51" V 8400 1200 40  0000 C CNN
F 1 "1k8" V 8500 1200 40  0000 C CNN
F 2 "EnvSensor:R_0805_HandSoldering" V 8430 1200 30  0001 C CNN
F 3 "" H 8500 1200 30  0000 C CNN
	1    8500 1200
	1    0    0    -1  
$EndComp
NoConn ~ 4250 2100
NoConn ~ 3600 1250
$Comp
L EnvSensor:GND-PAx5 #PWR024
U 1 1 57028F26
P 4100 4800
F 0 "#PWR024" H 4100 4800 30  0001 C CNN
F 1 "GND" H 4100 4730 30  0001 C CNN
F 2 "" H 4100 4800 60  0000 C CNN
F 3 "" H 4100 4800 60  0000 C CNN
	1    4100 4800
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:IRLML6402 Q1
U 1 1 5702953B
P 1700 4550
F 0 "Q1" H 1750 4760 60  0000 R CNN
F 1 "IRLML6402" H 1780 4340 60  0000 R CNN
F 2 "EnvSensor:SOT-23-calin" H 1690 4550 60  0001 C CNN
F 3 "" H 1690 4550 60  0000 C CNN
	1    1700 4550
	0    -1   -1   0   
$EndComp
$Comp
L EnvSensor:VDD #PWR025
U 1 1 570295D0
P 2000 4300
F 0 "#PWR025" H 2000 4150 50  0001 C CNN
F 1 "VDD" H 1950 4450 50  0000 C CNN
F 2 "" H 2000 4300 60  0000 C CNN
F 3 "" H 2000 4300 60  0000 C CNN
	1    2000 4300
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:RR R3
U 1 1 57029831
P 2150 4650
F 0 "R3" V 2230 4650 40  0000 C CNN
F 1 "47k" V 2150 4650 40  0000 C CNN
F 2 "EnvSensor:R_0805_HandSoldering" V 2080 4650 30  0001 C CNN
F 3 "" H 2150 4650 30  0000 C CNN
	1    2150 4650
	-1   0    0    1   
$EndComp
$Comp
L EnvSensor:VDD #PWR026
U 1 1 570298D8
P 2150 4300
F 0 "#PWR026" H 2150 4150 50  0001 C CNN
F 1 "VDD" H 2200 4450 50  0000 C CNN
F 2 "" H 2150 4300 60  0000 C CNN
F 3 "" H 2150 4300 60  0000 C CNN
	1    2150 4300
	1    0    0    -1  
$EndComp
Text Label 1300 5000 0    60   ~ 0
Vpo
$Comp
L EnvSensor:+3.3VP #PWR027
U 1 1 5702AA28
P 1300 4400
F 0 "#PWR027" H 1350 4430 20  0001 C CNN
F 1 "+3.3VP" H 1300 4490 30  0000 C CNN
F 2 "" H 1300 4400 60  0000 C CNN
F 3 "" H 1300 4400 60  0000 C CNN
	1    1300 4400
	0    -1   -1   0   
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR028
U 1 1 5702AF74
P 5200 1250
F 0 "#PWR028" H 5250 1280 20  0001 C CNN
F 1 "+3.3VP" H 5200 1340 30  0000 C CNN
F 2 "" H 5200 1250 60  0000 C CNN
F 3 "" H 5200 1250 60  0000 C CNN
	1    5200 1250
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR029
U 1 1 5702AFE8
P 5900 1250
F 0 "#PWR029" H 5950 1280 20  0001 C CNN
F 1 "+3.3VP" H 5900 1340 30  0000 C CNN
F 2 "" H 5900 1250 60  0000 C CNN
F 3 "" H 5900 1250 60  0000 C CNN
	1    5900 1250
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR030
U 1 1 5702B05C
P 7000 1250
F 0 "#PWR030" H 7050 1280 20  0001 C CNN
F 1 "+3.3VP" H 7000 1340 30  0000 C CNN
F 2 "" H 7000 1250 60  0000 C CNN
F 3 "" H 7000 1250 60  0000 C CNN
	1    7000 1250
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR031
U 1 1 5702B2D0
P 5200 2650
F 0 "#PWR031" H 5250 2680 20  0001 C CNN
F 1 "+3.3VP" H 5200 2740 30  0000 C CNN
F 2 "" H 5200 2650 60  0000 C CNN
F 3 "" H 5200 2650 60  0000 C CNN
	1    5200 2650
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR032
U 1 1 5702B344
P 5650 2650
F 0 "#PWR032" H 5700 2680 20  0001 C CNN
F 1 "+3.3VP" H 5650 2740 30  0000 C CNN
F 2 "" H 5650 2650 60  0000 C CNN
F 3 "" H 5650 2650 60  0000 C CNN
	1    5650 2650
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR033
U 1 1 5702B781
P 7450 2450
F 0 "#PWR033" H 7500 2480 20  0001 C CNN
F 1 "+3.3VP" H 7450 2540 30  0000 C CNN
F 2 "" H 7450 2450 60  0000 C CNN
F 3 "" H 7450 2450 60  0000 C CNN
	1    7450 2450
	0    1    1    0   
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR034
U 1 1 5702B7F5
P 7450 2250
F 0 "#PWR034" H 7500 2280 20  0001 C CNN
F 1 "+3.3VP" H 7450 2340 30  0000 C CNN
F 2 "" H 7450 2250 60  0000 C CNN
F 3 "" H 7450 2250 60  0000 C CNN
	1    7450 2250
	0    1    1    0   
$EndComp
$Comp
L EnvSensor:SW-Push4 SW1
U 1 1 5706D72F
P 7800 4550
F 0 "SW1" H 7800 4750 60  0000 C CNN
F 1 "SW-Push4" H 7800 4350 60  0000 C CNN
F 2 "EnvSensor:cSW-Push-4" H 7800 4550 60  0001 C CNN
F 3 "" H 7800 4550 60  0000 C CNN
	1    7800 4550
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C6
U 1 1 5706D7D7
P 8250 4550
F 0 "C6" V 8300 4650 40  0000 L CNN
F 1 "100nF" V 8300 4300 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 8288 4400 30  0001 C CNN
F 3 "" H 8250 4550 60  0000 C CNN
	1    8250 4550
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR035
U 1 1 5706D9F9
P 8150 4950
F 0 "#PWR035" H 8150 4950 30  0001 C CNN
F 1 "GND" H 8150 4880 30  0001 C CNN
F 2 "" H 8150 4950 60  0000 C CNN
F 3 "" H 8150 4950 60  0000 C CNN
	1    8150 4950
	1    0    0    -1  
$EndComp
Text Label 8000 4250 2    60   ~ 0
GP0/USW
$Comp
L EnvSensor:MCP16251 U40
U 1 1 57137A81
P 9850 3000
F 0 "U40" H 10000 3200 60  0000 C CNN
F 1 "MCP16251" H 9850 2800 60  0000 C CNN
F 2 "EnvSensor:SOT23-6" H 9850 3000 60  0001 C CNN
F 3 "" H 9850 3000 60  0000 C CNN
	1    9850 3000
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:INDUCTOR L40
U 1 1 57137B2D
P 9850 2650
F 0 "L40" V 9800 2650 40  0000 C CNN
F 1 "4,7uH" V 9950 2650 40  0000 C CNN
F 2 "EnvSensor:TYS50xx" H 9850 2650 60  0001 C CNN
F 3 "" H 9850 2650 60  0000 C CNN
	1    9850 2650
	0    -1   -1   0   
$EndComp
$Comp
L EnvSensor:C-PAx5 C41
U 1 1 57137E5E
P 8800 3300
F 0 "C41" V 8850 3400 40  0000 L CNN
F 1 "100nF" V 8850 3050 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 8838 3150 30  0001 C CNN
F 3 "" H 8800 3300 60  0000 C CNN
	1    8800 3300
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:C-PAx5 C40
U 1 1 57137E6A
P 8550 3300
F 0 "C40" V 8600 3400 40  0000 L CNN
F 1 "4.7uF" V 8600 3050 40  0000 L CNN
F 2 "EnvSensor:C_0805_HandSoldering" H 8588 3150 30  0001 C CNN
F 3 "" H 8550 3300 60  0000 C CNN
	1    8550 3300
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR036
U 1 1 57137E76
P 8800 3700
F 0 "#PWR036" H 8800 3700 30  0001 C CNN
F 1 "GND" H 8800 3630 30  0001 C CNN
F 2 "" H 8800 3700 60  0000 C CNN
F 3 "" H 8800 3700 60  0000 C CNN
	1    8800 3700
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR037
U 1 1 57137E7C
P 8550 3700
F 0 "#PWR037" H 8550 3700 30  0001 C CNN
F 1 "GND" H 8550 3630 30  0001 C CNN
F 2 "" H 8550 3700 60  0000 C CNN
F 3 "" H 8550 3700 60  0000 C CNN
	1    8550 3700
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:RR R40
U 1 1 57138170
P 10700 2750
F 0 "R40" V 10780 2750 40  0000 C CNN
F 1 "1M69" V 10700 2750 40  0000 C CNN
F 2 "EnvSensor:R_0805_HandSoldering" V 10630 2750 30  0001 C CNN
F 3 "" H 10700 2750 30  0000 C CNN
	1    10700 2750
	-1   0    0    1   
$EndComp
$Comp
L EnvSensor:RR R41
U 1 1 57138696
P 10700 3400
F 0 "R41" V 10780 3400 40  0000 C CNN
F 1 "1M" V 10700 3400 40  0000 C CNN
F 2 "EnvSensor:R_0805_HandSoldering" V 10630 3400 30  0001 C CNN
F 3 "" H 10700 3400 30  0000 C CNN
	1    10700 3400
	-1   0    0    1   
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR038
U 1 1 5713879C
P 10700 3700
F 0 "#PWR038" H 10700 3700 30  0001 C CNN
F 1 "GND" H 10700 3630 30  0001 C CNN
F 2 "" H 10700 3700 60  0000 C CNN
F 3 "" H 10700 3700 60  0000 C CNN
	1    10700 3700
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:VCC #PWR039
U 1 1 57138DB0
P 6200 4300
F 0 "#PWR039" H 6200 4400 30  0001 C CNN
F 1 "VCC" H 6200 4450 30  0000 C CNN
F 2 "" H 6200 4300 60  0000 C CNN
F 3 "" H 6200 4300 60  0000 C CNN
	1    6200 4300
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:VCC #PWR040
U 1 1 57138F47
P 8550 2600
F 0 "#PWR040" H 8550 2700 30  0001 C CNN
F 1 "VCC" H 8550 2750 30  0000 C CNN
F 2 "" H 8550 2600 60  0000 C CNN
F 3 "" H 8550 2600 60  0000 C CNN
	1    8550 2600
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR041
U 1 1 57139648
P 9300 3700
F 0 "#PWR041" H 9300 3700 30  0001 C CNN
F 1 "GND" H 9300 3630 30  0001 C CNN
F 2 "" H 9300 3700 60  0000 C CNN
F 3 "" H 9300 3700 60  0000 C CNN
	1    9300 3700
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:RR R52
U 1 1 566A155F
P 2900 4900
F 0 "R52" V 2980 4900 40  0000 C CNN
F 1 "2k7" V 2900 4900 40  0000 C CNN
F 2 "EnvSensor:R_1206_HandSoldering" V 2830 4900 30  0001 C CNN
F 3 "" H 2900 4900 30  0000 C CNN
	1    2900 4900
	0    1    1    0   
$EndComp
Text Label 3450 4900 0    60   ~ 0
An1
$Comp
L EnvSensor:RR R53
U 1 1 571403A9
P 6550 4350
F 0 "R53" V 6630 4350 40  0000 C CNN
F 1 "330" V 6550 4350 40  0000 C CNN
F 2 "EnvSensor:R_0805_HandSoldering" V 6480 4350 30  0001 C CNN
F 3 "" H 6550 4350 30  0000 C CNN
	1    6550 4350
	0    1    1    0   
$EndComp
Text Label 6850 4350 0    60   ~ 0
An0
Text Label 8200 1550 0    60   ~ 0
SDA
Text Label 8200 1650 0    60   ~ 0
SCL
$Comp
L EnvSensor:+3.3VP #PWR042
U 1 1 57140CF8
P 10450 1100
F 0 "#PWR042" H 10500 1130 20  0001 C CNN
F 1 "+3.3VP" H 10450 1190 30  0000 C CNN
F 2 "" H 10450 1100 60  0000 C CNN
F 3 "" H 10450 1100 60  0000 C CNN
	1    10450 1100
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:VDD #PWR043
U 1 1 571421BD
P 10800 2450
F 0 "#PWR043" H 10800 2300 50  0001 C CNN
F 1 "VDD" H 10800 2600 50  0000 C CNN
F 2 "" H 10800 2450 60  0000 C CNN
F 3 "" H 10800 2450 60  0000 C CNN
	1    10800 2450
	0    1    1    0   
$EndComp
Text Label 2800 4750 0    60   ~ 0
NRST
Text Label 5150 4600 0    60   ~ 0
GP4
Text Label 4700 2000 2    60   ~ 0
GP4
Text Label 4150 950  2    60   ~ 0
GP6
Text Label 4150 850  2    60   ~ 0
GP7
Text Label 2650 3300 0    60   ~ 0
Vpo
$Comp
L EnvSensor:GND-PAx5 #PWR044
U 1 1 57E84635
P 3050 4600
F 0 "#PWR044" H 3050 4600 30  0001 C CNN
F 1 "GND" H 3050 4530 30  0001 C CNN
F 2 "" H 3050 4600 60  0000 C CNN
F 3 "" H 3050 4600 60  0000 C CNN
	1    3050 4600
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR045
U 1 1 57E84A79
P 3350 4550
F 0 "#PWR045" H 3400 4580 20  0001 C CNN
F 1 "+3.3VP" H 3350 4640 30  0000 C CNN
F 2 "" H 3350 4550 60  0000 C CNN
F 3 "" H 3350 4550 60  0000 C CNN
	1    3350 4550
	-1   0    0    1   
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR046
U 1 1 57E84DF7
P 2550 4900
F 0 "#PWR046" H 2550 4900 30  0001 C CNN
F 1 "GND" H 2550 4830 30  0001 C CNN
F 2 "" H 2550 4900 60  0000 C CNN
F 3 "" H 2550 4900 60  0000 C CNN
	1    2550 4900
	0    1    1    0   
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR047
U 1 1 57E8583F
P 10450 1650
F 0 "#PWR047" H 10450 1650 30  0001 C CNN
F 1 "GND" H 10450 1580 30  0001 C CNN
F 2 "" H 10450 1650 60  0000 C CNN
F 3 "" H 10450 1650 60  0000 C CNN
	1    10450 1650
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR048
U 1 1 57E858CE
P 9050 1750
F 0 "#PWR048" H 9050 1750 30  0001 C CNN
F 1 "GND" H 9050 1680 30  0001 C CNN
F 2 "" H 9050 1750 60  0000 C CNN
F 3 "" H 9050 1750 60  0000 C CNN
	1    9050 1750
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR049
U 1 1 57E8595D
P 8950 2250
F 0 "#PWR049" H 8950 2250 30  0001 C CNN
F 1 "GND" H 8950 2180 30  0001 C CNN
F 2 "" H 8950 2250 60  0000 C CNN
F 3 "" H 8950 2250 60  0000 C CNN
	1    8950 2250
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR050
U 1 1 57E85CFA
P 8500 900
F 0 "#PWR050" H 8550 930 20  0001 C CNN
F 1 "+3.3VP" H 8500 990 30  0000 C CNN
F 2 "" H 8500 900 60  0000 C CNN
F 3 "" H 8500 900 60  0000 C CNN
	1    8500 900 
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR051
U 1 1 57E85D89
P 8650 900
F 0 "#PWR051" H 8700 930 20  0001 C CNN
F 1 "+3.3VP" H 8650 990 30  0000 C CNN
F 2 "" H 8650 900 60  0000 C CNN
F 3 "" H 8650 900 60  0000 C CNN
	1    8650 900 
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR052
U 1 1 57E86150
P 10250 1100
F 0 "#PWR052" H 10300 1130 20  0001 C CNN
F 1 "+3.3VP" H 10250 1190 30  0000 C CNN
F 2 "" H 10250 1100 60  0000 C CNN
F 3 "" H 10250 1100 60  0000 C CNN
	1    10250 1100
	1    0    0    -1  
$EndComp
NoConn ~ 10150 1450
NoConn ~ 10150 1550
NoConn ~ 10150 1650
NoConn ~ 2450 2500
$Comp
L EnvSensor:CS_4 P10
U 1 1 57EDF933
P 3200 4350
F 0 "P10" H 3200 4600 60  0000 C CNN
F 1 "Photo" V 3300 4350 50  0000 C CNN
F 2 "EnvSensor:Pin_Header_Straight_1x04" H 3200 4500 60  0001 C CNN
F 3 "" H 3200 4500 60  0000 C CNN
	1    3200 4350
	0    1    -1   0   
$EndComp
$Comp
L EnvSensor:CS_3 P11
U 1 1 57EDFD03
P 4600 4600
F 0 "P11" H 4600 4800 60  0000 C CNN
F 1 "USART" V 4700 4600 50  0000 C CNN
F 2 "EnvSensor:Pin_Header_Straight_1x03" H 4600 4700 60  0001 C CNN
F 3 "" H 4600 4700 60  0000 C CNN
	1    4600 4600
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:CS_4 P2
U 1 1 57EE02F4
P 2050 1150
F 0 "P2" H 2050 1400 60  0000 C CNN
F 1 "SWD" V 2150 1150 50  0000 C CNN
F 2 "EnvSensor:Pin_Header_Straight_1x04" H 2050 1300 60  0001 C CNN
F 3 "" H 2050 1300 60  0000 C CNN
	1    2050 1150
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:CS_2 P1
U 1 1 57EE05EA
P 6200 4600
F 0 "P1" V 6100 4600 60  0000 C CNN
F 1 "PwrIn" V 6300 4600 50  0000 C CNN
F 2 "EnvSensor:Pin_Header_Straight_1x02" H 6200 4650 60  0001 C CNN
F 3 "" H 6200 4650 60  0000 C CNN
	1    6200 4600
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:CS_1 Po2
U 1 1 57EE3037
P 5550 4600
F 0 "Po2" H 5700 4600 60  0000 C CNN
F 1 "~" H 5700 4600 50  0000 C CNN
F 2 "EnvSensor:PinTest-SMD" H 5550 4600 60  0001 C CNN
F 3 "" H 5550 4600 60  0000 C CNN
	1    5550 4600
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:CS_1 P20
U 1 1 57EE38C9
P 7800 3450
F 0 "P20" H 7800 3550 50  0000 C CNN
F 1 "Antenna" H 7800 3350 50  0000 C CNN
F 2 "EnvSensor:Pin_Header_Straight_1x01" H 7800 3450 60  0001 C CNN
F 3 "" H 7800 3450 60  0000 C CNN
	1    7800 3450
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:GND-PAx5 #PWR053
U 1 1 57EFEBC5
P 4450 3700
F 0 "#PWR053" H 4450 3700 30  0001 C CNN
F 1 "GND" H 4450 3630 30  0001 C CNN
F 2 "" H 4450 3700 60  0000 C CNN
F 3 "" H 4450 3700 60  0000 C CNN
	1    4450 3700
	1    0    0    -1  
$EndComp
$Comp
L EnvSensor:+3.3VP #PWR054
U 1 1 57EFF18D
P 4450 3200
F 0 "#PWR054" H 4500 3230 20  0001 C CNN
F 1 "+3.3VP" V 4450 3350 30  0000 C CNN
F 2 "" H 4450 3200 60  0000 C CNN
F 3 "" H 4450 3200 60  0000 C CNN
	1    4450 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 3050 3400 3750
Wire Wire Line
	7800 3050 7050 3050
Wire Wire Line
	3300 3050 3300 3750
Wire Wire Line
	7800 3150 7050 3150
Wire Wire Line
	3200 3050 3200 3750
Wire Wire Line
	7800 3250 7050 3250
Wire Wire Line
	5850 2950 5750 2950
Wire Wire Line
	5750 2950 5750 2650
Wire Wire Line
	4250 1800 4800 1800
Wire Wire Line
	4250 1900 4800 1900
Wire Wire Line
	1900 1100 1400 1100
Wire Wire Line
	1900 1300 1400 1300
Wire Wire Line
	1900 1000 1800 1000
Wire Wire Line
	1800 1000 1800 900 
Wire Wire Line
	1900 1200 1800 1200
Wire Wire Line
	1800 1200 1800 1450
Wire Wire Line
	2450 2200 1800 2200
Wire Wire Line
	1500 2100 2450 2100
Wire Wire Line
	4350 2500 4250 2500
Wire Wire Line
	2450 1800 2350 1800
Wire Wire Line
	3200 1250 3200 650 
Wire Wire Line
	3200 650  4800 650 
Wire Wire Line
	3300 1250 3300 750 
Wire Wire Line
	3300 750  4800 750 
Wire Wire Line
	7050 2850 7150 2850
Wire Wire Line
	7150 2850 7150 3350
Wire Wire Line
	7050 3350 7150 3350
Connection ~ 7150 3350
Wire Wire Line
	7150 3550 7050 3550
Wire Wire Line
	5850 3550 5650 3550
Wire Wire Line
	5650 3550 5650 2650
Wire Wire Line
	7050 3450 7650 3450
Wire Wire Line
	1550 3000 2150 3000
Wire Wire Line
	2150 3000 2150 2300
Wire Wire Line
	2150 2300 2450 2300
Wire Wire Line
	1550 3100 2250 3100
Wire Wire Line
	2250 3100 2250 2400
Wire Wire Line
	2250 2400 2450 2400
Wire Wire Line
	4250 2200 4800 2200
Wire Wire Line
	4250 2300 4800 2300
Wire Wire Line
	4250 2400 4800 2400
Wire Wire Line
	4450 4500 4100 4500
Wire Wire Line
	4450 4600 4100 4600
Wire Wire Line
	4100 4700 4450 4700
Wire Wire Line
	3600 3750 3600 3050
Wire Wire Line
	6000 1350 5450 1350
Wire Wire Line
	6000 1450 5450 1450
Wire Wire Line
	6900 1550 7500 1550
Wire Wire Line
	6900 1650 7500 1650
Wire Wire Line
	7000 1250 7000 1350
Wire Wire Line
	7000 1450 6900 1450
Wire Wire Line
	6900 1350 7000 1350
Connection ~ 7000 1350
Wire Wire Line
	6000 1650 5900 1650
Wire Wire Line
	5900 1250 5900 1550
Wire Wire Line
	5900 1550 6000 1550
Wire Wire Line
	5750 2650 6400 2650
Wire Wire Line
	3100 3050 3100 3750
Wire Wire Line
	3500 3050 3500 3750
Wire Wire Line
	7800 2950 7050 2950
Wire Wire Line
	5850 2850 5450 2850
Wire Wire Line
	1650 1850 1800 1850
Connection ~ 1800 2200
Wire Wire Line
	1050 1850 900  1850
Wire Wire Line
	700  2200 700  2250
Wire Wire Line
	950  2200 950  2250
Wire Wire Line
	1200 2200 1200 2250
Wire Wire Line
	1500 2100 1500 2250
Wire Wire Line
	1800 1850 1800 2200
Wire Wire Line
	1800 2650 1800 2750
Wire Wire Line
	1500 2650 1500 2750
Wire Wire Line
	1200 2650 1200 2750
Wire Wire Line
	950  2650 950  2750
Wire Wire Line
	700  2650 700  2750
Wire Wire Line
	5450 2850 5450 2950
Wire Wire Line
	5200 2650 5200 2950
Connection ~ 7150 3550
Wire Wire Line
	5450 3700 5450 3350
Wire Wire Line
	5200 3700 5200 3350
Wire Wire Line
	3700 3050 3700 3200
Wire Wire Line
	5900 1650 5900 1800
Wire Wire Line
	5200 1250 5200 1300
Wire Wire Line
	5200 1700 5200 1800
Wire Wire Line
	3100 1100 3100 1250
Wire Wire Line
	3000 1100 3000 1250
Wire Wire Line
	3700 1250 3700 1150
Wire Wire Line
	3700 1150 4800 1150
Wire Wire Line
	5950 750  5450 750 
Wire Wire Line
	6550 750  6450 750 
Wire Wire Line
	7050 750  6950 750 
Wire Wire Line
	7450 2250 7350 2250
Wire Wire Line
	7450 2450 7350 2450
Wire Wire Line
	6850 2250 6400 2250
Wire Wire Line
	6850 2450 6400 2450
Wire Wire Line
	8100 1550 8650 1550
Wire Wire Line
	8100 1650 8500 1650
Wire Wire Line
	8750 1550 8850 1650
Wire Wire Line
	8850 1650 9150 1650
Wire Wire Line
	9150 1550 8850 1550
Wire Wire Line
	8850 1550 8750 1650
Wire Wire Line
	10450 1100 10450 1150
Wire Wire Line
	10250 1350 10150 1350
Wire Wire Line
	10250 1100 10250 1350
Wire Wire Line
	8650 1450 8650 1550
Connection ~ 8650 1550
Wire Wire Line
	8500 1450 8500 1650
Connection ~ 8500 1650
Wire Wire Line
	4100 4700 4100 4800
Wire Wire Line
	1900 4400 2000 4400
Wire Wire Line
	2000 4400 2000 4300
Wire Wire Line
	1500 4400 1300 4400
Wire Wire Line
	2150 4300 2150 4400
Wire Wire Line
	2150 5000 2150 4900
Wire Wire Line
	1200 5000 1700 5000
Wire Wire Line
	1700 5000 1700 4750
Connection ~ 1700 5000
Wire Wire Line
	8150 4850 8150 4950
Wire Wire Line
	8250 4850 8250 4750
Wire Wire Line
	7550 4850 8050 4850
Wire Wire Line
	8800 3500 8800 3700
Wire Wire Line
	8550 3500 8550 3700
Wire Wire Line
	10400 2900 10300 2900
Wire Wire Line
	10400 2450 10400 2650
Wire Wire Line
	10400 2650 10150 2650
Wire Wire Line
	9550 2650 9300 2650
Wire Wire Line
	9300 2650 9300 2900
Wire Wire Line
	9300 2900 9400 2900
Wire Wire Line
	10400 2450 9200 2450
Wire Wire Line
	9200 2450 9200 2900
Wire Wire Line
	9200 3100 9400 3100
Connection ~ 10400 2650
Wire Wire Line
	10700 3650 10700 3700
Wire Wire Line
	10700 3000 10700 3100
Wire Wire Line
	10300 3100 10700 3100
Connection ~ 10700 3100
Wire Wire Line
	10300 3000 10500 3000
Wire Wire Line
	10500 3000 10500 2450
Wire Wire Line
	10500 2450 10700 2450
Wire Wire Line
	10700 2450 10700 2500
Wire Wire Line
	8550 2600 8550 2900
Wire Wire Line
	8550 2900 8800 2900
Connection ~ 8550 2900
Connection ~ 9200 2900
Wire Wire Line
	8800 3100 8800 2900
Connection ~ 8800 2900
Wire Wire Line
	9400 3000 9300 3000
Wire Wire Line
	9300 3000 9300 3700
Wire Wire Line
	3150 4900 3250 4900
Wire Wire Line
	3250 4500 3250 4900
Wire Wire Line
	6800 4350 7050 4350
Connection ~ 10700 2450
Wire Wire Line
	3150 4500 3150 4750
Wire Wire Line
	3150 4750 2650 4750
Wire Wire Line
	5400 4600 5050 4600
Wire Wire Line
	3400 1250 3400 850 
Wire Wire Line
	3400 850  4250 850 
Wire Wire Line
	4250 2000 4800 2000
Wire Wire Line
	3000 3050 3000 3300
Wire Wire Line
	3000 3300 2550 3300
Wire Wire Line
	3500 1250 3500 950 
Wire Wire Line
	3500 950  4250 950 
Wire Wire Line
	3050 4500 3050 4600
Wire Wire Line
	3350 4500 3350 4550
Wire Wire Line
	2650 4900 2550 4900
Wire Wire Line
	8500 900  8500 950 
Wire Wire Line
	8650 900  8650 950 
Wire Wire Line
	8950 2150 8950 2250
Wire Wire Line
	10450 1550 10450 1650
Wire Wire Line
	9150 1450 9050 1450
Wire Wire Line
	9050 1450 9050 1750
Wire Wire Line
	8950 1750 8950 1350
Wire Wire Line
	8950 1350 9150 1350
Connection ~ 3250 4900
Wire Wire Line
	7950 4450 8050 4450
Wire Wire Line
	8050 4450 8050 4250
Wire Wire Line
	7550 4250 8050 4250
Wire Wire Line
	7550 4250 7550 4450
Wire Wire Line
	7550 4450 7650 4450
Wire Wire Line
	8250 4250 8250 4350
Connection ~ 8050 4250
Wire Wire Line
	7950 4650 8050 4650
Wire Wire Line
	8050 4650 8050 4850
Wire Wire Line
	7550 4850 7550 4650
Wire Wire Line
	7550 4650 7650 4650
Connection ~ 8150 4850
Connection ~ 8050 4850
Wire Wire Line
	6200 4300 6200 4350
Wire Wire Line
	6200 4800 6200 4850
Wire Wire Line
	6300 4350 6200 4350
Connection ~ 6200 4350
Wire Wire Line
	4550 3300 4100 3300
Wire Wire Line
	4550 3400 4100 3400
Text Label 4200 3300 0    60   ~ 0
SCL
Text Label 4200 3400 0    60   ~ 0
SDA
$Comp
L EnvSensor:CS_1 Po1
U 1 1 57EFFA08
P 5550 4500
F 0 "Po1" H 5700 4500 60  0000 C CNN
F 1 "~" H 5700 4500 50  0000 C CNN
F 2 "EnvSensor:PinTest-SMD" H 5550 4500 60  0001 C CNN
F 3 "" H 5550 4500 60  0000 C CNN
	1    5550 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 4500 5050 4500
Text Label 5150 4500 0    60   ~ 0
GP7
$Comp
L EnvSensor:CS_4 P12
U 1 1 57EFFD02
P 4700 3450
F 0 "P12" H 4700 3700 60  0000 C CNN
F 1 "ExtConn" V 4800 3450 50  0000 C CNN
F 2 "EnvSensor:Pin_Header_Straight_1x04" H 4700 3600 60  0001 C CNN
F 3 "" H 4700 3600 60  0000 C CNN
	1    4700 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3200 4450 3500
Wire Wire Line
	4450 3500 4550 3500
Wire Wire Line
	4550 3600 4450 3600
Wire Wire Line
	4450 3600 4450 3700
$Comp
L EnvSensor:BOARDLAYOUT B1
U 1 1 58B73A13
P 9500 4550
F 0 "B1" H 9350 4800 60  0000 C CNN
F 1 "BSLH" H 9500 4250 60  0000 C CNN
F 2 "EnvSensor:Box-Sens-LH-NPTH" H 9500 4550 60  0001 C CNN
F 3 "" H 9500 4550 60  0000 C CNN
	1    9500 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 3350 7150 3550
Wire Wire Line
	7000 1350 7000 1450
Wire Wire Line
	1800 2200 1800 2250
Wire Wire Line
	7150 3550 7150 3700
Wire Wire Line
	8650 1550 8750 1550
Wire Wire Line
	8500 1650 8750 1650
Wire Wire Line
	1700 5000 2150 5000
Wire Wire Line
	10400 2650 10400 2900
Wire Wire Line
	10700 3100 10700 3150
Wire Wire Line
	8550 2900 8550 3100
Wire Wire Line
	9200 2900 9200 3100
Wire Wire Line
	8800 2900 9200 2900
Wire Wire Line
	10700 2450 10800 2450
Wire Wire Line
	3250 4900 3700 4900
Wire Wire Line
	8050 4250 8250 4250
Wire Wire Line
	8150 4850 8250 4850
Wire Wire Line
	8050 4850 8150 4850
Wire Wire Line
	6200 4350 6200 4400
$EndSCHEMATC
