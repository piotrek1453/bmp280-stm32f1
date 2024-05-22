/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BMP280.h"
#include "i2c.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t vStatusTaskHandle;
struct BMP280_Result bmp280_result;
/* USER CODE END Variables */
/* Definitions for statusTask */
osThreadId_t statusTaskHandle;
const osThreadAttr_t statusTask_attributes = {
    .name = "statusTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for ledTask */
osThreadId_t ledTaskHandle;
const osThreadAttr_t ledTask_attributes = {
    .name = "ledTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for USART2TxMutex */
osMutexId_t USART2TxMutexHandle;
const osMutexAttr_t USART2TxMutex_attributes = {.name = "USART2TxMutex"};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void vStatusTask(void *argument);
void vLedTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook(void) {
  /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
  to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
  task. It is essential that code added to this hook function never attempts
  to block in any way (for example, call xQueueReceive() with a block time
  specified, or call vTaskDelay()). If the application makes use of the
  vTaskDelete() API function (as this demo application does) then it is also
  important that vApplicationIdleHook() is permitted to return to its calling
  function, because it is the responsibility of the idle task to clean up
  memory allocated by the kernel to any task that has since been deleted. */
  //  HAL_GPIO_TogglePin(ON_BOARD_LED_1_GPIO_Port, ON_BOARD_LED_1_Pin);
  //  HAL_GPIO_TogglePin(ON_BOARD_LED_2_GPIO_Port, ON_BOARD_LED_2_Pin);
  // printf("idle task\r\n");
  //  vTaskDelay(pdMS_TO_TICKS(1000));
}
/* USER CODE END 2 */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of USART2TxMutex */
  USART2TxMutexHandle = osMutexNew(&USART2TxMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of statusTask */
  statusTaskHandle = osThreadNew(vStatusTask, NULL, &statusTask_attributes);

  /* creation of ledTask */
  ledTaskHandle = osThreadNew(vLedTask, NULL, &ledTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_vStatusTask */
/**
 * @brief  Function implementing the statusTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_vStatusTask */
void vStatusTask(void *argument) {
  /* USER CODE BEGIN vStatusTask */
  /* Infinite loop */
  while (true) {
    bmp280_result = BMP280_Measure_I2C(hi2c1, BMP280_DEVICE_ADDRESS_GND);

    if (osMutexAcquire(USART2TxMutexHandle, osWaitForever) == osOK) {
      // printf("Pressure\tTemperature\r\n");
      printf("%0.2f hPa\r\n%0.2f deg C\r\n",
             bmp280_result.Pressure / 100,
             bmp280_result.Temperature);
      osMutexRelease(USART2TxMutexHandle);
    }
    osDelay(200);
  }
  /* USER CODE END vStatusTask */
}

/* USER CODE BEGIN Header_vLedTask */
/**
 * @brief Function implementing the ledTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_vLedTask */
void vLedTask(void *argument) {
  /* USER CODE BEGIN vLedTask */
  /* Infinite loop */
  uint8_t rhythm = 0;
  while (true) {
    if (rhythm < 3) {
      GPIOB->ODR ^= (1U << 2) | (1U << 13);
      ++rhythm;
      osDelay(100);
    } else if (rhythm < 6) {
      GPIOB->ODR ^= (1U << 2) | (1U << 13);
      ++rhythm;
      osDelay(500);
    } else {
      rhythm = 0;
    }
  }
  /* USER CODE END vLedTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
