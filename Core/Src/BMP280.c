/**
 * @file BMP280.c
 * @brief BMP280 driver file
 *
 *  Created on: Apr 5, 2024 \n
 *      Author: Piotr Jucha
 */

#include "BMP280.h"

int32_t rawTemperature, rawPressure;

uint16_t dig_T1, dig_P1;
int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8,
    dig_P9;

bool BMP280_InitI2C(uint8_t osrs_t, uint8_t osrs_p, uint8_t acq_mode,
                    uint8_t t_sb, uint8_t filter_tc,
                    I2C_HandleTypeDef i2c_handle) {}

void BMP280_CalibrationConstantsRead_I2C(I2C_HandleTypeDef i2c_handle,
                                         uint8_t device_address) {
  uint8_t calibrationConstantsRaw[26];

  HAL_I2C_Mem_Read_DMA(&i2c_handle, device_address, BMP280_REG_CALIB00, 1,
                       calibrationConstantsRaw, 26);

  dig_T1 = calibrationConstantsRaw[0] | calibrationConstantsRaw[1] << 8;
  dig_T2 = calibrationConstantsRaw[2] | calibrationConstantsRaw[3] << 8;
  dig_T3 = calibrationConstantsRaw[4] | calibrationConstantsRaw[5] << 8;

  dig_P1 = calibrationConstantsRaw[6] | calibrationConstantsRaw[7] << 8;
  dig_P2 = calibrationConstantsRaw[8] | calibrationConstantsRaw[9] << 8;
  dig_P3 = calibrationConstantsRaw[10] | calibrationConstantsRaw[11] << 8;
  dig_P4 = calibrationConstantsRaw[12] | calibrationConstantsRaw[13] << 8;
  dig_P5 = calibrationConstantsRaw[14] | calibrationConstantsRaw[15] << 8;
  dig_P6 = calibrationConstantsRaw[16] | calibrationConstantsRaw[17] << 8;
  dig_P7 = calibrationConstantsRaw[18] | calibrationConstantsRaw[19] << 8;
  dig_P8 = calibrationConstantsRaw[20] | calibrationConstantsRaw[21] << 8;
  dig_P9 = calibrationConstantsRaw[22] | calibrationConstantsRaw[23] << 8;
}

void BMP280_Wake_I2C(I2C_HandleTypeDef i2c_handle, uint8_t device_address) {
  uint8_t buffer;

  HAL_I2C_Mem_Read_DMA(&i2c_handle, device_address, BMP280_REG_CTRL_MEAS, 1,
                       &buffer, 1);
  buffer |= BMP280_VAL_CTRL_MEAS_MODE_FORCED;
  HAL_I2C_Mem_Write_DMA(&i2c_handle, device_address, BMP280_REG_CTRL_MEAS, 1,
                        &buffer, 1);
}

float BMP280_Measure_I2C(I2C_HandleTypeDef i2c_handle, uint8_t device_address) {
}
