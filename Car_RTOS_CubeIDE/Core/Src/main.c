/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "TJ_MPU6050.h"
#include "string.h"
#include "stdio.h"
//#include "SEGGER_SYSVIEW.h"
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
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//Sensor task handles
TaskHandle_t xUS1TaskHandle;
TaskHandle_t xUS2TaskHandle;
TaskHandle_t xUS3TaskHandle;
TaskHandle_t xUS4TaskHandle;
TaskHandle_t xUS5TaskHandle;

//Motion task handles
TaskHandle_t xTurnTestTaskHandle;

//IMU task handle
TaskHandle_t xIMUTaskHandle;

const float SpeedSound = 0.0343;
float range1R;
float range2R;
float range3R;
float range4R;
float range5R;
float range1F = 0.0f;
float range2F = 0.0f;
float range3F = 0.0f;
float range4F = 0.0f;
float range5F = 0.0f;
uint8_t icFlag1 = 0;
uint8_t captureIdx1 = 0;
uint32_t edge1Time1=0, edge2Time1=0;
uint8_t icFlag2 = 0;
uint8_t captureIdx2 = 0;
uint32_t edge1Time2=0, edge2Time2=0;
uint8_t icFlag3 = 0;
uint8_t captureIdx3 = 0;
uint32_t edge1Time3=0, edge2Time3=0;
uint8_t icFlag4 = 0;
uint8_t captureIdx4 = 0;
uint32_t edge1Time4=0, edge2Time4=0;
uint8_t icFlag5 = 0;
uint8_t captureIdx5 = 0;
uint32_t edge1Time5=0, edge2Time5=0;
uint8_t USNUM = 1;

char uartBuf[100];
char uartSenBuf[32] = {0};

//IMU
ScaledData_Def AccelScaled, GyroScaled;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */
void vUS1Task(void *pvParameters);
void vUS2Task(void *pvParameters);
void vUS3Task(void *pvParameters);
void vUS4Task(void *pvParameters);
void vUS5Task(void *pvParameters);
void usDelay(uint32_t uSec);
void LowPassFilter(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void vTurnTestTask(void *pvParameters);
void vMPU6050Task(void *pvParameters);
void MPU6050Setup(void);
int _write(int file, char *ptr, int len);

//float LowPassFilter(float rangeDataold,float rangeDatanew);
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
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  //Start PWM for motor drivers
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

  //Setup IMU
  MPU6050Setup();
  //SEGGER_SYSVIEW_Conf();
  //SEGGER_SYSVIEW_Start();
  // Create Range sensor task
  xTaskCreate(vUS1Task ,"US1-Task", configMINIMAL_STACK_SIZE, NULL, 1, &xUS1TaskHandle);
  xTaskCreate(vUS2Task ,"US2-Task", configMINIMAL_STACK_SIZE, NULL, 1, &xUS2TaskHandle);
  xTaskCreate(vUS3Task ,"US3-Task", configMINIMAL_STACK_SIZE, NULL, 1, &xUS3TaskHandle);
  xTaskCreate(vUS4Task ,"US4-Task", configMINIMAL_STACK_SIZE, NULL, 1, &xUS4TaskHandle);
  xTaskCreate(vUS5Task ,"US5-Task", configMINIMAL_STACK_SIZE, NULL, 1, &xUS5TaskHandle);

  //Motion tasks
  xTaskCreate(vTurnTestTask, "Turn-Test-Task", configMINIMAL_STACK_SIZE, NULL, 1, &xTurnTestTaskHandle);

  //IMU Task
  xTaskCreate(vMPU6050Task, "IMU-Task", configMINIMAL_STACK_SIZE, NULL, 1, &xIMUTaskHandle);

  vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 21000;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 4000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 2000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 84-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 4;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 84-1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 65535;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 4;
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1|TRIG_2_Pin|TRIG_4_Pin|BIN1_1_Pin
                          |STBY_1_Pin|AIN1_1_Pin|AIN2_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, TRIG_5_Pin|LD2_Pin|TRIG_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TRIG_1_Pin|BIN2_2_Pin|BIN1_2_Pin|STBY2_Pin
                          |AIN1_2_Pin|AIN2_2_Pin|BIN2_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC1 TRIG_2_Pin TRIG_4_Pin BIN1_1_Pin
                           STBY_1_Pin AIN1_1_Pin AIN2_1_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|TRIG_2_Pin|TRIG_4_Pin|BIN1_1_Pin
                          |STBY_1_Pin|AIN1_1_Pin|AIN2_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : TRIG_5_Pin LD2_Pin TRIG_3_Pin */
  GPIO_InitStruct.Pin = TRIG_5_Pin|LD2_Pin|TRIG_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : TRIG_1_Pin BIN2_2_Pin BIN1_2_Pin STBY2_Pin
                           AIN1_2_Pin AIN2_2_Pin BIN2_1_Pin */
  GPIO_InitStruct.Pin = TRIG_1_Pin|BIN2_2_Pin|BIN1_2_Pin|STBY2_Pin
                          |AIN1_2_Pin|AIN2_2_Pin|BIN2_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void vUS1Task(void *pvParameters)
{
	for(;;){
		//1. Set TRIG pin low for usec
		HAL_GPIO_WritePin(TRIG_1_GPIO_Port, TRIG_1_Pin, GPIO_PIN_RESET);
		usDelay(3);

		//2. Send quick pulse from trig pin
		HAL_GPIO_WritePin(TRIG_1_GPIO_Port, TRIG_1_Pin, GPIO_PIN_SET);
		usDelay(10);
		HAL_GPIO_WritePin(TRIG_1_GPIO_Port, TRIG_1_Pin, GPIO_PIN_RESET);

		//3. ECHO signal
		USNUM = 1;
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
		//Wait for IC flag
		uint32_t startTick = HAL_GetTick();
		do
		{
			if(icFlag1) break;
		}while((HAL_GetTick() - startTick) < 500);
		icFlag1 = 0;
		HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1);

		//4. calculate distance in cm
		if(edge2Time1 > edge1Time1)
		{
			range1R = ((edge2Time1 - edge1Time1) + 0.0f)*(SpeedSound/2);
			LowPassFilter();
		}
		else
		{
			range1R = 0.0f;
		}

		//Send data through UART
		//printf("Range 1 raw: %f \n", range1R);
		//printf("Range 1 filtered: %f \n", range1F);
//		sprintf(uartSenBuf, "Range 1: %f\n", range1F);
//		HAL_UART_Transmit(&huart2, (uint8_t *)uartSenBuf, strlen(uartSenBuf), 1000);
//		HAL_UART_Transmit(&huart2, (uint8_t *)range1R, strlen(range1R), 1000);


		//5. Delay for 10 ms
		HAL_Delay(10);
	}
}

