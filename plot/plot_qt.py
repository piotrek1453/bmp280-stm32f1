import pyqtgraph as pg
import serial
from serial.tools import list_ports

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
    print("{}: {} [{}]".format(port, desc, hwid))

ser = serial.Serial(
    port=STM32_serial, baudrate=115200, stopbits=1, parity=serial.PARITY_NONE
)

if not ser.isOpen():
    ser.open()

print(ser.readline().decode())

temp_plot = pg.plot(title="Temperature")

press_plot = pg.plot(title="Pressure")

while True:
    rawString = ser.readline().decode()

    if "hPa" in rawString:
        pressure = rawString.replace("hPa", "")
        press_data.append(float(pressure))
    elif "deg C" in rawString:
        temperature = rawString.replace("deg C", "")
        temp_data.append((float(temperature)))

    temp_plot.plot(temp_data, clear=True)
    press_plot.plot(press_data, clear=True)
    pg.QtWidgets.QApplication.processEvents()
