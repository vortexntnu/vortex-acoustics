/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.cpp
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
/* Private includes -----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdint.h>
#include <time.h>

#include "main.h"
#include "analyze_data.h"

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_adc.h"

#if CURR_TESTING_BOOL
  #include "testing.h"
#endif /* CURR_TESTING_BOOL */ 

/* USER CODE END Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* Handler for the ADC, DMA, ETH and SPI */
ADC_HandleTypeDef hadc1;      
DMA_HandleTypeDef hdma_adc1;
ETH_HandleTypeDef heth;       
//SPI_HandleTypeDef hspi1;        

/* Errors and errors-detected */
uint32_t error_idx = 0;
const uint16_t max_num_errors = 256;
static volatile ERROR_TYPES errors_occured[max_num_errors];

/* Memory that the DMA will push the data to */
static volatile uint32_t ADC1_converted_values[NUM_HYDROPHONES * DMA_BUFFER_LENGTH];

/* Variable used to indicate if conversion is ready. Changed via cb-function */
static volatile uint8_t bool_DMA_conv_ready = 0;

/* Variable used to indicate if an error occured during convertion. Changed via cb-function */
static volatile uint8_t bool_DMA_conv_error = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* INIT-functions */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ETH_Init(void);
//static void MX_SPI1_Init(void);

/* Overwrite of weak cb-function. Called when DMA is finished. Changes bool_DMA_conv_ready */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

/* Another function to handle interrupts which should be called when DMA is finished */
void DMA2_Stream0_IRQHandler(void);

/* Function to start the convertion over DMA */
static void start_convertion_adc_dma(void);

/* Function to access DMA to get data from the hydrophones */
static void read_measurements(
          float32_t data_hyd_port[IN_BUFFER_LENGTH], 
          float32_t data_hyd_starboard[IN_BUFFER_LENGTH],
          float32_t data_hyd_stern[IN_BUFFER_LENGTH]);

/* Functions to log errors */
static void log_error(ERROR_TYPES error_code);
static void check_signal_error(uint8_t& bool_time_error); 

/* Function to coordinate the communication over the ethernet */
uint8_t ethernet_coordination(void);

