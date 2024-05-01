/**
 * @file BMP280.h
 * @brief BMP280 driver file header
 *
 *  Created on: Apr 5, 2024 \n
 *      Author: Piotr Jucha
 */

#pragma once

#include "stm32f1xx_hal.h"
#include <stdbool.h>

/**
 * @brief Initialize sensor with chosen settings
 * @param osrs_t Temperature oversampling setting
 * @param osrs_p Pressure oversampling setting
 * @param acq_mode Acquisition mode setting
 * @param t_sb Standby time setting
 * @param filter_tc Sensor IIR Filter time constant setting
 * @param i2c_handle Desired MCU I2C peripheral for communication with sensor
 * @param device_address I2C device address
 * @return Configuration status\n
 * false == unsuccessful\n
 * true == successful
 */
bool BMP280_Init_I2C(uint8_t osrs_t,
                     uint8_t osrs_p,
                     uint8_t acq_mode,
                     uint8_t t_sb,
                     uint8_t filter_tc,
                     I2C_HandleTypeDef i2c_handle,
                     uint8_t device_address);

/**
 * @brief Read sensor calibration parameters over I2C
 * @param i2c_handle Desired MCU I2C peripheral for communication with sensor
 * @param device_address I2C device address
 */
void BMP280_CalibrationConstantsRead_I2C(I2C_HandleTypeDef i2c_handle,
                                         uint8_t device_address);

/**
 * @brief Wake sensor over I2C - used when measuring in forced mode
 * @param i2c_handle Desired MCU I2C peripheral for communication with sensor\n
 * @param device_address I2C device address\n
 * BMP280_DEVICE_ADDRESS_GND = 0x76\n
 * BMP280_DEVICE_ADDRESS_VDDIO = 0x77
 * @return Wake status\n
 * false == unsuccessful\n
 * true == successful
 */
bool BMP280_Wake_I2C(I2C_HandleTypeDef i2c_handle, uint8_t device_address);

/**
 * @brief Measure temperature and pressure over I2C
 * @param i2c_handle Desired MCU I2C peripheral for communication with sensor
 * @param device_address I2C device address\n
 * BMP280_DEVICE_ADDRESS_GND = 0x76\n
 * BMP280_DEVICE_ADDRESS_VDDIO = 0x77
 * @return Measurement values
 */
float BMP280_Measure_I2C(I2C_HandleTypeDef i2c_handle, uint8_t device_address);

/**
 * \name Sensor I2C addresses
 */
//@{
#define BMP280_DEVICE_ADDRESS_GND 0x76 << 1   /**< Used when SDO pulled down */
#define BMP280_DEVICE_ADDRESS_VDDIO 0x77 << 1 /**< Used when SDO pulled up */
//@}

/**
 *  \name Raw temperature data registers addresses, read only
 */
//@{
#define BMP280_REG_TEMP_XLSB 0xFC /**< LSB temperature data chunk */
#define BMP280_REG_TEMP_LSB 0xFB  /**< Middle temperature data chunk */
#define BMP280_REG_TEMP_MSB 0xFA  /**< MSB temperature data chunk */
//@}

/**
 *  \name Raw pressure data registers addresses, read only
 */
//@{
#define BMP280_REG_PRESS_XLSB 0xF9 /**< LSB pressure data chunk */
#define BMP280_REG_PRESS_LSB 0xF8  /**< Middle pressure data chunk */
#define BMP280_REG_PRESS_MSB 0xF7  /**< MSB pressure data chunk */
//@}

/**
 * \name Control registers addresses, read+write
 */
//@{
#define BMP280_REG_CONFIG 0xF5    /**< Configuration register */
#define BMP280_REG_CTRL_MEAS 0xF4 /**< Data acquisition mode register */
//@}

/**
 * \name Status register address, do not write
 */
#define BMP280_REG_STATUS 0xF3 /**< Device status register */

/**
 * \name Reset register address, write only
 */
#define BMP280_REG_RESET 0xE0 /**< Device reset register */

/**
 * \name Device ID register address, read only
 */
#define BMP280_REG_ID 0xD0 /**< DEVID register */

/**
 * \name Calibration registers addresses, read only
 */
#define BMP280_REG_CALIB25 0xA1 /**< Calibration register 25 */
#define BMP280_REG_CALIB24 0xA0 /**< Calibration register 24 */
#define BMP280_REG_CALIB23 0x9F /**< Calibration register 23 */
#define BMP280_REG_CALIB22 0x9E /**< Calibration register 22 */
#define BMP280_REG_CALIB21 0x9D /**< Calibration register 21 */
#define BMP280_REG_CALIB20 0x9C /**< Calibration register 20 */
#define BMP280_REG_CALIB19 0x9B /**< Calibration register 19 */
#define BMP280_REG_CALIB18 0x9A /**< Calibration register 18 */
#define BMP280_REG_CALIB17 0x99 /**< Calibration register 17 */
#define BMP280_REG_CALIB16 0x98 /**< Calibration register 16 */
#define BMP280_REG_CALIB15 0x97 /**< Calibration register 15 */
#define BMP280_REG_CALIB14 0x96 /**< Calibration register 14 */
#define BMP280_REG_CALIB13 0x95 /**< Calibration register 13 */
#define BMP280_REG_CALIB12 0x94 /**< Calibration register 12 */
#define BMP280_REG_CALIB11 0x93 /**< Calibration register 11 */
#define BMP280_REG_CALIB10 0x92 /**< Calibration register 10 */
#define BMP280_REG_CALIB09 0x91 /**< Calibration register 9 */
#define BMP280_REG_CALIB08 0x90 /**< Calibration register 8 */
#define BMP280_REG_CALIB07 0x8F /**< Calibration register 7 */
#define BMP280_REG_CALIB06 0x8E /**< Calibration register 6 */
#define BMP280_REG_CALIB05 0x8D /**< Calibration register 5 */
#define BMP280_REG_CALIB04 0x8C /**< Calibration register 4 */
#define BMP280_REG_CALIB03 0x8B /**< Calibration register 3 */
#define BMP280_REG_CALIB02 0x8A /**< Calibration register 2 */
#define BMP280_REG_CALIB01 0x89 /**< Calibration register 1 */
#define BMP280_REG_CALIB00 0x88 /**< Calibration register 0 */

