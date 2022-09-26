EESchema Schematic File Version 4
EELAYER 30 0
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
L power:+3V3 #PWR0102
U 1 1 62660E6E
P 9100 2000
F 0 "#PWR0102" H 9100 1850 50  0001 C CNN
F 1 "+3V3" H 9150 2150 50  0000 C CNN
F 2 "" H 9100 2000 50  0001 C CNN
F 3 "" H 9100 2000 50  0001 C CNN
	1    9100 2000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 62660E68
P 8600 2300
F 0 "#PWR0103" H 8600 2050 50  0001 C CNN
F 1 "GND" H 8605 2127 50  0000 C CNN
F 2 "" H 8600 2300 50  0001 C CNN
F 3 "" H 8600 2300 50  0001 C CNN
	1    8600 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J7
U 1 1 62740E96
P 8600 4500
F 0 "J7" H 8700 4400 50  0000 L CNN
F 1 "Screw_Terminal_01x02" H 8400 4300 50  0001 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 8600 4500 50  0001 C CNN
F 3 "~" H 8600 4500 50  0001 C CNN
	1    8600 4500
	-1   0    0    -1  
$EndComp
$Comp
L RF_Module:ESP32-WROOM-32 U1
U 1 1 62672731
P 3000 2200
F 0 "U1" V 2400 1000 50  0000 C CNN
F 1 "ESP32-WROOM-32" V 2300 1150 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 3000 700 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf" H 2700 2250 50  0001 C CNN
	1    3000 2200
	0    -1   -1   0   
$EndComp
$Comp
L power:+3V3 #PWR0104
U 1 1 626DD8BD
P 1350 2200
F 0 "#PWR0104" H 1350 2050 50  0001 C CNN
F 1 "+3V3" H 1365 2373 50  0000 C CNN
F 2 "" H 1350 2200 50  0001 C CNN
F 3 "" H 1350 2200 50  0001 C CNN
	1    1350 2200
	1    0    0    -1  
$EndComp
Text GLabel 1900 1600 1    50   Input ~ 0
TX0D
Text GLabel 2100 1600 1    50   Input ~ 0
RX0D
Text GLabel 2700 1600 1    50   Input ~ 0
MTDO
Text GLabel 2600 1600 1    50   Input ~ 0
MTMS
Text GLabel 2500 1600 1    50   Input ~ 0
MTCK
Text GLabel 2400 1600 1    50   Input ~ 0
MTDI
$Comp
L Device:R R5
U 1 1 626F4BC0
P 5700 6800
F 0 "R5" V 5700 6750 50  0000 L CNN
F 1 "100" V 5650 6500 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 5630 6800 50  0001 C CNN
F 3 "~" H 5700 6800 50  0001 C CNN
	1    5700 6800
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R3
U 1 1 626F5866
P 5700 6600
F 0 "R3" V 5700 6550 50  0000 L CNN
F 1 "100" V 5650 6300 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 5630 6600 50  0001 C CNN
F 3 "~" H 5700 6600 50  0001 C CNN
	1    5700 6600
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R4
U 1 1 626F5FD8
P 5700 6700
F 0 "R4" V 5700 6650 50  0000 L CNN
F 1 "100" V 5650 6400 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 5630 6700 50  0001 C CNN
F 3 "~" H 5700 6700 50  0001 C CNN
	1    5700 6700
	0    -1   -1   0   
$EndComp
Text GLabel 6050 6700 2    50   Input ~ 0
MTCK
Text GLabel 6050 6600 2    50   Input ~ 0
MTMS
Text GLabel 6050 6800 2    50   Input ~ 0
MTDO
$Comp
L power:GND #PWR0105
U 1 1 6270A2E1
P 4400 2200
F 0 "#PWR0105" H 4400 1950 50  0001 C CNN
F 1 "GND" H 4405 2027 50  0000 C CNN
F 2 "" H 4400 2200 50  0001 C CNN
F 3 "" H 4400 2200 50  0001 C CNN
	1    4400 2200
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 6270C253
P 3150 3300
F 0 "SW2" H 3000 3600 50  0000 L CNN
F 1 "SW_Push" H 3000 3500 50  0000 L CNN
F 2 "robot-pcb_custom:wurth_tactile_sw" H 3150 3500 50  0001 C CNN
F 3 "~" H 3150 3500 50  0001 C CNN
	1    3150 3300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 627104CE
P 2950 3450
F 0 "C5" H 3065 3496 50  0000 L CNN
F 1 "0.1u" H 3065 3405 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 2988 3300 50  0001 C CNN
F 3 "~" H 2950 3450 50  0001 C CNN
	1    2950 3450
	1    0    0    -1  