/**
  * @brief The application entry point.
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* Checks if the code is used for testing or not */
  #if CURR_TESTING_BOOL
  /**
   * Testing
   */
  TESTING::test_trilateration_algorithm();

  #else
  /**
   * Normal operations
   */

  /** 
   * Would like the system to try to restart if an error occurs.
   * The system has therefore two infinite loops, and only a power-cut (should)
   * stop the system from restarting
   * 
   * Should only calibrate the system once - the first time it starts up
   * This is due to calibration taking relatively long time
   */
  while(1){
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
    //MX_SPI1_Init();
    

    /* USER CODE BEGIN 2 */
    /** 
     * Initialize variables for trilateration 
     * Log error if invalid
     * 
     * If there is an error, this indicates that there is a more serious error
     * in the code. Breaks out of the while loop and shuts down the system if that is the case
     */
    if(!TRILATERATION::initialize_trilateration_globals()){
      log_error(ERROR_TYPES::ERROR_TRILATERATION_INIT);
      break; 
    }


    /* Initialize the matrices used for trilatiration */
    Matrix_2_3_f A_matrix = TRILATERATION::initialize_A_matrix();
    Vector_2_1_f B_vector = TRILATERATION::initialize_B_vector();


    /* TDOA between the measurements */
    uint32_t TDOA_port_starboard, TDOA_port_stern, TDOA_starboard_stern;

    uint32_t TDOA_array[NUM_HYDROPHONES] = 
          { TDOA_hyd_port, TDOA_hyd_starboard, lag_hyd_stern };


    /* Intializing the data-arrays */
    float32_t raw_data_port[IN_BUFFER_LENGTH];
    float32_t raw_data_starboard[IN_BUFFER_LENGTH];
    float32_t raw_data_stern[IN_BUFFER_LENGTH];

    float32_t* raw_data_array[NUM_HYDROPHONES] = 
          { &raw_data_port[0], &raw_data_starboard[0], &raw_data_stern[0] };

    float32_t filtered_data_port[IN_BUFFER_LENGTH];
    float32_t filtered_data_starboard[IN_BUFFER_LENGTH];
    float32_t filtered_data_stern[IN_BUFFER_LENGTH];

    float32_t* filtered_data_array[NUM_HYDROPHONES] = 
          { &filtered_data_port[0], &filtered_data_starboard[0], &filtered_data_stern[0] };


    /* Variables used to indicate error(s) with the signal */
    uint8_t bool_time_error = 0;


    /* Variable indicating the max distance between the hydrophones */
    static float32_t max_hydrophone_distance = -1.0f;


    /* Variable indicating the max allowed time-difference between the signals */
    static float32_t max_time_diff = -1.0f;


    /* Estimated position of the acoustic pinger */
    float32_t x_pos_es, y_pos_es;


    /* Starting com between ADC and DMA */
    start_convertion_adc_dma();
    
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1){
        /** 
        * Using the ethernet to decleare the MCU to start recording.
        * Start a time-estimate, such that the Xavier knows how old 
        * the estimated position is 
        * 
        * NOTE: Requires more logic here! Otherwise it will be a bug,
        * since the system will not move further 
        */
        if(ethernet_coordination()){
          /** 
           * TODO@TODO
           * 
           * Do something if given an order over ethernet
           * Must be implemented further and in coordination with the team
           * making the code/implementation for the Xavier
           */
        }

        /* (Re)starting DMA */
        // if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) ADC1_converted_values, 
        //     NUM_HYDROPHONES * DMA_BUFFER_LENGTH) != HAL_OK){
        //   log_error(ERROR_TYPES::ERROR_DMA_START);
        //   continue;
        // }

        /** 
         * Waiting for the DMA to be ready
         * 
         * The variables @p bool_DMA_conv_ready and @p bool_DMA_conv_error are 
         * changed via interrupt/cb-function
         */
        while(!bool_DMA_conv_ready && !bool_DMA_conv_error);

        /**
         * Checking if an error occured during convertion 
         * 
         * Restarting the convertion between ADC and memory
         */
        if(bool_DMA_conv_error){
          log_error(ERROR_TYPES::ERROR_DMA_CONV);
          bool_DMA_conv_error = 0;
          bool_DMA_conv_ready = 0;
          start_convertion_adc_dma();
          continue;
        }

        /**
         * Stopping the DMA to prevent overwriting the memory
         */
        // if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK){     
        //   log_error(ERROR_TYPES::ERROR_DMA_STOP);
        //   continue;
        // }

        /** 
         * Reading the data from the ADC 
         * 
         * The data should be correct, as the DMA-transfer has stopped. It should
         * therefore be impossible to overwrite the memory
         */
        read_measurements(data_hyd_port, data_hyd_starboard, data_hyd_stern);

        /**
         * Recording the time of measurement in seconds after startup
         *
         * This should be synchronized with the Xavier, such that the main system
         * knows when the measurements where taken and could act accordingly
         */
        //float32_t time_measurement = (float32_t)difftime(time(NULL), time_initial_startup);

        /**
         * Data is transferred to other memory
         * 
         * Restarting reading and transfer again, such that new data is ready almost immediately
         * when the CPU has processed the old data
         */
        start_convertion_adc_dma();

        /* Filtering the raw data */
        ANALYZE_DATA::filter_raw_data(raw_data_array, filtered_data_array);

        /* Calulating the TDOA-array */
        ANALYZE_DATA::calculate_TDOA_array(filtered_data_array, TDOA_array);

        /**
         * Checking is the measurements are valid. The measurements 
         * are discarded if they deviate too much in either time lag
         * 
         * Take new samples if the data is invalid
         */
        if(!TRILATERATION::check_valid_signals(TDOA_array, bool_time_error)){
          log_error(ERROR_TYPES::ERROR_TIME_SIGNAL);
          continue;
        }

        /**
         * Triliterate the position of the acoustic pinger
         * 
         * The coordinates are given as a reference to the center of the AUV
         */
        if(!TRILATERATION::trilaterate_pinger_position(A_matrix, B_vector, 
            lag_array, x_pos_es, y_pos_es)){
          log_error(ERROR_TYPES::ERROR_A_NOT_INVERTIBLE);
          continue;
        }

        /**
         * TODO@TODO
         * 
         * Send the data to the Xavier in a predetermined format
         * 
         * Required to send the estimated position of the acoustic pinger
         * alongside the time of measurment
         */
    }

    /* Stopping the ADC and the DMA for safety */
    if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK){     
      log_error(ERROR_TYPES::ERROR_DMA_STOP);
      continue;
    }
  }

  /**
   * TODO@TODO
   * 
   * If broken out of the loop by an error and not in testing
   * 
   * Send an error-report to the Xavier
   */
  #endif /* CURR_TESTING_BOOL */

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

  /* 
  Affect the sampling frequency of the ADC. By using ..._DIV16, we get
  a sample frequency of 112.5 kHz
  */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV16;  

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
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    log_error(ERROR_TYPES::ERROR_ADC_INIT);
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    log_error(ERROR_TYPES::ERROR_ADC_CONFIG);
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    log_error(ERROR_TYPES::ERROR_ADC_CONFIG);
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    log_error(ERROR_TYPES::ERROR_ADC_CONFIG);
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
  * 
  * @warning Some include-errors here. Commented out until these are solved
  */
// static void MX_SPI1_Init(void)
// {