/**
 * \name Valid ID register value
 */
#define BMP280_VAL_DEVID 0x58 /**< Device ID */

/**
 * \name Valid RESET register value
 */
#define BMP280_VAL_RESET 0xB6 /**< Writing to reset register resets device */

/**
 * \name Valid STATUS register flags masks
 */
#define BMP280_VAL_STATUS_MEASURING 1U << 3 /**< Measuring busy flag */
#define BMP280_VAL_STATUS_IM_UPDATE 1U << 0 /**< NVM copying status flag */

/**
 * \name Valid CTRL_MEAS register acquisition option values
 */
#define BMP280_VAL_CTRL_MEAS_OSRS_T_0 0b00000000  /**< Disabled measurement */
#define BMP280_VAL_CTRL_MEAS_OSRS_T_1 0b00100000  /**< 1x oversampling */
#define BMP280_VAL_CTRL_MEAS_OSRS_T_2 0b01000000  /**< 2x oversampling */
#define BMP280_VAL_CTRL_MEAS_OSRS_T_4 0b01100000  /**< 4x oversampling */
#define BMP280_VAL_CTRL_MEAS_OSRS_T_8 0b10000000  /**< 8x oversampling */
#define BMP280_VAL_CTRL_MEAS_OSRS_T_16 0b11100000 /**< 16x oversampling */

#define BMP280_VAL_CTRL_MEAS_OSRS_P_0 0b00000000  /**< Disabled measurement */
#define BMP280_VAL_CTRL_MEAS_OSRS_P_1 0b00000100  /**< 1x oversampling */
#define BMP280_VAL_CTRL_MEAS_OSRS_P_2 0b00001000  /**< 2x oversampling */
#define BMP280_VAL_CTRL_MEAS_OSRS_P_4 0b00001100  /**< 4x oversampling */
#define BMP280_VAL_CTRL_MEAS_OSRS_P_8 0b00010000  /**< 8x oversampling */
#define BMP280_VAL_CTRL_MEAS_OSRS_P_16 0b00011100 /**< 16x oversampling */

#define BMP280_VAL_CTRL_MEAS_MODE_SLEEP 0b00000000  /**< Sleep mode */
#define BMP280_VAL_CTRL_MEAS_MODE_FORCED 0b00000001 /**< Forced mode */
#define BMP280_VAL_CTRL_MEAS_MODE_NORMAL 0b00000011 /**< Continuous mode */

#define BMP280_VAL_CTRL_CONFIG_T_SB_0_5 0b00000000  /**< Standby 0.5ms */
#define BMP280_VAL_CTRL_CONFIG_T_SB_62_5 0b00100000 /**< Standby 62.25ms */
#define BMP280_VAL_CTRL_CONFIG_T_SB_125 0b01000000  /**< Standby 125ms */
#define BMP280_VAL_CTRL_CONFIG_T_SB_250 0b01100000  /**< Standby 250ms */
#define BMP280_VAL_CTRL_CONFIG_T_SB_500 0b10000000  /**< Standby 500ms */
#define BMP280_VAL_CTRL_CONFIG_T_SB_1000 0b10100000 /**< Standby 1000ms */
#define BMP280_VAL_CTRL_CONFIG_T_SB_2000 0b11000000 /**< Standby 2000ms */
#define BMP280_VAL_CTRL_CONFIG_T_SB_4000 0b11100000 /**< Standby 4000ms */

#define BMP280_VAL_CTRL_CONFIG_FILTER_0 0b00000000  /**< Filter disabled */
#define BMP280_VAL_CTRL_CONFIG_FILTER_2 0b00000100  /**< 2x filter */
#define BMP280_VAL_CTRL_CONFIG_FILTER_4 0b00001000  /**< 4x filter */
#define BMP280_VAL_CTRL_CONFIG_FILTER_8 0b00001100  /**< 8x filter */
#define BMP280_VAL_CTRL_CONFIG_FILTER_16 0b00010000 /**< 16x filter */

#define BMP280_VAL_CTRL_SPI3W_EN 0b00000001 /**< Enable SPI 3-wire */

/**
 * \name MCU specific setting - affects pressure processing formula
 */
//@{
#define RETURN_64BIT true  /**< for MCU with 64-bit operations support */
#define RETURN_32BIT false /**< for MCU without 64-bit operations support */
                           //@}

/* INC_BMP280_H_ */
