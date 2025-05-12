#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# This script reads data from a serial port and plots temperature and pressure data.
# It uses the PySerial library to handle serial communication and Matplotlib for plotting.
# The script automatically detects the serial port for the STM32 device and reads data from it.
# It expects the data to be in a specific format, with temperature and pressure values followed by their units.
# The script collects the data in lists and plots them in a two-subplot figure.

import matplotlib.pyplot as plt
import serial
from serial.tools import list_ports
from termcolor import colored

ports = serial.tools.list_ports.comports()
STM32_serial = None

buffer_size = 20
pressure = None
temperature = None
temp_data = []
press_data = []

for port, desc, hwid in sorted(ports):
    if "STMicroelectronics STLink Virtual COM Port" in desc:
        STM32_serial = port

if STM32_serial is None:
    print(colored("STM32 Serial Port not found", "red"))
    print(colored("Automatic port detection failed.", "blue"))
    print(colored("Please select the port manually.", "blue"))
    print(colored("Available ports:", "blue"))
    for port, desc, hwid in sorted(ports):
        print(colored(f"Port: {port} | Description: {desc} | HWID: {hwid}\n", "yellow"))
    print("Example: COM3 (Windows) or /dev/ttyUSB0 (Unix-like)")
    STM32_serial = input("Please enter the port name: ")

ser = serial.Serial(
    port=STM32_serial, baudrate=115200, stopbits=1, parity=serial.PARITY_NONE
)

if not ser.isOpen():
    ser.open()

print(ser.readline().decode())

while True:
    while len(temp_data) < buffer_size or len(press_data) < buffer_size:
        rawString = ser.readline().decode()

        if "hPa" in rawString:
            pressure = rawString.replace("hPa", "")
            press_data.append(float(pressure))
        elif "deg C" in rawString:
            temperature = rawString.replace("deg C", "")
            temp_data.append((float(temperature)))

        pressure = temperature = None

        print(temp_data)
        print(press_data)

    fig, (ax1, ax2) = plt.subplots(2, sharex=True)
    ax1.ticklabel_format(useOffset=False)
    ax2.ticklabel_format(useOffset=False)
    ax1.set_ylabel("Temperature")
    ax2.set_ylabel("Pressure")

    ax1.plot(temp_data)
    ax2.plot(press_data)
    ax1.set_xticks(range(0, buffer_size + 1))
    ax2.set_xticks(range(0, buffer_size + 1))

    plt.show()
    plt.close()
    temp_data = []
    press_data = []