//   /* USER CODE BEGIN SPI1_Init 0 */

//   /* USER CODE END SPI1_Init 0 */

//   /* USER CODE BEGIN SPI1_Init 1 */

//   /* USER CODE END SPI1_Init 1 */
//   /* SPI1 parameter configuration*/
//   hspi1.Instance = SPI1;
//   hspi1.Init.Mode = SPI_MODE_MASTER;
//   hspi1.Init.Direction = SPI_DIRECTION_2LINES;
//   hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
//   hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
//   hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
//   hspi1.Init.NSS = SPI_NSS_SOFT;
//   hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
//   hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
//   hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
//   hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//   hspi1.Init.CRCPolynomial = 7;
//   hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
//   hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
//   if (HAL_SPI_Init(&hspi1) != HAL_OK)
//   {
//     log_error(ERROR_TYPES::ERROR_SPI_INIT);
//   }
//   /* USER CODE BEGIN SPI1_Init 2 */

//   /* USER CODE END SPI1_Init 2 */

// }

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA register setup for channel 0. Warning: hardcoded test! */
  MODIFY_REG(
    DMA2_Stream0->CR,       /* Control register                       */                                        
    DMA_SxCR_DIR      |     /* Clear: Direction                       */
      DMA_SxCR_CIRC   |     /* Clear: Mode                            */
      DMA_SxCR_PINC   |     /* Clear: Peripheral increment            */
      DMA_SxCR_MINC   |     /* Clear: Memory increment                */
      DMA_SxCR_PSIZE  |     /* Clear: Peripheral data align           */
      DMA_SxCR_MSIZE  |     /* Clear: Memory data align               */
      DMA_SxCR_PL     |     /* Clear: Priority                        */
      DMA_SxCR_PFCTRL,      /* Clear: Magic mask bit (!!)             */
    0x00000000U       |     /* Set: Direction: peripheral to memory   */
      0x00000001U     |     /* Set: Peripheral: increment             */
      DMA_SxCR_MINC   |     /* Set: Memory: increment                 */
      DMA_SxCR_PSIZE  |     /* Set: Peripheral data align: word       */
      DMA_SxCR_MSIZE  |     /* Set: Memory data align: word           */
      DMA_SxCR_CIRC   |     /* Set: Mode: circular                    */
      DMA_SxCR_PL_1);       /* Set: Priority: high                    */

  /* Setting peripheral address */
  WRITE_REG(DMA2_Stream0->PAR, (uint32_t)&(ADC1->DR));

  /* Setting up memory address */
  WRITE_REG(DMA2_Stream0->M0AR, (uint32_t)&ADC1_converted_values[0]);
 
  /* Setting total transfer size and number of channels */
  MODIFY_REG(DMA2_Stream0->NDTR, DMA_SxNDT, DMA_BUFFER_LENGTH * NUM_HYDROPHONES);

  /* DMA interrupt init */
  /* Set DMA2_Stream0_IRQn interrupt priority */
  //HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  NVIC_SetPriority(DMA2_Stream0_IRQn, 0);

  /* Enable interrupt for DMA */
  //HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  /* Enable interrupt for transfer complete */
  //SET_BIT(DMA2_Stream0->CR, DMA_SxCR_TCIE); /* Might be fucked! */

  /* Enable interrupt for error */
  //SET_BIT(DMA2_Stream0->CR, DMA_SxCR_TEIE); /* Might be fucked */

  /* Enable DMA-transfer */
  SET_BIT(DMA2_Stream0->CR, DMA_SxCR_EN);
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

/**
 * @brief Read the ADC by using DMA
 * 
 * The data is fed into the even indeces of the arrays, while the odd indeces
 * are set to 0. This is due to the usage of complex FFT, which requires the
 * imaginary components to be on the odd indeces. 
 * 
 * @param p_data_hyd_port Pointer to the memory for the port hydrophone
 * @param p_data_hyd_starboard Pointer to the starboard hydrophone's memory
 * @param p_data_hyd_stern Pointer to the stern hydrophone's memory
 * 
 * @warning Unsure if I have read/understood it correctly!
 * Assuming that there exists two possibilities for the DMA to push the
 * data depending on the rank:
 * 
 *    1) 
 *      Data is pushed in serial after rank. Since the DMA/ADC is in scan
 *      mode, it scans all of the channel after rank. Most likely
 *        
 *        r1 = rank1, r2 = rank2, r3 = rank3
 *        {r1, r2, r3, r1, r2, r3, ... , r1, r2, r3}
 * 
 *    2) 
 *      All of the data for the highest priority rank is pushed first, then
 *      the data belonging to the second priority rank and so forth
 * 
 *      {r1, r1, ..., r1, r2, r2, ..., r2, r3, r3, ..., r3}
 * 
 * From my understanding of the circular DMA and ADC in scan-mode, it uses
 * the first method. The code assumes this true, however it is potentially 
 * a serious bug! 
 */
