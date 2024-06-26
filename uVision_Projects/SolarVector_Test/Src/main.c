/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This program finds solar vectors and prints information over serial based 
	* on which #defines are uncommented below.
	*
	* See the Solar Vector Test SOP.
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <SolarVectors.h>
#include <DigitalFilters.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// #define YAWPITCH_TEST // Uncomment to unit test yaw and pitch function
#define PRINT_RAW_VALS
// #define PRINT_SOLAR_VECTOR
#define PRINT_YAWPITCH

#define ADC_CHANNEL_XP 0
#define ADC_CHANNEL_XN 1
#define ADC_CHANNEL_YP 2
#define ADC_CHANNEL_YN 3
#define ADC_CHANNEL_ZP 4
#define ADC_CHANNEL_ZN 5
#define NUM_ADC_CHANNELS 6
#define MOVING_AVG_LENGTH 20

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

ADC_ChannelConfTypeDef sConfig = {0};
uint32_t adc_data[NUM_ADC_CHANNELS];
float sv_raw[NUM_ADC_CHANNELS];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

	char transmit[200];

	#ifdef YAWPITCH_TEST // Unit testing
	int num_tests = 7;
	double x_components[] = {1,		0.7071,	 -0.7071,   0.7071,  -0.7071,  0.7071,        0};
	double y_components[] = {0, 	0.7071,		0.7071,  -0.7071,  -0.7071,       0,  -0.7071};
	double z_components[] = {0, 			 0, 			 0, 			 0, 			 0,  0.7071,  -0.7071};
	
	double desired_yaw[] =  {0, 			45, 		 135,			 -45,			-135,				0,			-90};
	double desired_pitch[] = {0,			 0,				 0,				 0,				 0, 		 45,			-45};
	
	for(int i = 0;i < num_tests;i++) {
		Matrix v = make3x1Vector(x_components[i], y_components[i], z_components[i]);
		matrixScale(v, 1.0/vectorNorm(v));
		char transmit[50];
		printSolarVector(v, transmit);
		HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 50);
		sprintf(transmit, "Desired:\r\nyaw: %3.1f pitch: %3.1f\r\n\r\n", desired_yaw[i], desired_pitch[i]);
		HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 50);
	}
	
	while(1);
	#endif
	
	HAL_ADC_Start_DMA(&hadc1, adc_data, NUM_ADC_CHANNELS); // Start ADC in DMA mode
	
	sprintf(transmit, "Finished init\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 20);
	
	// See the Attitude Control System document for info on the body reference frame and axes
	double volts_xp, volts_xn, volts_yp, volts_yn, volts_zp, volts_zn; // Positive and negative raw current values for each axis
	
	MovingAvgFilter filter = newMovingAvgFilter(NUM_ADC_CHANNELS, MOVING_AVG_LENGTH);
	
	Matrix solar_vector = newMatrix(3, 1); // Make a new column vector Matrix for the solar vector
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		// Read ADC
		for(int i = 0;i < NUM_ADC_CHANNELS;i++) {
			sv_raw[i] = ADC_TO_VOLTS(adc_data[i]);
		}
		runMovingAvgFilter(filter, sv_raw); // Moving average solar vector readings
		
		#ifdef PRINT_RAW_VALS
		// Read data and convert to volts
		volts_xp = sv_raw[ADC_CHANNEL_XP]*XP_SCALE;
		volts_xn = sv_raw[ADC_CHANNEL_XN]*XN_SCALE;
		volts_yp = sv_raw[ADC_CHANNEL_YP]*YP_SCALE;
		volts_yn = sv_raw[ADC_CHANNEL_YN]*YN_SCALE;
		volts_zp = sv_raw[ADC_CHANNEL_ZP]*ZP_SCALE;
		volts_zn = sv_raw[ADC_CHANNEL_ZN]*ZN_SCALE;
		
		// Print x values
		sprintf(transmit, "xp: %2.6f xn: %2.6f\r\n", volts_xp, volts_xn);
		HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 40);
		
		// Print y values
		sprintf(transmit, "yp: %2.6f yn: %2.6f\r\n", volts_yp, volts_yn);
		HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 40);
		
		// Print z values
		sprintf(transmit, "zp: %2.6f zn: %2.6f\r\n", volts_zp, volts_zn);
		HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 40);
		#endif
		
		// Calculate solar vector
		SV_Status sv_return = findSolarVector(sv_raw, 6, solar_vector);
		
		
		if(sv_return == SV_FOUND) { // Check if a valid solar vector was found
			#ifdef PRINT_SOLAR_VECTOR
			// Print solar vector over serial using the Matrix library command;
			printMatrix(solar_vector, transmit);
			HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 40);
			#endif
			#ifdef PRINT_YAWPITCH
			// Print solar vector over serial using the SolarVector library command
			printSolarVector(solar_vector, transmit);
			HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 40);
			#endif
		}
		else if(sv_return == SV_NOTFOUND){
			sprintf(transmit, "Solar vector indeterminate!\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 30);
		}
		else {
			sprintf(transmit, "Dark detected!\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 20);
		}
		
		sprintf(transmit, "\r\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)transmit, strlen(transmit), 30);
		
		HAL_Delay(100);
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  /**Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 6;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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

#ifdef  USE_FULL_ASSERT
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
