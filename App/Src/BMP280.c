/**
 * @file BMP280.c
 * @brief BMP280 driver file
 *
 *  Created on: Apr 5, 2024 \n
 *      Author: Piotr Jucha
 */

#include "BMP280.h"

static int32_t rawTemperature, rawPressure;

static struct BMP280_Result result;

static const struct BMP280_Result noResult = {0.0, 0.0};

static uint16_t dig_T1, dig_P1;
static int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7,
    dig_P8, dig_P9;

static inline HAL_StatusTypeDef
BMP280_RawDataRead_I2C(I2C_HandleTypeDef i2c_handle, uint8_t device_address);

static inline int32_t BMP280_calculate_T_int32(int32_t adc_T);

#if RETURN_64BIT
static inline uint32_t BMP280_calculate_P_int64(int32_t adc_P);
#elif RETURN_32BIT
static inline uint32_t BMP280_calculate_P_int32(int32_t adc_P);
#endif

/**
 * Read constants used for temperature and pressure calculations from
 * sensor's memory
 */
//@{
void BMP280_CalibrationConstantsRead_I2C(I2C_HandleTypeDef i2c_handle,
                                         uint8_t device_address) {
  uint8_t calibrationConstantsRaw[26];

  HAL_I2C_Mem_Read(&i2c_handle,
                   device_address,
                   BMP280_REG_CALIB00,
                   1,
                   calibrationConstantsRaw,
                   26,
                   HAL_MAX_DELAY);

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

} //@}

/**
 * Reset the sensor, check if device ID is valid, read calibration constants,
 * write oversampling, acquisition mode, readout timing and filter data to the
 * sensor
 */
//@{
bool BMP280_Init_I2C(uint8_t osrs_t,
                     uint8_t osrs_p,
                     uint8_t acq_mode,
                     uint8_t t_sb,
                     uint8_t filter_tc,
                     I2C_HandleTypeDef i2c_handle,
                     uint8_t device_address) {
  uint8_t writeBuffer, readBuffer; // Variables used for applying changes to
                                   // selected bits in device registers */
  HAL_StatusTypeDef status;

  // Reset the device
  writeBuffer = 0xB6;
  status = HAL_I2C_Mem_Write(&i2c_handle,
                             device_address,
                             BMP280_REG_RESET,
                             1,
                             &writeBuffer,
                             1,
                             HAL_MAX_DELAY);
  if (status != HAL_OK) {
    return false;
  }
  HAL_Delay(100);

  // Read device ID
  status = HAL_I2C_Mem_Read(&i2c_handle,
                            device_address,
                            BMP280_REG_ID,
                            1,
                            &readBuffer,
                            1,
                            HAL_MAX_DELAY);
  if (status != HAL_OK || readBuffer != BMP280_VAL_DEVID) {
    return false;
  }

  // Read calibration constants
  BMP280_CalibrationConstantsRead_I2C(i2c_handle, device_address);

  // Write timing and IIR data to config register
  writeBuffer = (t_sb << 5) | (filter_tc << 2);
  status = HAL_I2C_Mem_Write(&i2c_handle,
                             device_address,
                             BMP280_REG_CONFIG,
                             1,
                             &writeBuffer,
                             1,
                             HAL_MAX_DELAY);
  status = HAL_I2C_Mem_Read(&i2c_handle,
                            device_address,
                            BMP280_REG_CONFIG,
                            1,
                            &readBuffer,
                            1,
                            HAL_MAX_DELAY);
  if (readBuffer != writeBuffer || status != HAL_OK) {
    return false;
  }

  // Write oversampling and mode data to ctrl_meas register
  writeBuffer = (osrs_t << 5) | (osrs_p << 2) | (acq_mode << 0);
  status = HAL_I2C_Mem_Write(&i2c_handle,
                             device_address,
                             BMP280_REG_CTRL_MEAS,
                             1,
                             &writeBuffer,
                             1,
                             HAL_MAX_DELAY);
  status = HAL_I2C_Mem_Read(&i2c_handle,
                            device_address,
                            BMP280_REG_CTRL_MEAS,
                            1,
                            &readBuffer,
                            1,
                            HAL_MAX_DELAY);
  if (readBuffer != writeBuffer || status != HAL_OK) {
    return false;
  }

  return true;
} //@}

/**
 * Wake sensor by writing MEASURE_MODE_FORCED bit to CTRL_MEAS register
 */
