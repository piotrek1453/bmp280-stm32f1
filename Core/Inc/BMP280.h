/*
 * BMP280.h
 *
 *  Created on: Apr 5, 2024
 *      Author: Piotr Jucha
 */

#pragma once

/**
 * Registers address definitions
 */
// temperature data registers, read-only
#define BMP280_REG_TEMP_XLSB 0xFC
#define BMP280_REG_TEMP_LSB 0xFB
#define BMP280_REG_TEMP_MSB 0xFA
// pressure data registers, read-only
#define BMP280_REG_PRESS_XLSB 0xF9
#define BMP280_REG_PRESS_LSB 0xF8
#define BMP280_REG_PRESS_MSB 0xF7
// control registers, read+write
#define BMP280_REG_CONFIG 0xF5
#define BMP280_REG_CTRL_MEAS 0xF4
// status register, do not write
#define BMP280_REG_STATUS 0xF3
// reset register, write only
#define BMP280_REG_RESET 0xE0
// device ID register, read-only
#define BMP280_REG_ID 0xD0
// callibration registers, read only
#define BMP280_REG_CALIB25 0xA1
#define BMP280_REG_CALIB24 0xA0
#define BMP280_REG_CALIB23 0x9F
#define BMP280_REG_CALIB22 0x9E
#define BMP280_REG_CALIB21 0x9D
#define BMP280_REG_CALIB20 0x9C
#define BMP280_REG_CALIB19 0x9B
#define BMP280_REG_CALIB18 0x9A
#define BMP280_REG_CALIB17 0x99
#define BMP280_REG_CALIB16 0x98
#define BMP280_REG_CALIB15 0x97
#define BMP280_REG_CALIB14 0x96
#define BMP280_REG_CALIB13 0x95
#define BMP280_REG_CALIB12 0x94
#define BMP280_REG_CALIB11 0x93
#define BMP280_REG_CALIB10 0x92
#define BMP280_REG_CALIB09 0x91
#define BMP280_REG_CALIB08 0x90
#define BMP280_REG_CALIB07 0x8F
#define BMP280_REG_CALIB06 0x8E
#define BMP280_REG_CALIB05 0x8D
#define BMP280_REG_CALIB04 0x8C
#define BMP280_REG_CALIB03 0x8B
#define BMP280_REG_CALIB02 0x8A
#define BMP280_REG_CALIB01 0x89
#define BMP280_REG_CALIB00 0x88

/* INC_BMP280_H_ */