void vUS2Task(void *pvParameters)
{
	for(;;){
		//1. Set TRIG pin low for usec
		HAL_GPIO_WritePin(TRIG_2_GPIO_Port, TRIG_2_Pin, GPIO_PIN_RESET);
		usDelay(3);

		//2. Send quick pulse from trig pin
		HAL_GPIO_WritePin(TRIG_2_GPIO_Port, TRIG_2_Pin, GPIO_PIN_SET);
		usDelay(10);
		HAL_GPIO_WritePin(TRIG_2_GPIO_Port, TRIG_2_Pin, GPIO_PIN_RESET);

		//3. ECHO signal
		USNUM = 2;
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
		//Wait for IC flag
		uint32_t startTick = HAL_GetTick();
		do
		{
			if(icFlag2) break;
		}while((HAL_GetTick() - startTick) < 500);
		icFlag2 = 0;
		HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_2);

		//4. calculate distance in cm
		if(edge2Time2 > edge1Time2)
		{
			range2R = ((edge2Time2 - edge1Time2) + 0.0f)*(SpeedSound/2);
			LowPassFilter();
		}
		else
		{
			range2R = 0.0f;
		}

		//printf("Range 2 raw: %f \n", range2R);
		//printf("Range 2 filtered: %f \n", range2F);


		//5. Delay for 10 ms
		HAL_Delay(10);
	}
}

