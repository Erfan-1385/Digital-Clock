/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <CODENIC_LCD.h>
#include <Rotary.h>
#include <stdio.h>
#include <stdbool.h>

#define DMAlen 100
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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
RotaryHandler rotary;

char display[16];

uint32_t temp, getTemp[DMAlen];
int second, minute, hour;
int minuteA = -1, hourA = 0;
int day, month;
int weekDay;

bool alarmMode;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM1_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc == &hadc1) {
		for(uint16_t len = 0; len < DMAlen; len++) {
			temp += getTemp[len];
		}
		temp /= DMAlen;
		temp *= 3.3;
		temp *= 100;
		temp /= 4095;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(HAL_GPIO_ReadPin(BUTTON_START_GPIO_Port, BUTTON_START_Pin) == 0) {
		second++;
		if(second > 59) {second = 0; minute++;}
		if(minute > 59) {minute = 0; hour++;}
		if(hour > 23) {day++; weekDay++; hour = 0;}
		if(weekDay > 7) {weekDay = 0;}
		if(day > 31 && month < 7) {day = 0; month++;}
		if(day > 30 && month > 6) {day = 0; month++;}
		if(month > 12) {month = 0;}

	}
	else {
		return;
	}

}


void startup(void) {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, RESET);
	HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, RESET);

	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 999;
	htim1.Init.Prescaler = 15999;
	if(HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}

	LCDCHAR_Init(16, 2);
	LCDCHAR_CursorOff();
	LCDCHAR_Clear();

	RotaryInit(&rotary, GPIOB, SCK_Pin, DATA_Pin, SWI_Pin);

	HAL_ADC_Start_DMA(&hadc1, getTemp, DMAlen);
}

void Alarm(void) {
	if(hour == hourA && minute == minuteA && second == 0) {
		LCDCHAR_Clear();
		RotarySetCounter(&rotary, 0);
		RotarySetSwitchCounter(&rotary, 0);

		alarmMode = 1;

		do {
			if(__HAL_TIM_GET_COUNTER(&htim1) > 500) {HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, SET);}
			else {HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, RESET);}
			RotaryBegin(&rotary);
			LCDCHAR_Puts(0, 0, "Wake Up!");
		} while(RotaryGetSwitchState(&rotary) != RotarySwitchON);
		LCDCHAR_Clear();
		RotarySetCounter(&rotary, 0);
		RotarySetSwitchCounter(&rotary, 0);

		alarmMode = 0;

		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, RESET);
	}
}

void startCLock(void) {
	switch(HAL_GPIO_ReadPin(BUTTON_START_GPIO_Port, BUTTON_START_Pin)) {
	case RESET:
		HAL_TIM_Base_Start_IT(&htim1);
		break;

	case SET:
		HAL_TIM_Base_Stop_IT(&htim1);
		break;
	}
}

void setWeekDay(void) {
	switch(weekDay) {
	case 0:
		LCDCHAR_Puts(10, 1, "SUN");
		break;

	case 1:
		LCDCHAR_Puts(10, 1, "MON");
		break;

	case 2:
		LCDCHAR_Puts(10, 1, "THU");
		break;

	case 3:
		LCDCHAR_Puts(10, 1, "WEN");
		break;

	case 4:
		LCDCHAR_Puts(10, 1, "THE");
		break;

	case 5:
		LCDCHAR_Puts(10, 1, "FRI");
		break;

	case 6:
		LCDCHAR_Puts(10, 1, "SAT");
		break;
	}
}

void resetSettings(void) {
	if(alarmMode == 1) {Alarm();}
	HAL_NVIC_SystemReset();
}

void alarmSetting(void) {
	RotarySetCounter(&rotary, hourA);
	RotarySetSwitchCounter(&rotary, 0);

	LCDCHAR_Clear();

	LCDCHAR_Puts(0, 0, "Alarm");
	HAL_Delay(1000);
	LCDCHAR_Clear();

	while(1) {
		if(alarmMode == 1) {Alarm();}

		RotaryBegin(&rotary);

		if(RotaryGetSwitchState(&rotary) == RotarySwitchON) {
			LCDCHAR_Clear();
			switch(RotaryGetSwitchCounter(&rotary)) {
			case 0:
				RotarySetCounter(&rotary, hourA);
				break;
			case 1:
				RotarySetCounter(&rotary, minuteA);
				break;
			}
		}

		switch(RotaryGetSwitchCounter(&rotary)) {
		case 0:
			sprintf(display, "Hour : %02d", hourA);
			LCDCHAR_Puts(0, 0, display);

			if(RotaryGetCounter(&rotary) == -1) {RotarySetCounter(&rotary, 23);}
			else if(RotaryGetCounter(&rotary) > 23) {RotarySetCounter(&rotary, 0);}
			else {hourA = RotaryGetCounter(&rotary);}
			break;
		case 1:
			sprintf(display, "Minute : %02d", minuteA);
			LCDCHAR_Puts(0, 0, display);

			if(RotaryGetCounter(&rotary) == -1) {RotarySetCounter(&rotary, 59);}
			else if(RotaryGetCounter(&rotary) > 59) {RotarySetCounter(&rotary, 0);}
			else {minuteA = RotaryGetCounter(&rotary);}
			break;
		case 2:
			LCDCHAR_Clear();
			LCDCHAR_Puts(0, 0, "Alarm Setting");
			LCDCHAR_Puts(0, 1, "Saved");
			HAL_Delay(1000);

			RotarySetCounter(&rotary, 2);
			RotarySetSwitchCounter(&rotary, 0);
			LCDCHAR_Clear();
			return;
			break;
		}
	}
}

