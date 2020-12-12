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

#include "hydrophones.h"
#include <stdint.h>

#include "stm32f7xx_hal.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc;

uint32_t ADC1ConvertedValues[DSP_CONSTANTS::DMA_BUFFER_LENGTH];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ETH_Init(void);
static void MX_SPI1_Init(void);

// WARNING: Not implemented!!
static void read_ADC(
            float32_t* data_hyd_port, 
            float32_t* data_hyd_starboard,
            float32_t* data_hyd_stern);


// Function to be implemented later
// Gives an order over ethernet
uint8_t ethernet_coordination(void);

/**
  * @brief  The application entry point.
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ETH_Init();
  MX_SPI1_Init();
  
  /* USER CODE BEGIN 2 */
	if (HAL_ADC_Start(&hadc) != HAL_OK)
	  Error_Handler();

  HYDROPHONES::Hydrophones hyd_port(HYDROPHONES::pos_hyd_port);
  HYDROPHONES::Hydrophones hyd_starboard(HYDROPHONES::pos_hyd_starboard);
  HYDROPHONES::Hydrophones hyd_stern(HYDROPHONES::pos_hyd_stern);

  // Lag from each hydrophone
  uint32_t lag_hyd_port, lag_hyd_starboard, lag_hyd_stern;

  // Intensity measured for each hydrophone
  float32_t intensity_port, intensity_starboard, intensity_stern;

  // Range-estimate based on some calculation
  float32_t range_es_port, range_es_starboard, range_es_stern;

  // Intializing the raw-data-arrays
  float32_t* c_data_hyd_port = 
        (float32_t*) malloc(sizeof(float32_t) * DSP_CONSTANTS::DMA_BUFFER_LENGTH);
  float32_t* c_data_hyd_starboard = 
        (float32_t*) malloc(sizeof(float32_t) * DSP_CONSTANTS::DMA_BUFFER_LENGTH);
  float32_t* c_data_hyd_stern = 
        (float32_t*) malloc(sizeof(float32_t) * DSP_CONSTANTS::DMA_BUFFER_LENGTH);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(true){

      /** 
      * Using the ethernet to decleare the MCU to start recording.
      * Start a time-estimate, such that the Xavier knows how old 
      * the estimated position is 
      * 
      * NOTE: Requires more logic here! Otherwise it will be a bug,
      * since the system will not move further 
      */
      if(ethernet_coordination()){
        // Do something if given an order over ethernet
        // Must be implemented further
      }

      // Getting data from the pins
      read_ADC(c_data_hyd_port, c_data_hyd_starboard, c_data_hyd_stern);

      // Calculating the lag
      hyd_port.analyze_data(c_data_hyd_port);
      hyd_starboard.analyze_data(c_data_hyd_starboard);
      hyd_stern.analyze_data(c_data_hyd_stern);

      lag_hyd_port = hyd_port.get_lag();
      lag_hyd_starboard = hyd_starboard.get_lag();
      lag_hyd_stern = hyd_stern.get_lag();

      // Checking if the lag is valid
      // Take new sample if not valid data
      /**
       * NOTE: The intensity is not implemented as of 10.12.2020
       */
      if(!TRILITERATION::check_valid_signals(lag_hyd_port,
            lag_hyd_starboard, lag_hyd_stern, 0, 0, 0)){
        continue;
      }

      // Calculate an estimate for the range
      intensity_port = hyd_port.get_intensity();
      intensity_starboard = hyd_starboard.get_intensity();
      intensity_stern = hyd_stern.get_intensity();

      range_es_port = hyd_port.get_lag();
      range_es_starboard = hyd_starboard.get_lag();
      range_es_stern = hyd_stern.get_lag();

      // Calculate estimate 
      std::pair<float32_t, float32_t> position_es = 
          TRILITERATION::estimate_pinger_position(lag_hyd_port,
            lag_hyd_starboard, lag_hyd_stern, intensity_stern,
            intensity_starboard, intensity_stern);

      // Do something with the estimates

      // Send the data to the Xavier to get the possible direction and range
  }
  free(c_data_hyd_port);
  free(c_data_hyd_starboard);
  free(c_data_hyd_stern);

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
  RCC_OscInitStruct.PLL.PLLQ = 6;
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;  // Affect the sampling frequency of the ADC. With ..._DIV4, we get
                                                     // a sample frequency of 450 kHz. This results in 112,5 kHz per 
                                                     // hydrophone

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
  heth.Init.MACAddr[0] =   0x00;
  heth.Init.MACAddr[1] =   0x80;
  heth.Init.MACAddr[2] =   0xE1;
  heth.Init.MACAddr[3] =   0x00;
  heth.Init.MACAddr[4] =   0x00;
  heth.Init.MACAddr[5] =   0x00;
  heth.Init.RxMode = ETH_RXPOLLING_MODE;
  heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

}


/* USER CODE BEGIN 4 */
static void read_ADC(
            float32_t* data_hyd_port, 
            float32_t* data_hyd_starboard,
            float32_t* data_hyd_stern){
  for(int i = 0; i < DSP_CONSTANTS::WORKING_BUFFER_LENGTH; i++){
    // This is inefficient. Might be better to use the DMA to 
    // read continously... 

    // HAL_ADC_Start(&hadc);
    // HAL_ADC_PollForConversion(&hadc, 5);
    // adc_6 = HAL_ADC_GetValue(&hadc);
    // HAL_ADC_Start(&hadc)
    // HAL_ADC_PollForConversion(&hadc, 5);
    // adc_7 = HAL_ADC_GetValue(&hadc);

    // //last ADC in the sequence
    // HAL_ADC_Start(&hadc);
    // HAL_ADC_PollForConversion(&hadc, 5);
    // adc_9 = HAL_ADC_GetValue(&hadc);
  }

  for(int i = 0; i < DSP_CONSTANTS::DMA_BUFFER_LENGTH; i++){
    // for(int i = 0; i < DSP_CONSTANTS::WORKING_BUFFER_LENGTH; i++){
    //   input[2*i] = ADC1ConvertedValues[i];
    //   input[2*i+1] = 0; //Imaginary part set to zero
    // }
    /**
     * @warning MUST BE UPDATED!!
     */
    data_hyd_port[i] = 0;
    data_hyd_starboard[i] = 0;
    data_hyd_stern[i] = 0;
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void){
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

/**
 * @brief Function to handle coordination with the through ethernet
 * 
 * @warning Not implemented as of 11.12.2020, as I am not sure how the
 * communication between the STM32 and the main CPU should be
 * 
 */
uint8_t ethernet_coordination(void){
  return 0;
}


#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line){
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
