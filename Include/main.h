/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.cpp file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f7xx_hal.h"

/**
 * @brief Enum to hold some of the potential errors that could occur
 * 
 * @warning ERROR_INVALID_SIGNAL is most likely on occuring, as it 
 * is calculated multiple times a second. Must prevent it from 
 * eliminating the RAM/memory
 */
enum class Error_types{
  ERROR_ADC_INIT,             // Error on initializing ADC
  ERROR_ADC_CONFIG,           // Error on configuring ADC 
  ERROR_DMA_INIT,             // Error on initializing DMA
  ERROR_DMA_CONFIG,           // Error on configuring DMA
  ERROR_DMA_START,            // Error while starting DMA
  ERROR_DMA_STOP,             // Error while stopping DMA
  ERROR_TRILITERATION_INIT,   // Error on initializing TRILITERATION 
  ERROR_INVALID_SIGNAL,       // Error on recieving invalid signals
  ERROR_UNIDENTIFIED,         // Unidentified error. Thrown using Error_handler 
  ERROR_MEMORY                // Out of memory for error_handling
};


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