void dateSetting(void) {
	RotarySetCounter(&rotary, month);
	RotarySetSwitchCounter(&rotary, 0);

	LCDCHAR_Clear();

	LCDCHAR_Puts(0, 0, "Date");
	HAL_Delay(1000);
	LCDCHAR_Clear();

	while(1) {
		if(alarmMode == 1) {Alarm();}

		RotaryBegin(&rotary);

		if(RotaryGetSwitchState(&rotary) == RotarySwitchON) {
			LCDCHAR_Clear();
			switch(RotaryGetSwitchCounter(&rotary)) {
			case 0:
				RotarySetCounter(&rotary, month);
				break;
			case 1:
				RotarySetCounter(&rotary, day);
				break;
			case 2:
				RotarySetCounter(&rotary, weekDay);
				break;
			}
		}

		switch(RotaryGetSwitchCounter(&rotary)) {
		case 0:
			sprintf(display, "Month : %02d", month);
			LCDCHAR_Puts(0, 0, display);

			if(RotaryGetCounter(&rotary) == -1) {RotarySetCounter(&rotary, 12);}
			else if(RotaryGetCounter(&rotary) > 12) {RotarySetCounter(&rotary, 0);}
			else {month = RotaryGetCounter(&rotary);}
			break;

		case 1:
			sprintf(display, "Day : %02d", day);
			LCDCHAR_Puts(0, 0, display);

			if(RotaryGetCounter(&rotary) == -1) {RotarySetCounter(&rotary, 31);}
			else if(RotaryGetCounter(&rotary) > 31) {RotarySetCounter(&rotary, 0);}
			else {day = RotaryGetCounter(&rotary);}
			break;

		case 2:
			switch(weekDay) {
			case 0:
				sprintf(display, "Week Day : SUN");
				LCDCHAR_Puts(0, 0, display);
				break;

			case 1:
				sprintf(display, "Week Day : MON");
				LCDCHAR_Puts(0, 0, display);
				break;

			case 2:
				sprintf(display, "Week Day : TUE");
				LCDCHAR_Puts(0, 0, display);
				break;

			case 3:
				sprintf(display, "Week Day : WEN");
				LCDCHAR_Puts(0, 0, display);
				break;

			case 4:
				sprintf(display, "Week Day : THE");
				LCDCHAR_Puts(0, 0, display);
				break;

			case 5:
				sprintf(display, "Week Day : FRI");
				LCDCHAR_Puts(0, 0, display);
				break;

			case 6:
				sprintf(display, "Week Day : SAT");
				LCDCHAR_Puts(0, 0, display);
				break;
			}

			if(RotaryGetCounter(&rotary) == -1) {RotarySetCounter(&rotary, 6);}
			else if(RotaryGetCounter(&rotary) > 6) {RotarySetCounter(&rotary, 0);}
			else {weekDay = RotaryGetCounter(&rotary);}
			break;

		case 3:
			LCDCHAR_Clear();
			LCDCHAR_Puts(0, 0, "Date Setting");
			LCDCHAR_Puts(0, 1, "Saved");
			HAL_Delay(1000);

			RotarySetCounter(&rotary, 1);
			RotarySetSwitchCounter(&rotary, 0);
			LCDCHAR_Clear();
			return;
			break;
		}
	}
}