$EndComp
Text Notes 2800 3200 0    50   ~ 0
Boot
$Comp
L Device:C C1
U 1 1 6271AE5A
P 1250 2350
F 0 "C1" H 1050 2350 50  0000 L CNN
F 1 "22u" H 1050 2250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 1288 2200 50  0001 C CNN
F 3 "~" H 1250 2350 50  0001 C CNN
	1    1250 2350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 6271B9CB
P 1450 2350
F 0 "C2" H 1550 2350 50  0000 L CNN
F 1 "0.1u" H 1500 2250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 1488 2200 50  0001 C CNN
F 3 "~" H 1450 2350 50  0001 C CNN
	1    1450 2350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 627299AD
P 1350 2500
F 0 "#PWR0106" H 1350 2250 50  0001 C CNN
F 1 "GND" H 1350 2350 50  0000 C CNN
F 2 "" H 1350 2500 50  0001 C CNN
F 3 "" H 1350 2500 50  0001 C CNN
	1    1350 2500
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 62736634
P 2500 3300
F 0 "SW1" H 2650 3600 50  0000 R CNN
F 1 "SW_Push" H 2650 3500 50  0000 R CNN
F 2 "robot-pcb_custom:wurth_tactile_sw" H 2500 3500 50  0001 C CNN
F 3 "~" H 2500 3500 50  0001 C CNN
	1    2500 3300
	-1   0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 6273663B
P 2300 3450
F 0 "C4" H 2415 3496 50  0000 L CNN
F 1 "0.1u" H 2415 3405 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 2338 3300 50  0001 C CNN
F 3 "~" H 2300 3450 50  0001 C CNN
	1    2300 3450
	1    0    0    -1  
$EndComp
Text Notes 2150 3200 0    50   ~ 0
En
Text Notes 8750 900  0    50   ~ 0
Power
Text Notes 8250 3650 0    50   ~ 0
\nMotor driver
$Comp
L Connector:Conn_01x03_Male J5
U 1 1 6287506B
P 6750 2150
F 0 "J5" H 6700 2250 50  0000 R CNN
F 1 "Conn_01x03_Male" H 7250 2350 50  0000 R CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x03_P2.54mm_Vertical" H 6750 2150 50  0001 C CNN
F 3 "~" H 6750 2150 50  0001 C CNN
	1    6750 2150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 62875071
P 6050 2250
F 0 "#PWR0107" H 6050 2000 50  0001 C CNN
F 1 "GND" H 6055 2077 50  0000 C CNN
F 2 "" H 6050 2250 50  0001 C CNN
F 3 "" H 6050 2250 50  0001 C CNN
	1    6050 2250
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0108
U 1 1 62875077
P 6050 2150
F 0 "#PWR0108" H 6050 2000 50  0001 C CNN
F 1 "+3V3" H 6000 2300 50  0000 C CNN
F 2 "" H 6050 2150 50  0001 C CNN
F 3 "" H 6050 2150 50  0001 C CNN
	1    6050 2150
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Male J4
U 1 1 62879C6A
P 6750 1550
F 0 "J4" H 6700 1650 50  0000 R CNN
F 1 "Conn_01x03_Male" H 7250 1750 50  0000 R CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x03_P2.54mm_Vertical" H 6750 1550 50  0001 C CNN
F 3 "~" H 6750 1550 50  0001 C CNN
	1    6750 1550
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 62879C70
P 6050 1650
F 0 "#PWR0109" H 6050 1400 50  0001 C CNN
F 1 "GND" H 6055 1477 50  0000 C CNN
F 2 "" H 6050 1650 50  0001 C CNN
F 3 "" H 6050 1650 50  0001 C CNN
	1    6050 1650
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0110
U 1 1 62879C76
P 6050 1550
F 0 "#PWR0110" H 6050 1400 50  0001 C CNN
F 1 "+3V3" H 6000 1700 50  0000 C CNN
F 2 "" H 6050 1550 50  0001 C CNN
F 3 "" H 6050 1550 50  0001 C CNN
	1    6050 1550
	1    0    0    -1  
$EndComp
Text Notes 5850 1200 0    50   ~ 0
PWM out pins
Text Notes 4850 6250 0    50   ~ 0
JTAG
Wire Wire Line
	5850 6800 6050 6800
Wire Wire Line
	6050 6600 5850 6600
Wire Wire Line
	5850 6700 6050 6700
Text GLabel 1800 1600 1    50   Input ~ 0
IO0
Text GLabel 2950 3300 0    50   Input ~ 0
IO0
Wire Wire Line
	3350 3600 3350 3300
Wire Wire Line
	2700 3600 2700 3300
Text GLabel 2300 3300 0    50   Input ~ 0
EN
Wire Wire Line
	2500 3600 2700 3600
Wire Wire Line
	2300 3600 2500 3600