void vUS3Task(void *pvParameters)
{
	for(;;){
		//1. Set TRIG pin low for usec
		HAL_GPIO_WritePin(TRIG_3_GPIO_Port, TRIG_3_Pin, GPIO_PIN_RESET);
		usDelay(3);

		//2. Send quick pulse from trig pin
		HAL_GPIO_WritePin(TRIG_3_GPIO_Port, TRIG_3_Pin, GPIO_PIN_SET);
		usDelay(10);
		HAL_GPIO_WritePin(TRIG_3_GPIO_Port, TRIG_3_Pin, GPIO_PIN_RESET);

		//3. ECHO signal
		USNUM = 3;
		HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_4);
		//Wait for IC flag
		uint32_t startTick = HAL_GetTick();
		do
		{
			if(icFlag3) break;
		}while((HAL_GetTick() - startTick) < 500);
		icFlag3 = 0;
		HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_4);

		//4. calculate distance in cm
		if(edge2Time3 > edge1Time3)
		{
			range3R = ((edge2Time3 - edge1Time3) + 0.0f)*(SpeedSound/2);
			LowPassFilter();
		}
		else
		{
			range3R = 0.0f;
		}

		//printf("Range 3 raw: %f \n", range3R);
		//printf("Range 3 filtered: %f \n", range3F);


		//5. Delay for 10 ms
		HAL_Delay(10);
	}
}

void vUS4Task(void *pvParameters)
{
	for(;;){
		//1. Set TRIG pin low for usec
		HAL_GPIO_WritePin(TRIG_4_GPIO_Port, TRIG_4_Pin, GPIO_PIN_RESET);
		usDelay(3);

		//2. Send quick pulse from trig pin
		HAL_GPIO_WritePin(TRIG_4_GPIO_Port, TRIG_4_Pin, GPIO_PIN_SET);
		usDelay(10);
		HAL_GPIO_WritePin(TRIG_4_GPIO_Port, TRIG_4_Pin, GPIO_PIN_RESET);

		//3. ECHO signal
		USNUM = 4;
		HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1);
		//Wait for IC flag
		uint32_t startTick = HAL_GetTick();
		do
		{
			if(icFlag4) break;
		}while((HAL_GetTick() - startTick) < 500);
		icFlag4 = 0;
		HAL_TIM_IC_Stop_IT(&htim5, TIM_CHANNEL_1);

		//4. calculate distance in cm
		if(edge2Time4 > edge1Time4)
		{
			range4R = ((edge2Time4 - edge1Time4) + 0.0f)*(SpeedSound/2);
			LowPassFilter();
		}
		else
		{
			range4R = 0.0f;
		}

		//printf("Range 2 raw: %f \n", range2R);
		//printf("Range 2 filtered: %f \n", range2F);


		//5. Delay for 10 ms
		HAL_Delay(10);
	}
}

void vUS5Task(void *pvParameters)
{
	for(;;){
		//1. Set TRIG pin low for usec
		HAL_GPIO_WritePin(TRIG_5_GPIO_Port, TRIG_5_Pin, GPIO_PIN_RESET);
		usDelay(3);

		//2. Send quick pulse from trig pin
		HAL_GPIO_WritePin(TRIG_5_GPIO_Port, TRIG_5_Pin, GPIO_PIN_SET);
		usDelay(10);
		HAL_GPIO_WritePin(TRIG_5_GPIO_Port, TRIG_5_Pin, GPIO_PIN_RESET);

		//3. ECHO signal
		USNUM = 5;
		HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_2);
		//Wait for IC flag
		uint32_t startTick = HAL_GetTick();
		do
		{
			if(icFlag5) break;
		}while((HAL_GetTick() - startTick) < 500);
		icFlag5 = 0;
		HAL_TIM_IC_Stop_IT(&htim5, TIM_CHANNEL_2);

		//4. calculate distance in cm
		if(edge2Time5 > edge1Time5)
		{
			range5R = ((edge2Time5 - edge1Time5) + 0.0f)*(SpeedSound/2);
			LowPassFilter();
		}
		else
		{
			range5R = 0.0f;
		}

		//printf("Range 2 raw: %f \n", range2R);
		//printf("Range 2 filtered: %f \n", range2F);


		//5. Delay for 10 ms
		HAL_Delay(10);
	}
}