bool BMP280_Wake_I2C(I2C_HandleTypeDef i2c_handle, uint8_t device_address) {
  uint8_t buffer; // Helper variable, used to prevent overwriting other data in
                  // CTRL_MEAS register
  HAL_StatusTypeDef status;

  // Read device ID
  status = HAL_I2C_Mem_Read(
      &i2c_handle, device_address, BMP280_REG_ID, 1, &buffer, 1, HAL_MAX_DELAY);
  if (status != HAL_OK || buffer != 0x58) {
    return false;
  }

  status = HAL_I2C_Mem_Read(&i2c_handle,
                            device_address,
                            BMP280_REG_CTRL_MEAS,
                            1,
                            &buffer,
                            1,
                            HAL_MAX_DELAY);
  if (status != HAL_OK) {
    return false;
  }

  buffer |= BMP280_VAL_CTRL_MEAS_MODE_FORCED;

  status = HAL_I2C_Mem_Write(&i2c_handle,
                             device_address,
                             BMP280_REG_CTRL_MEAS,
                             1,
                             &buffer,
                             1,
                             HAL_MAX_DELAY);
  if (status != HAL_OK) {
    return false;
  }

  return true;
}

struct BMP280_Result BMP280_Measure_I2C(I2C_HandleTypeDef i2c_handle,
                                        uint8_t device_address) {

  if (BMP280_RawDataRead_I2C(i2c_handle, device_address) == HAL_OK) {
    if (rawTemperature == 0x80000) {
      result.Temperature = 0; // value in case temp measurement was disabled
    } else {
      result.Temperature = (BMP280_calculate_T_int32(rawTemperature)) /
                           100.0; // as per datasheet, the temp is x100
    }

    if (rawPressure == 0x80000)
      result.Pressure = 0; // value in case temp measurement was disabled
    else {
#if RETURN_64BIT
      result.Pressure = (BMP280_calculate_P_int64(rawPressure)) /
                        256.0; // as per datasheet, the pressure is x256

#elif RETURN_32BIT
      result.Pressure = (BMP280_calculate_P_int32(
          rawPressure)); // as per datasheet, the pressure is Pa

#endif
    }
    return result;
  }

  // if the device is detached
  else {
    return noResult;
  }
}

static inline HAL_StatusTypeDef
BMP280_RawDataRead_I2C(I2C_HandleTypeDef i2c_handle, uint8_t device_address) {
  HAL_StatusTypeDef status;
  uint8_t MeasurementStatus = {0}, RawData[6] = {0};

  do {
    status = HAL_I2C_Mem_Read(&i2c_handle,
                              device_address,
                              BMP280_REG_STATUS,
                              1,
                              &MeasurementStatus,
                              1,
                              HAL_MAX_DELAY);
  } while (MeasurementStatus & 0b00001000); // Wait for measurement to finish

  status = HAL_I2C_Mem_Read(&i2c_handle,
                            device_address,
                            BMP280_REG_PRESS_MSB,
                            1,
                            RawData,
                            6,
                            HAL_MAX_DELAY);

  rawPressure = RawData[0] << 12 | RawData[1] << 4 | RawData[2] >> 4;
  rawTemperature = RawData[3] << 12 | RawData[4] << 4 | RawData[5] >> 4;

  return status;
}

/************* COMPENSATION CALCULATION AS PER DATASHEET (page 25)
 * **************************/

/* Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123”
   equals 51.23 DegC. t_fine carries fine temperature as global value
*/
int32_t t_fine;
static inline int32_t BMP280_calculate_T_int32(int32_t adc_T) {
  int32_t var1, var2, T;
  // compensate
  var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
            ((adc_T >> 4) - ((int32_t)dig_T1))) >>
           12) *
          ((int32_t)dig_T3)) >>
         14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  // calculate
  return T;
}

#if RETURN_64BIT
/* Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer
   bits and 8 fractional bits). Output value of “24674867” represents
   24674867/256 = 96386.2 Pa = 963.862 hPa
*/
static inline uint32_t BMP280_calculate_P_int64(int32_t adc_P) {
  int64_t var1, var2, p;
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)dig_P6;
  var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
  var2 = var2 + (((int64_t)dig_P4) << 35);
  var1 =
      ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
  return (uint32_t)p;
}

#elif RETURN_32BIT
// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386”
// equals 96386 Pa = 963.86 hPa
static inline uint32_t BMP280_calculate_P_int32(int32_t adc_P) {
  int32_t var1, var2;
  uint32_t p;
  var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)dig_P6);
  var2 = var2 + ((var1 * ((int32_t)dig_P5)) << 1);
  var2 = (var2 >> 2) + (((int32_t)dig_P4) << 16);
  var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) +
          ((((int32_t)dig_P2) * var1) >> 1)) >>
         18;
  var1 = ((((32768 + var1)) * ((int32_t)dig_P1)) >> 15);
  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
  if (p < 0x80000000) {
    p = (p << 1) / ((uint32_t)var1);
  } else {
    p = (p / (uint32_t)var1) * 2;
  }
  var1 = (((int32_t)dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
  var2 = (((int32_t)(p >> 2)) * ((int32_t)dig_P8)) >> 13;
  p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
  return p;
}
#endif