Connection ~ 2500 3600
$Comp
L power:GND #PWR0111
U 1 1 6290CC6E
P 2500 3600
F 0 "#PWR0111" H 2500 3350 50  0001 C CNN
F 1 "GND" H 2505 3427 50  0000 C CNN
F 2 "" H 2500 3600 50  0001 C CNN
F 3 "" H 2500 3600 50  0001 C CNN
	1    2500 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 3600 3350 3600
Wire Wire Line
	2950 3600 3150 3600
Connection ~ 3150 3600
$Comp
L power:GND #PWR0112
U 1 1 626C6473
P 3150 3600
F 0 "#PWR0112" H 3150 3350 50  0001 C CNN
F 1 "GND" H 3155 3427 50  0000 C CNN
F 2 "" H 3150 3600 50  0001 C CNN
F 3 "" H 3150 3600 50  0001 C CNN
	1    3150 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 2500 1350 2500
Wire Wire Line
	1250 2200 1350 2200
Connection ~ 1350 2500
Wire Wire Line
	1350 2500 1450 2500
Wire Wire Line
	1600 2200 1450 2200
Connection ~ 1350 2200
Connection ~ 1450 2200
Wire Wire Line
	1450 2200 1350 2200
NoConn ~ 3000 2800
NoConn ~ 3100 2800
NoConn ~ 3200 2800
NoConn ~ 3300 2800
NoConn ~ 3400 2800
NoConn ~ 3500 2800
Text GLabel 2800 1600 1    50   Input ~ 0
TX2D
Text GLabel 2900 1600 1    50   Input ~ 0
RX2D
Text GLabel 3750 6850 0    50   Input ~ 0
TX2D
Text GLabel 3750 6950 0    50   Input ~ 0
RX2D
Wire Wire Line
	5850 6900 6050 6900
Text GLabel 6050 6900 2    50   Input ~ 0
MTDI
$Comp
L Device:R R6
U 1 1 626F6766
P 5700 6900
F 0 "R6" V 5700 6850 50  0000 L CNN
F 1 "100" V 5650 6600 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 5630 6900 50  0001 C CNN
F 3 "~" H 5700 6900 50  0001 C CNN
	1    5700 6900
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 627423D6
P 5050 7000
F 0 "#PWR0113" H 5050 6750 50  0001 C CNN
F 1 "GND" H 5055 6827 50  0000 C CNN
F 2 "" H 5050 7000 50  0001 C CNN
F 3 "" H 5050 7000 50  0001 C CNN
	1    5050 7000
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0114
U 1 1 62745C63
P 5050 6600
F 0 "#PWR0114" H 5050 6450 50  0001 C CNN
F 1 "+3V3" H 5065 6773 50  0000 C CNN
F 2 "" H 5050 6600 50  0001 C CNN
F 3 "" H 5050 6600 50  0001 C CNN
	1    5050 6600
	1    0    0    -1  
$EndComp
Text GLabel 6550 2050 0    50   Input ~ 0
PWM2
Text GLabel 6550 1450 0    50   Input ~ 0
PWM1
Wire Wire Line
	6050 2250 6550 2250
Wire Wire Line
	6050 2150 6550 2150
Wire Wire Line
	6050 1650 6550 1650
Wire Wire Line
	6050 1550 6550 1550
Text GLabel 3100 1600 1    50   Input ~ 0
PWM1
Text GLabel 3200 1600 1    50   Input ~ 0
PWM2
Text GLabel 3300 1600 1    50   Input ~ 0
MOTOR_A_IN1
Text GLabel 3400 1600 1    50   Input ~ 0
MOTOR_A_IN2
Text GLabel 3800 1600 1    50   Input ~ 0
MOTOR_B_IN1
Text GLabel 3900 1600 1    50   Input ~ 0
MOTOR_B_IN2
NoConn ~ 2000 2800
NoConn ~ 2100 2800
Text Notes 3250 6300 0    50   ~ 0
HC12 interface
Text GLabel 9700 4500 2    50   Input ~ 0
MOTOR_A_IN2
Text GLabel 9700 4400 2    50   Input ~ 0
MOTOR_A_IN1
Text GLabel 9700 4700 2    50   Input ~ 0
MOTOR_B_IN2
Text GLabel 9700 4600 2    50   Input ~ 0
MOTOR_B_IN1
Text GLabel 2200 1600 1    50   Input ~ 0
LED
Text GLabel 3000 1600 1    50   Input ~ 0
HC12_SET
NoConn ~ 3700 1600
NoConn ~ 3600 1600
NoConn ~ 3500 1600
NoConn ~ 2300 1600
NoConn ~ 2000 1600
Text GLabel 1750 6700 0    50   Input ~ 0
EN
Text Notes 1050 6400 0    50   ~ 0
Programming interface
Text GLabel 1750 6800 0    50   Input ~ 0
TX0D
Text GLabel 1750 6900 0    50   Input ~ 0
RX0D
Text GLabel 2250 6900 2    50   Input ~ 0
IO0
$Comp
L power:+3.3V #PWR0115
U 1 1 6313C306
P 2550 6700
F 0 "#PWR0115" H 2550 6550 50  0001 C CNN
F 1 "+3.3V" H 2565 6873 50  0000 C CNN
F 2 "" H 2550 6700 50  0001 C CNN
F 3 "" H 2550 6700 50  0001 C CNN
	1    2550 6700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0116