void usDelay(uint32_t uSec)
{
	if(uSec < 2) uSec = 2;
	TIM4->ARR = uSec - 1;
	TIM4->EGR = 1;
	TIM4->SR &= ~1;
	TIM4->CR1 |= 1;
	while((TIM4->SR&0x0001) != 1);
	TIM4->SR &= ~(0x0001);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	if(USNUM == 1){
		if(captureIdx1 == 0)
		{
			edge1Time1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

			captureIdx1 = 1;
		}
		else if(captureIdx1 == 1)
		{
			edge2Time1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			captureIdx1 = 0;
			icFlag1 = 1;
		}
	}
	else if(USNUM == 2){
		if(captureIdx2 == 0)
		{
			edge1Time2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

			captureIdx2 = 1;
		}
		else if(captureIdx2 == 1)
		{
			edge2Time2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			captureIdx2 = 0;
			icFlag2 = 1;
		}
	}
	else if(USNUM == 3){
		if(captureIdx3 == 0)
		{
			edge1Time3 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);

			captureIdx3 = 1;
		}
		else if(captureIdx3 == 1)
		{
			edge2Time3 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
			captureIdx3 = 0;
			icFlag3 = 1;
		}
	}
	else if(USNUM == 4){
		if(captureIdx4 == 0)
		{
			edge1Time4 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

			captureIdx4 = 1;
		}
		else if(captureIdx4 == 1)
		{
			edge2Time4 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			captureIdx4 = 0;
			icFlag4 = 1;
		}
	}
	else{
		if(captureIdx5 == 0)
		{
			edge1Time5 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

			captureIdx5 = 1;
		}
		else if(captureIdx5 == 1)
		{
			edge2Time5 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			captureIdx5 = 0;
			icFlag5 = 1;
		}
	}
}

void LowPassFilter(void)
{
	if (USNUM == 1){
		if (range1R != 0){
			range1F = (0.75)*range1F + (0.25)*range1R;
		}
	}
	else if (USNUM == 2){
		if (range2R != 0){
			range2F = (0.75)*range2F + (0.25)*range2R;
		}
	}
	else if (USNUM == 3){
		if (range3R != 0){
			range3F = (0.75)*range3F + (0.25)*range3R;
		}
	}
	else if (USNUM == 4){
		if (range4R != 0){
			range4F = (0.75)*range4F + (0.25)*range4R;
		}
	}
	else{
		if (range5R != 0){
			range5F = (0.75)*range5F + (0.25)*range5R;
		}
	}
}


void vTurnTestTask(void *pvParameters)
{
	for(;;){
		HAL_GPIO_WritePin(AIN2_1_GPIO_Port, AIN2_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(AIN1_1_GPIO_Port, AIN1_1_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(STBY_1_GPIO_Port, STBY_1_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(BIN2_1_GPIO_Port, BIN2_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(BIN1_1_GPIO_Port, BIN1_1_Pin, GPIO_PIN_RESET);

		htim1.Instance->CCR4 = 0;
		htim1.Instance->CCR3 = 0;

		HAL_GPIO_WritePin(AIN2_2_GPIO_Port, AIN2_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(AIN1_2_GPIO_Port, AIN1_2_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(STBY2_GPIO_Port, STBY2_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(BIN2_2_GPIO_Port, BIN2_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(BIN1_2_GPIO_Port, BIN1_2_Pin, GPIO_PIN_SET);

		htim1.Instance->CCR2 = 0;
		htim1.Instance->CCR1 = 0;

		HAL_Delay(10);
	}
}

void MPU6050Setup(void)
{
	  MPU_ConfigTypeDef MpuConfig;
	  //Initialize the MPU6050
	  MPU6050_Init(&hi2c1);
	  //Configure accel and gyro parameters
	  MpuConfig.Accel_Full_Scale = AFS_SEL_4g;
	  MpuConfig.ClockSource = Internal_8MHz;
	  MpuConfig.CONFIG_DLPF = DLPF_184A_188G_Hz;
	  MpuConfig.Gyro_Full_Scale = FS_SEL_500;
	  MpuConfig.Sleep_Mode_Bit = 0;
	  MPU6050_Config(&MpuConfig);
}

void vMPU6050Task(void *pvParameters)
{
	for(;;)
	{
		//Get scaled MPU6050 data in milli g
		MPU6050_Get_Accel_Cali(&AccelScaled);
		MPU6050_Get_Gyro_Scale(&GyroScaled);

		HAL_Delay(10);
	}
}

int _write(int file, char *ptr, int len)
{
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++){
		ITM_SendChar(*ptr++);
	}
	return len;
}
//float LowPassFilter(float rangeDataold,float rangeDatanew)
//{
//	float rangeDataF = (0.75)*rangeDataold + (0.25)*rangeDatanew;
//	return rangeDataF;
//}
/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
