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

/**
 * @brief Important defines that unlocks multiple of the functions
 * and defines from the include-files. Many of the files could be defined 
 * in the dependencies/lib-files, and therefore #ifndef #define #endif is
 * used for each single #define 
 */
#ifndef STM32_DEFINES
#define STM32_DEFINES

#ifndef STM32F767xx 
  #define STM32F767xx               /* Current MCU used for the project           */
#endif /* STM32F767xx               */  

#ifndef ARM_MATH_CM7
  #define ARM_MATH_CM7              /* Processor-version used on the STM32        */
#endif /* ARM_MATH_CM7              */

#ifndef _USE_MATH_DEFINES
  #define _USE_MATH_DEFINES         /* Gives access to math-defines such as M_PI  */
#endif /* _USE_MATH_DEFINES         */

#ifndef __SOFTFP__
  #define __SOFTFP__                /* Allows assert by software                  */
#endif /* __SOFTFP__                */

#ifndef HAL_ADC_MODULE_ENABLED
  #define HAL_ADC_MODULE_ENABLED    /* Enables ADC                                */
#endif /* HAL_ADC_MODULE_ENABLED    */ 

#ifndef HAL_GPIO_MODULE_ENABLED
  #define HAL_GPIO_MODULE_ENABLED   /* Enables GPIO                               */
#endif /* HAL_GPIO_MODULE_ENABLED   */

#ifndef HAL_ETH_MODULE_ENABLED
  #define HAL_ETH_MODULE_ENABLED    /* Enables ETH                                */
#endif /* HAL_ETH_MODULE_ENABLED    */

#ifndef HAL_EXTI_MODULE_ENABLED
  #define HAL_EXTI_MODULE_ENABLED   /* Enables interrupt when GPIO is toggling    */
#endif /* HAL_EXTI_MODULE_ENABLED   */

#ifndef HAL_DMA_MODULE_ENABLED
  #define HAL_DMA_MODULE_ENABLED    /* Enables DMA                                */
#endif /* HAL_DMA_MODULE_ENABLED    */

#ifndef HAL_RCC_MODULE_ENABLED
  #define HAL_RCC_MODULE_ENABLED    /* Enables peripheral clocks                  */
#endif /* HAL_RCC_MODULE_ENABLED    */

#ifndef HAL_FLASH_MODULE_ENABLED
  #define HAL_FLASH_MODULE_ENABLED  /* Enables flash                              */
#endif /* HAL_FLASH_MODULE_ENABLED  */

#ifndef HAL_PWR_MODULE_ENABLED  
  #define HAL_PWR_MODULE_ENABLED    /* Enables power-control                      */
#endif /* HAL_PWR_MODULE_ENABLED */

#ifndef HAL_I2C_MODULE_ENABLED  
  #define HAL_I2C_MODULE_ENABLED    /* Enables I2C                                */
#endif /* HAL_I2C_MODULE_ENABLED    */

#ifndef HAL_CORTEX_MODULE_ENABLED
  #define HAL_CORTEX_MODULE_ENABLED /* Enables driver for Cortex M7               */
#endif /* HAL_CORTEX_MODULE_ENABLED */

#ifndef __IO
  #define __IO volatile             /* Low-level macro for compilation            */
#endif /* __IO */

#if __STDC_HOSTED__                 /* 1 if C-library in hosted environment       */
  #undef __STDC_HOSTED__            /* Reset define                               */
  #define __STDC_HOSTED__ 0         /* Define to 0, since not hosted environment  */
#endif /* __STDC_HOSTED__           */

#endif /* STM32_DEFINES */

/** 
 * Includes
 */
#include "stm32f7xx.h"
#include "stm32f767xx.h"
#include "system_stm32f7xx.h"

/**
 * @brief Enum to hold some of the potential errors that could occur
 * 
 * @warning ERROR_INVALID_SIGNAL is most likely on occuring, as it 
 * is calculated multiple times a second. Must prevent it from 
 * eliminating the RAM/memory
 */
typedef enum{
  ERROR_ADC_INIT,             /* Error on initializing ADC                          */
  ERROR_ADC_CONFIG,           /* Error on configuring ADC                           */
  ERROR_DMA_INIT,             /* Error on initializing DMA                          */
  ERROR_DMA_CONFIG,           /* Error on configuring DMA                           */
  ERROR_DMA_START,            /* Error while starting DMA                           */
  ERROR_DMA_STOP,             /* Error while stopping DMA                           */
  ERROR_SPI_INIT,             /* Error on initializing SPI                          */
  ERROR_ETH_INIT,             /* Error on initializing ETH                          */
  ERROR_TRILATERATION_INIT,   /* Error on initializing TRILATERATION                */
  ERROR_TIME_SIGNAL,          /* Error on calculating invalid time of signals       */
  ERROR_INTENSITY_SIGNAL,     /* Error on calculating invalid intensity of signals  */
  ERROR_UNIDENTIFIED,         /* Unidentified error. Thrown using Error_handler     */
  ERROR_MEMORY                /* Out of memory for error_handling                   */
}ERROR_TYPES; /* enum ERROR_TYPES */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