U 1 1 6313CE57
P 2550 6800
F 0 "#PWR0116" H 2550 6550 50  0001 C CNN
F 1 "GND" H 2555 6627 50  0000 C CNN
F 2 "" H 2550 6800 50  0001 C CNN
F 3 "" H 2550 6800 50  0001 C CNN
	1    2550 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 6700 2250 6700
Wire Wire Line
	2250 6800 2550 6800
Wire Wire Line
	5050 7000 5050 6900
Connection ~ 5050 7000
Connection ~ 5050 6800
Wire Wire Line
	5050 6800 5050 6700
Connection ~ 5050 6900
Wire Wire Line
	5050 6900 5050 6800
NoConn ~ 5550 7000
$Comp
L Connector:Conn_01x05_Female J2
U 1 1 6318514B
P 3950 6850
F 0 "J2" H 3978 6876 50  0000 L CNN
F 1 "Conn_01x05_Female" H 3978 6785 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x05_P2.54mm_Vertical" H 3950 6850 50  0001 C CNN
F 3 "~" H 3950 6850 50  0001 C CNN
	1    3950 6850
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0117
U 1 1 63185D81
P 3350 6650
F 0 "#PWR0117" H 3350 6500 50  0001 C CNN
F 1 "+3.3V" H 3365 6823 50  0000 C CNN
F 2 "" H 3350 6650 50  0001 C CNN
F 3 "" H 3350 6650 50  0001 C CNN
	1    3350 6650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0118
U 1 1 6318650A
P 3350 6750
F 0 "#PWR0118" H 3350 6500 50  0001 C CNN
F 1 "GND" H 3355 6577 50  0000 C CNN
F 2 "" H 3350 6750 50  0001 C CNN
F 3 "" H 3350 6750 50  0001 C CNN
	1    3350 6750
	1    0    0    -1  
$EndComp
Text GLabel 3750 7050 0    50   Input ~ 0
HC12_SET
Wire Wire Line
	3350 6750 3750 6750
Wire Wire Line
	3750 6650 3350 6650
$Comp
L Connector:Screw_Terminal_01x02 J8
U 1 1 6273C28D
P 8600 4900
F 0 "J8" H 8700 4800 50  0000 L CNN
F 1 "Screw_Terminal_01x02" H 8400 4700 50  0001 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 8600 4900 50  0001 C CNN
F 3 "~" H 8600 4900 50  0001 C CNN
	1    8600 4900
	-1   0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0119
U 1 1 6269E495
P 8100 2000
F 0 "#PWR0119" H 8100 1850 50  0001 C CNN
F 1 "+12V" H 8115 2173 50  0000 C CNN
F 2 "" H 8100 2000 50  0001 C CNN
F 3 "" H 8100 2000 50  0001 C CNN
	1    8100 2000
	1    0    0    -1  
$EndComp
Connection ~ 9100 2000
$Comp
L Device:LED D1
U 1 1 631D0CD4
P 9500 2150
F 0 "D1" V 9539 2032 50  0000 R CNN
F 1 "LED" V 9448 2032 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 9500 2150 50  0001 C CNN
F 3 "~" H 9500 2150 50  0001 C CNN
	1    9500 2150
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R7
U 1 1 631D156C
P 9350 2300
F 0 "R7" V 9450 2300 50  0000 C CNN
F 1 "68" V 9550 2300 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 9280 2300 50  0001 C CNN
F 3 "~" H 9350 2300 50  0001 C CNN
	1    9350 2300
	0    1    1    0   
$EndComp
Wire Wire Line
	8800 4800 8800 4900
Wire Wire Line
	8800 5000 8800 5100
Wire Wire Line
	8800 4600 8800 4700
Wire Wire Line
	9700 5400 9750 5400
Wire Wire Line
	8800 5200 8750 5200
Wire Wire Line
	8750 5200 8750 5400
Wire Wire Line
	8800 5400 8750 5400
Connection ~ 8750 5400
$Comp
L power:GND #PWR0120
U 1 1 63163F5F
P 9250 5700
F 0 "#PWR0120" H 9250 5450 50  0001 C CNN
F 1 "GND" H 9255 5527 50  0000 C CNN
F 2 "" H 9250 5700 50  0001 C CNN
F 3 "" H 9250 5700 50  0001 C CNN
	1    9250 5700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0121
