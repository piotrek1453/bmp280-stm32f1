### @package plot_qt
# Class for plotting data from BMP280 sensor received over UART


import numpy as np
import pyqtgraph as pg
import serial
from serial.tools import list_ports


class SerialPlot:
    ## @brief The constructor. Connects to STM32 COM port and continuously plots the data received
    def __init__(self):
        self.ser = self.connect_port()

        self.press_data, self.temp_data = self.initialize_dataframes()

        self.temp_plot = pg.plot(title="Temperature")
        self.press_plot = pg.plot(title="Pressure")
        pg.setConfigOptions(antialias=True)

        self.plot()

    ## @brief Helper method. Assumes OS is Windows and ST drivers are installed
    # @return Name of COM port where ST-Link is connected
    @staticmethod
    def detect_port():
        stm32_serial = None
        ports = serial.tools.list_ports.comports()
        for port, desc, hwid in sorted(ports):
            if "STMicroelectronics STLink Virtual COM Port" in desc:
                stm32_serial = port
            # print("{}: {} [{}]".format(port, desc, hwid))
        return stm32_serial

    ## @brief Open port to which ST-Link is connected
    # @return COM port handle
    def connect_port(self):
        ser = serial.Serial(
            port=self.detect_port(),
            baudrate=115200,
            stopbits=1,
            parity=serial.PARITY_NONE,
        )

        if not ser.isOpen():
            ser.open()
        return ser

    ## @brief Poll COM port for pressure and temperature values
    # @return Sensor readout values
    def get_sensor_values(self):
        pressure = None
        temperature = None

        while (temperature is None) or (pressure is None):
            raw_string = self.ser.readline().decode()

            if "hPa" in raw_string:
                pressure = float(raw_string.replace("hPa", ""))
            elif "deg C" in raw_string:
                temperature = float(raw_string.replace("deg C", ""))
        return pressure, temperature

    ## @brief Create new dataframes for storing sensor data
    # @return Initialized dataframes
    def initialize_dataframes(self):
        pressure, temperature = self.get_sensor_values()
        press_data = np.array([pressure])
        temp_data = np.array([temperature])
        return press_data, temp_data

    ## @brief Read sensor values and append to dataframes
    # @return Updated dataframes
    def append_dataframes(self):
        pressure, temperature = self.get_sensor_values()

        press_data = np.append(self.press_data, pressure)
        temp_data = np.append(self.temp_data, temperature)
        return press_data, temp_data

    ## @brief Continuously poll for data and update the plots
    def plot(self):
        while True:
            self.press_data, self.temp_data = self.append_dataframes()
            try:
                self.temp_plot.plot(self.temp_data, clear=True)
                self.press_plot.plot(self.press_data, clear=True)
            except Exception as ex:
                print(ex)

            pg.QtWidgets.QApplication.processEvents()


## @brief Helper function to be used with threading
def plot_thread():
    data_plot = SerialPlot()
    return data_plot


if __name__ == "__main__":
    import threading

    plot = threading.Thread(
        target=plot_thread, args=()
    )  # Create new thread running SerialPlot() class
    plot.start()  # Start the thread
    # Do something else
