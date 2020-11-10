/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "eth.h"
#include "gpio.h"

#include "hydrophones.hpp"
#include <stdint.h>


// Function to be implemented later
// Gives an order over ethernet
uint8_t ethernet_order();


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_ETH_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HYDROPHONES::Hydrophones hyd_port(HYDROPHONES::pos_hyd_port);
  HYDROPHONES::Hydrophones hyd_starboard(HYDROPHONES::pos_hyd_starboard);
  HYDROPHONES::Hydrophones hyd_stern(HYDROPHONES::pos_hyd_stern);

  // Lag from each hydrophone
  uint32_t lag_hyd_port, lag_hyd_starboard, lag_hyd_stern;

  // Intensity measured for each hydrophone
  double intensity_port, intensity_starboard, intensity_stern;

  // Range-estimate based on some calculation
  double range_es_port, range_es_starboard, range_es_stern;

  // Intializing the raw-data-arrays
  uint16_t c_data_hyd_port[DSP::interval_total_len];
  uint16_t c_data_hyd_starboard[DSP::interval_total_len];
  uint16_t c_data_hyd_stern[DSP::interval_total_len];

  // Simple bool to keep track of the state
  bool invalid_data = false;

  while(true){


      /** 
      * Using the ethernet to decleare the MCU to start recording.
      * Start a time-estimate, such that the Xavier knows how old 
      * the estimated position is 
      * 
      * NOTE: Requires more logic here! Otherwise it will be a bug,
      * since the system will not move further 
      */
      while(!ethernet_order() && !invalid_data);


      // Getting data from the pins


      // Calculating the lag
      hyd_port.calculate_lag(c_data_hyd_port);
      hyd_starboard.calculate_lag(c_data_hyd_starboard);
      hyd_stern.calculate_lag(c_data_hyd_stern);

      lag_hyd_port = hyd_port.get_lag();
      lag_hyd_starboard = hyd_starboard.get_lag();
      lag_hyd_stern = hyd_stern.get_lag();

      // Checking if the lag is valid
      // Take new sample if not valid data
      if(!TRILITERATION::check_valid_signals(lag_hyd_port,
            lag_hyd_starboard, lag_hyd_stern)){
        invalid_data = true;
        continue;
      }
      invalid_data = false;

      // Calculate an estimate for the range
      intensity_port = hyd_port.get_intensity();
      intensity_starboard = hyd_starboard.get_intensity();
      intensity_stern = hyd_stern.get_intensity();

      range_es_port = hyd_port.get_lag();
      range_es_starboard = hyd_starboard.get_lag();
      range_es_stern = hyd_stern.get_lag();

      // Calculate estimate 
      std::pair<double, double> position_es = 
          TRILITERATION::estimate_pinger_position(lag_hyd_port,
            lag_hyd_starboard, lag_hyd_stern, intensity_stern,
            intensity_starboard, intensity_stern);

      // Do something with the estimates

      // Send the data to the Xavier to get the possible direction and range
  }

  return 0;
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