U 1 1 6316485F
P 8200 4300
F 0 "#PWR0121" H 8200 4050 50  0001 C CNN
F 1 "GND" H 8050 4300 50  0000 C CNN
F 2 "" H 8200 4300 50  0001 C CNN
F 3 "" H 8200 4300 50  0001 C CNN
	1    8200 4300
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0122
U 1 1 631787DC
P 8200 4000
F 0 "#PWR0122" H 8200 3850 50  0001 C CNN
F 1 "+12V" H 8215 4173 50  0000 C CNN
F 2 "" H 8200 4000 50  0001 C CNN
F 3 "" H 8200 4000 50  0001 C CNN
	1    8200 4000
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0123
U 1 1 6318103D
P 8150 5300
F 0 "#PWR0123" H 8150 5150 50  0001 C CNN
F 1 "+12V" H 8165 5473 50  0000 C CNN
F 2 "" H 8150 5300 50  0001 C CNN
F 3 "" H 8150 5300 50  0001 C CNN
	1    8150 5300
	1    0    0    -1  
$EndComp
Connection ~ 8600 2300
Wire Wire Line
	8100 2000 8300 2000
Wire Wire Line
	8100 2300 8600 2300
$Comp
L Regulator_Linear:LM1117-3.3 U2
U 1 1 63156A86
P 8600 2000
F 0 "U2" H 8600 2242 50  0000 C CNN
F 1 "LM1117-3.3" H 8600 2151 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223" H 8600 2000 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm1117.pdf" H 8600 2000 50  0001 C CNN
	1    8600 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C6
U 1 1 6317072D
P 8100 2150
F 0 "C6" H 8218 2196 50  0000 L CNN
F 1 "10u" H 8218 2105 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 8138 2000 50  0001 C CNN
F 3 "~" H 8100 2150 50  0001 C CNN
	1    8100 2150
	1    0    0    -1  
$EndComp
Connection ~ 8100 2000
Wire Wire Line
	8600 2300 9100 2300
Wire Wire Line
	8900 2000 9100 2000
$Comp
L Device:CP C11
U 1 1 62660E90
P 9100 2150
F 0 "C11" H 9200 2250 50  0000 L CNN
F 1 "10u" H 9200 2150 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 9138 2000 50  0001 C CNN
F 3 "~" H 9100 2150 50  0001 C CNN
	1    9100 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9100 2000 9500 2000
Wire Wire Line
	9200 2300 9100 2300
Connection ~ 9100 2300
$Comp
L Connector:Screw_Terminal_01x02 J9
U 1 1 62656411
P 9350 1300
F 0 "J9" H 9300 1400 50  0000 L CNN
F 1 "Screw_Terminal_01x02" H 9300 1100 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 9350 1300 50  0001 C CNN
F 3 "~" H 9350 1300 50  0001 C CNN
	1    9350 1300
	1    0    0    1   
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J6
U 1 1 63179D98
P 8950 1300
F 0 "J6" H 8950 1400 50  0000 C CNN
F 1 "Screw_Terminal_01x02" H 8868 1066 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 8950 1300 50  0001 C CNN
F 3 "~" H 8950 1300 50  0001 C CNN
	1    8950 1300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0124
U 1 1 631842F3
P 9150 1350
F 0 "#PWR0124" H 9150 1100 50  0001 C CNN
F 1 "GND" H 9155 1177 50  0000 C CNN
F 2 "" H 9150 1350 50  0001 C CNN
F 3 "" H 9150 1350 50  0001 C CNN
	1    9150 1350
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0125
U 1 1 6318498A
P 9150 1150
F 0 "#PWR0125" H 9150 1000 50  0001 C CNN
F 1 "+12V" H 9165 1323 50  0000 C CNN
F 2 "" H 9150 1150 50  0001 C CNN
F 3 "" H 9150 1150 50  0001 C CNN
	1    9150 1150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C13
U 1 1 631866F6
P 9850 4100
F 0 "C13" V 9598 4100 50  0000 C CNN
F 1 "22n" V 9689 4100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 9888 3950 50  0001 C CNN
F 3 "~" H 9850 4100 50  0001 C CNN
	1    9850 4100
	0    1    1    0   
$EndComp
$Comp
L Device:C C12
U 1 1 63189B62
P 9750 5550
F 0 "C12" H 9865 5596 50  0000 L CNN
F 1 "470n" H 9865 5505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 9788 5400 50  0001 C CNN
F 3 "~" H 9750 5550 50  0001 C CNN
	1    9750 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8750 5400 8750 5700
$Comp
L Device:C C10
U 1 1 6318EA14
P 8650 4100
F 0 "C10" V 8398 4100 50  0000 C CNN
F 1 "220n" V 8489 4100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 8688 3950 50  0001 C CNN
F 3 "~" H 8650 4100 50  0001 C CNN
	1    8650 4100
	0    1    1    0   