void timeSetting(void) {
	RotarySetCounter(&rotary, hour);
	RotarySetSwitchCounter(&rotary, 0);

	LCDCHAR_Clear();


	LCDCHAR_Puts(0, 0, "Time");
	HAL_Delay(1000);
	LCDCHAR_Clear();

	while(1) {
		if(alarmMode == 1) {Alarm();}
		RotaryBegin(&rotary);

		if(RotaryGetSwitchState(&rotary) == RotarySwitchON) {
			LCDCHAR_Clear();
			switch(RotaryGetSwitchCounter(&rotary)) {
			case 0:
				RotarySetCounter(&rotary, hour);
				break;
			case 1:
				RotarySetCounter(&rotary, minute);
				break;
			case 2:
				RotarySetCounter(&rotary, second);
				break;
			}
		}

		switch(RotaryGetSwitchCounter(&rotary)) {
		case 0:
			sprintf(display, "Hour : %02d", hour);
			LCDCHAR_Puts(0, 0, display);

			if(RotaryGetCounter(&rotary) == -1) {RotarySetCounter(&rotary, 23);}
			else if(RotaryGetCounter(&rotary) > 23) {RotarySetCounter(&rotary, 0);}
			else {hour = RotaryGetCounter(&rotary);}
			break;
		case 1:
			sprintf(display, "Minute : %02d", minute);
			LCDCHAR_Puts(0, 0, display);

			if(RotaryGetCounter(&rotary) == -1) {RotarySetCounter(&rotary, 59);}
			else if(RotaryGetCounter(&rotary) > 59) {RotarySetCounter(&rotary, 0);}
			else {minute = RotaryGetCounter(&rotary);}
			break;
		case 2:
			sprintf(display, "Second : %02d", second);
			LCDCHAR_Puts(0, 0, display);

			if(RotaryGetCounter(&rotary) == -1) {RotarySetCounter(&rotary, 59);}
			else if(RotaryGetCounter(&rotary) > 59) {RotarySetCounter(&rotary, 0);}
			else {second = RotaryGetCounter(&rotary);}
			break;
		case 3:
			LCDCHAR_Clear();
			LCDCHAR_Puts(0, 0, "Time Setting");
			LCDCHAR_Puts(0, 1, "Saved");
			HAL_Delay(1000);

			RotarySetCounter(&rotary, 0);
			RotarySetSwitchCounter(&rotary, 0);
			LCDCHAR_Clear();
			return;
			break;
		}
	}
}

void menu(void) {
	LCDCHAR_Clear();
	RotarySetSwitchCounter(&rotary, 0);

	while(1) {
		RotaryBegin(&rotary);
		if(RotaryGetState(&rotary) != RotaryNone) {LCDCHAR_Clear();}

		switch(RotaryGetCounter(&rotary)) {
		case -1:
			RotarySetCounter(&rotary, 4);
			break;
		case 0:
			LCDCHAR_Puts(0, 0, "Time Setting");
			if(RotaryGetSwitchState(&rotary) == RotarySwitchON) {timeSetting();}
			break;
		case 1:
			LCDCHAR_Puts(0, 0, "Date Setting");
			if(RotaryGetSwitchState(&rotary) == RotarySwitchON) {dateSetting();}
			break;
		case 2:
			LCDCHAR_Puts(0, 0, "Alarm Setting");
			if(RotaryGetSwitchState(&rotary) == RotarySwitchON) {alarmSetting();}
			break;
		case 3:
			LCDCHAR_Puts(0, 0, "Reset Settings");
			if(RotaryGetSwitchState(&rotary) == RotarySwitchON) {resetSettings();}
			break;
		case 4:
			LCDCHAR_Puts(0, 0, "Return");
			if(RotaryGetSwitchState(&rotary) == RotarySwitchON) {RotarySetCounter(&rotary, 0); RotarySetSwitchCounter(&rotary, 0); return;}
			break;
		case 5:
			RotarySetCounter(&rotary, 0);
			break;
		}
	}
}

//void getTemp(void) {
//	if(HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK) {
//		temp = HAL_ADC_GetValue(&hadc1);
//		temp = temp * 3.3 * 100 / 4095;
//		return;
//	}
//}

void mainDisplay(void) {
	if(RotaryGetSwitchState(&rotary) == RotarySwitchON) {menu();}

	sprintf(display, "%02d:%02d:%02d", hour, minute, second);
	LCDCHAR_Puts(0, 0, display);

	sprintf(display, "%02ld%c", temp, 223);
	LCDCHAR_Puts(10, 0, display);

	sprintf(display, "%02d/%02d", month, day);
	LCDCHAR_Puts(0, 1, display);

	setWeekDay();
}
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
  MX_TIM1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  startup();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Alarm();
	  RotaryBegin(&rotary);
	  startCLock();
	  mainDisplay();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
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

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RS_Pin|RW_Pin|EN_Pin|D4_Pin
                          |D5_Pin|D6_Pin|D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RS_Pin RW_Pin EN_Pin D4_Pin
                           D5_Pin D6_Pin D7_Pin */
  GPIO_InitStruct.Pin = RS_Pin|RW_Pin|EN_Pin|D4_Pin
                          |D5_Pin|D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SWI_Pin DATA_Pin SCK_Pin BUTTON_START_Pin */
  GPIO_InitStruct.Pin = SWI_Pin|DATA_Pin|SCK_Pin|BUTTON_START_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
