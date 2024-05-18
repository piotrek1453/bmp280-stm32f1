# bmp280-stm32f1


Simple BMP280 driver embedded in example configured STM32CubeIDE project for STM32F103.


It should work with any STM32 MCU after replacing "#include "stm32f1xx_hal.h"" in BMP280.h with correct one.

## Wiring
For this code to run "out of the box" you need an STM32F103 and BMP280 sensor connected as follows:

Sensor<->MCU


VCC <-> 3V3


GND<->GND


SCL<->B6 (I2C1 clock)


SDA<->B7 (I2C1 data)


CSB<->3V3 (Disable SPI on sensor)


SDD<->GND (Set sensor address to 0x76)