$EndComp
Wire Wire Line
	8500 4100 8500 4200
Wire Wire Line
	8500 4200 8800 4200
Wire Wire Line
	10000 4100 10000 4200
Wire Wire Line
	9700 4200 10000 4200
$Comp
L Device:C C9
U 1 1 631935AC
P 8500 5450
F 0 "C9" H 8615 5496 50  0000 L CNN
F 1 "10n" H 8615 5405 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 8538 5300 50  0001 C CNN
F 3 "~" H 8500 5450 50  0001 C CNN
	1    8500 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C7
U 1 1 63193ADA
P 8150 5450
F 0 "C7" H 8268 5496 50  0000 L CNN
F 1 "100u" H 8268 5405 50  0000 L CNN
F 2 "Capacitor_SMD:CP_Elec_6.3x7.7" H 8188 5300 50  0001 C CNN
F 3 "~" H 8150 5450 50  0001 C CNN
	1    8150 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:C C8
U 1 1 63194388
P 8200 4150
F 0 "C8" H 8315 4196 50  0000 L CNN
F 1 "10n" H 8315 4105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 8238 4000 50  0001 C CNN
F 3 "~" H 8200 4150 50  0001 C CNN
	1    8200 4150
	1    0    0    -1  
$EndComp
Connection ~ 8200 4300
Connection ~ 8500 5300
Wire Wire Line
	8150 5300 8500 5300
Wire Wire Line
	8500 5300 8800 5300
Connection ~ 8150 5300
Wire Wire Line
	8750 5700 8500 5700
Wire Wire Line
	8150 5700 8150 5600
Wire Wire Line
	8500 5600 8500 5700
Connection ~ 8500 5700
Wire Wire Line
	8500 5700 8150 5700
Wire Wire Line
	8200 4300 8800 4300
Wire Wire Line
	8200 4000 8500 4000
Wire Wire Line
	8500 4000 8500 4100
Connection ~ 8500 4100
Connection ~ 8200 4000
$Comp
L power:+3.3V #PWR0126
U 1 1 631AA309
P 4400 1200
F 0 "#PWR0126" H 4400 1050 50  0001 C CNN
F 1 "+3.3V" H 4450 1350 50  0000 C CNN
F 2 "" H 4400 1200 50  0001 C CNN
F 3 "" H 4400 1200 50  0001 C CNN
	1    4400 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 631AE91F
P 10400 4950
F 0 "R8" H 10470 4996 50  0000 L CNN
F 1 "10k" H 10470 4905 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 10330 4950 50  0001 C CNN
F 3 "~" H 10400 4950 50  0001 C CNN
	1    10400 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R9
U 1 1 631AF21E
P 10400 5250
F 0 "R9" H 10470 5296 50  0000 L CNN
F 1 "15k" H 10470 5205 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 10330 5250 50  0001 C CNN
F 3 "~" H 10400 5250 50  0001 C CNN
	1    10400 5250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0127
U 1 1 631AFC25
P 10400 5400
F 0 "#PWR0127" H 10400 5150 50  0001 C CNN
F 1 "GND" H 10405 5227 50  0000 C CNN
F 2 "" H 10400 5400 50  0001 C CNN
F 3 "" H 10400 5400 50  0001 C CNN
	1    10400 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 5200 9700 5100
Text GLabel 9800 5400 2    50   Input ~ 0
DVDD
Text GLabel 10400 4800 1    50   Input ~ 0
DVDD
$Comp
L custom_symbols:DRV8935 U3
U 1 1 631C50C1
P 9250 4000
F 0 "U3" H 9250 4125 50  0000 C CNN
F 1 "DRV8935" H 9250 4034 50  0000 C CNN
F 2 "Package_SO:HTSSOP-28-1EP_4.4x9.7mm_P0.65mm_EP2.85x5.4mm" H 9250 4000 50  0001 C CNN
F 3 "" H 9250 4000 50  0001 C CNN
	1    9250 4000
	1    0    0    -1  
$EndComp
Connection ~ 8800 4600
Connection ~ 8800 5000
Connection ~ 9250 5700
Wire Wire Line
	9250 5700 9750 5700
Wire Wire Line
	9250 5700 9250 5550
Wire Wire Line
	8750 5700 9250 5700
Connection ~ 8750 5700
$Comp
L power:+3.3V #PWR0128
U 1 1 631CED27
P 4000 1050
F 0 "#PWR0128" H 4000 900 50  0001 C CNN
F 1 "+3.3V" H 4150 1150 50  0000 C CNN
F 2 "" H 4000 1050 50  0001 C CNN
F 3 "" H 4000 1050 50  0001 C CNN
	1    4000 1050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 631D1BB9
P 4000 1200
F 0 "R2" H 4070 1246 50  0000 L CNN
F 1 "10k" H 4070 1155 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 3930 1200 50  0001 C CNN
F 3 "~" H 4000 1200 50  0001 C CNN
	1    4000 1200
	1    0    0    -1  