static void read_measurements(
        float32_t p_data_hyd_port[IN_BUFFER_LENGTH], 
        float32_t p_data_hyd_starboard[IN_BUFFER_LENGTH],
        float32_t p_data_hyd_stern[IN_BUFFER_LENGTH]){

  /**
   * Reading the data. Dropping the last couple of datapoints, since
   * 2048 % 3 = 2. Reducing the number of datapoints reduces the 
   * accuracy of the analysis, however prevents out-of-range error
   */
  for(uint i = 0; i < DMA_BUFFER_LENGTH - NUM_HYDROPHONES; i++){
    p_data_hyd_port[2 * i] = ADC1_converted_values[3 * i];
    p_data_hyd_port[(2 * i) + 1] = 0;

    p_data_hyd_starboard[2 * i] = (float32_t)ADC1_converted_values[(3 * i) + 1];
    p_data_hyd_starboard[(2 * i) + 1] = 0;

    p_data_hyd_stern[2 * i] = (float32_t)ADC1_converted_values[(3 * i) + 2];
    p_data_hyd_stern[(2 * i) + 1] = 0;
  }
}


/**
 * @brief Detects if the error was caused by either time or the intensity
 * and logs the correct error
 * 
 * Could be updated to detect other errors
 * 
 * @param bool_time_error Int used to indicate error with the time
 */
static void check_signal_error(uint8_t& bool_time_error){
  if(bool_time_error){
    bool_time_error = 0;
    log_error(ERROR_TYPES::ERROR_TIME_SIGNAL);
  }
}


/**
 * @brief Function to handle communication over ethernet
 * 
 * @warning Not implemented as of 11.12.2020, as I am not sure how the
 * communication between the STM32 and the main CPU should be
 */
uint8_t ethernet_coordination(void){
  return 0;
}


/**
 * @brief Function to log the errors
 * Could also be interesting to implement a timestamp to detect when the
 * error occurs. This could be done in the future, but is not a priority
 * as of 14.12.2020
 * 
 * NOTE: Could be improved by using a txt-file or other log, or send the number
 * of errors to the Xavier to be analyzed later. Using an array for temporary
 * storage, however should be improved in the future
 * 
 * @param error The error that occured
 */
static void log_error(ERROR_TYPES error){
  if(error_idx < max_num_errors - 1){
    errors_occured[error_idx] = error;
    error_idx++;
    return;
  }
  if(error_idx == max_num_errors - 1){
    errors_occured[error_idx] = ERROR_TYPES::ERROR_MEMORY;
  }
}


/**
 * @brief Overwriting a weak CB-function. The function is triggered when the
 * DMA is finished transfering data to ADC1_converted_values
 * 
 * The funtion changes the variable bool_DMA_conv_ready
 * 
 * @param hadc Pointer to the ADC-handler that uses this CB-function
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
  bool_DMA_conv_ready = 1;
}


/**
 * @brief Interrupt-function that is more hardware-oriented/lower level compared 
 * to HAL_ADC_ConvCpltCallback(). Both should in theory be triggered, however
 * experimentation has showed that there is some error in the code
 */
void DMA2_Stream0_IRQHandler(void){
  /* Checking if converting is complete */
  if(READ_BIT(DMA2->LISR, DMA_LISR_TCIF0)) {
    /* Clear register */
    WRITE_REG(DMA2->LIFCR, DMA_LIFCR_CTCIF0);

    /* Setting global state variable */
    bool_DMA_conv_ready = 1;
  }

  /* Checking if an error occured during converting */
  if (READ_BIT(DMA2->LISR, DMA_LISR_TEIF0)) {
    /* Clear register */
    WRITE_REG(DMA2->LIFCR, DMA_LIFCR_CTEIF0);

    /* Setting global error variable */
    bool_DMA_conv_error = 1;
  }
}

/**
 * @brief A function that starts the convertion between the ADC and the DMA
 * 
 * According to the manual, to restart the convertion between the ADC and DMA, 
 * the bit must be cleared by software: 
 * 
 * "However the DMA bit is not cleared by hardware. It must be written to 0, 
 * then to 1 to start a new transfer"
 */
static void start_convertion_adc_dma(void){
  /* Triggering ADC with DMA */
  CLEAR_BIT(ADC1->CR2, ADC_CR2_DMA);
  SET_BIT(ADC1->CR2, ADC_CR2_DMA);

  /* Starting ADC-convertion */
  SET_BIT(ADC1->CR2, ADC_CR2_SWSTART);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of an unidentified error occurrence
  */
void Error_Handler(void){
  log_error(ERROR_TYPES::ERROR_UNIDENTIFIED);
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
  Error_handler();
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