$EndComp
Text GLabel 4000 1400 2    50   Input ~ 0
nFAULT
$Comp
L power:GND #PWR0129
U 1 1 631DA14B
P 9850 4950
F 0 "#PWR0129" H 9850 4700 50  0001 C CNN
F 1 "GND" H 9950 4950 50  0000 C CNN
F 2 "" H 9850 4950 50  0001 C CNN
F 3 "" H 9850 4950 50  0001 C CNN
	1    9850 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 5100 10400 5100
Connection ~ 9700 5100
Connection ~ 10400 5100
Wire Wire Line
	9800 5400 9750 5400
Connection ~ 9750 5400
Wire Wire Line
	1800 2800 1800 3300
Wire Wire Line
	1800 3300 1550 3300
Wire Wire Line
	1450 3300 1550 3300
Text GLabel 1450 3300 0    50   Input ~ 0
EN
$Comp
L power:+3V3 #PWR0130
U 1 1 6277E8E8
P 1550 3000
F 0 "#PWR0130" H 1550 2850 50  0001 C CNN
F 1 "+3V3" H 1565 3173 50  0000 C CNN
F 2 "" H 1550 3000 50  0001 C CNN
F 3 "" H 1550 3000 50  0001 C CNN
	1    1550 3000
	1    0    0    -1  
$EndComp
Connection ~ 1550 3300
$Comp
L power:GND #PWR0131
U 1 1 6277A74D
P 1550 3600
F 0 "#PWR0131" H 1550 3350 50  0001 C CNN
F 1 "GND" H 1555 3427 50  0000 C CNN
F 2 "" H 1550 3600 50  0001 C CNN
F 3 "" H 1550 3600 50  0001 C CNN
	1    1550 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 626EB5A0
P 1550 3450
F 0 "C3" H 1350 3450 50  0000 L CNN
F 1 "1u" H 1350 3350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 1588 3300 50  0001 C CNN
F 3 "~" H 1550 3450 50  0001 C CNN
	1    1550 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 626EDB5E
P 1550 3150
F 0 "R1" H 1700 3050 50  0000 C CNN
F 1 "10k" H 1700 3150 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 1480 3150 50  0001 C CNN
F 3 "~" H 1550 3150 50  0001 C CNN
	1    1550 3150
	-1   0    0    1   
$EndComp
$Comp
L Connector:TestPoint TP6
U 1 1 6321E89C
P 10400 5100
F 0 "TP6" V 10354 5288 50  0000 L CNN
F 1 "TestPoint" V 10445 5288 50  0000 L CNN
F 2 "TestPoint:TestPoint_Keystone_5000-5004_Miniature" H 10600 5100 50  0001 C CNN
F 3 "~" H 10600 5100 50  0001 C CNN
	1    10400 5100
	0    1    1    0   
$EndComp
Text GLabel 9700 5300 2    50   Input ~ 0
nFAULT
Text GLabel 4900 3250 0    50   Input ~ 0
MOTOR_A_IN1
Text GLabel 4900 3350 0    50   Input ~ 0
MOTOR_A_IN2
Text GLabel 4900 3450 0    50   Input ~ 0
MOTOR_B_IN1
Text GLabel 4900 3550 0    50   Input ~ 0
MOTOR_B_IN2
Text GLabel 4900 3650 0    50   Input ~ 0
nFAULT
$Comp
L Connector:TestPoint TP2
U 1 1 6322B851
P 4900 3350
F 0 "TP2" V 4900 3550 50  0000 L CNN
F 1 "TestPoint" V 4900 3700 50  0000 L CNN
F 2 "TestPoint:TestPoint_Keystone_5000-5004_Miniature" H 5100 3350 50  0001 C CNN
F 3 "~" H 5100 3350 50  0001 C CNN
	1    4900 3350
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP1
U 1 1 6322C4ED
P 4900 3250
F 0 "TP1" V 4900 3450 50  0000 L CNN
F 1 "TestPoint" V 4900 3600 50  0000 L CNN
F 2 "TestPoint:TestPoint_Keystone_5000-5004_Miniature" H 5100 3250 50  0001 C CNN
F 3 "~" H 5100 3250 50  0001 C CNN
	1    4900 3250
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP3
U 1 1 6322CB70
P 4900 3450
F 0 "TP3" V 4900 3650 50  0000 L CNN
F 1 "TestPoint" V 4900 3800 50  0000 L CNN
F 2 "TestPoint:TestPoint_Keystone_5000-5004_Miniature" H 5100 3450 50  0001 C CNN
F 3 "~" H 5100 3450 50  0001 C CNN
	1    4900 3450
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP4
U 1 1 6322E5D0
P 4900 3550
F 0 "TP4" V 4900 3750 50  0000 L CNN
F 1 "TestPoint" V 4900 3900 50  0000 L CNN
F 2 "TestPoint:TestPoint_Keystone_5000-5004_Miniature" H 5100 3550 50  0001 C CNN
F 3 "~" H 5100 3550 50  0001 C CNN
	1    4900 3550
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP5
U 1 1 6322EC6A
P 4900 3650
F 0 "TP5" V 4900 3850 50  0000 L CNN
F 1 "TestPoint" V 4900 4000 50  0000 L CNN
F 2 "TestPoint:TestPoint_Keystone_5000-5004_Miniature" H 5100 3650 50  0001 C CNN
F 3 "~" H 5100 3650 50  0001 C CNN
	1    4900 3650
	0    1    1    0   
$EndComp
Wire Wire Line
	8800 4400 8800 4500
$Comp
L Device:LED D2
U 1 1 6318D836
P 3700 3300
F 0 "D2" V 3739 3182 50  0000 R CNN
F 1 "LED" V 3648 3182 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 3700 3300 50  0001 C CNN
F 3 "~" H 3700 3300 50  0001 C CNN
	1    3700 3300
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R10
U 1 1 6318E751
P 3700 3600
F 0 "R10" H 3770 3646 50  0000 L CNN
F 1 "68" H 3770 3555 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 3630 3600 50  0001 C CNN
F 3 "~" H 3700 3600 50  0001 C CNN
	1    3700 3600
	1    0    0    -1  
$EndComp
Text GLabel 3700 3150 0    50   Input ~ 0
LED
$Comp
L power:GND #PWR01
U 1 1 6319104D
P 3700 3750
F 0 "#PWR01" H 3700 3500 50  0001 C CNN
F 1 "GND" H 3705 3577 50  0000 C CNN
F 2 "" H 3700 3750 50  0001 C CNN
F 3 "" H 3700 3750 50  0001 C CNN
	1    3700 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 4950 9850 4900
Wire Wire Line
	9850 4900 9750 4900
Wire Wire Line
	9750 4900 9750 5000
Wire Wire Line
	9750 5000 9700 5000
Connection ~ 8800 4500
Connection ~ 8800 4900
$Comp
L Device:R R11
U 1 1 6325C553
P 4400 1350
F 0 "R11" H 4470 1396 50  0000 L CNN
F 1 "10k" H 4470 1305 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 4330 1350 50  0001 C CNN
F 3 "~" H 4400 1350 50  0001 C CNN
	1    4400 1350
	1    0    0    -1  
$EndComp
Text GLabel 9700 4300 2    50   Input ~ 0
nSLEEP
Text GLabel 4100 1600 2    50   Input ~ 0
nSLEEP
Wire Wire Line
	4400 1500 4100 1500
Wire Wire Line
	4100 1500 4100 1600
Wire Wire Line
	4000 1350 4000 1600
$Comp
L Switch:SW_DPST_x2 SW3
U 1 1 6327B921
P 4600 1500
F 0 "SW3" H 4850 1700 50  0000 C CNN
F 1 "SW_DPST_x2" H 4900 1600 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4600 1500 50  0001 C CNN
F 3 "~" H 4600 1500 50  0001 C CNN
	1    4600 1500
	1    0    0    -1  
$EndComp
Connection ~ 4400 1500
$Comp
L power:GND #PWR02
U 1 1 6327F3F1
P 4800 1550
F 0 "#PWR02" H 4800 1300 50  0001 C CNN
F 1 "GND" H 4805 1377 50  0000 C CNN
F 2 "" H 4800 1550 50  0001 C CNN
F 3 "" H 4800 1550 50  0001 C CNN
	1    4800 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 1550 4800 1500
Connection ~ 9150 1200
Wire Wire Line
	9150 1150 9150 1200
Wire Wire Line
	9150 1300 9150 1350
Connection ~ 9150 1300
$Comp
L Connector_Generic:Conn_02x05_Odd_Even J3
U 1 1 632CCE3A
P 5250 6800
F 0 "J3" H 5300 7217 50  0000 C CNN
F 1 "Conn_02x05_Odd_Even" H 5300 7126 50  0000 C CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_2x05_P1.27mm_Vertical_SMD" H 5250 6800 50  0001 C CNN
F 3 "~" H 5250 6800 50  0001 C CNN
	1    5250 6800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J1
U 1 1 632E355A
P 1950 6800
F 0 "J1" H 2000 7117 50  0000 C CNN
F 1 "Conn_02x03_Odd_Even" H 2000 7026 50  0000 C CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_2x03_P1.27mm_Vertical_SMD" H 1950 6800 50  0001 C CNN
F 3 "~" H 1950 6800 50  0001 C CNN
	1    1950 6800
	1    0    0    -1  
$EndComp
$EndSCHEMATC
