/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "DFPlayer.h"
#include <string.h>
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define S0_L HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
#define S0_H HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
#define S1_L HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
#define S1_H HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
#define S2_L HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
#define S2_H HAL_GPIO_WritePin (GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
#define S3_L HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
#define S3_H HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
//#define LED_OFF HAL_GPIO_WritePin(GPIOC, GPTO_PIN_4, GPIO_PIN_RESET);
//#define LED_ON HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
float RGB_Scale[3];
int *rgb;
int count;
int cnt[3];
int flag;
uint8_t buf[200];
DFPLAYER_Name MP3;
//A9_TX, A10_RX
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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void filter(int s2, int s3){
	if(s2 == 0 && s3 == 0){
		S2_L; S3_L;
	}
	if(s2 == 0 && s3 == 1){
		S2_L; S3_H;
	}
	if(s2 == 1 && s3 == 0){
		S2_H; S3_L;
	}
	if(s2 == 1 && s3 == 1){
		S2_H; S3_H;
	}
}

void TSC_WB(int s2, int s3){
	count = 0;
	flag++;
	filter(s2, s3);
}

int *GetColor()
{
	int r[3];
	r[0] = RGB_Scale[0]*cnt[0];
	r[1] = RGB_Scale[1]*cnt[1];
	r[2] = RGB_Scale[2]*cnt[2]+3;
	if (r[0]>255 && r[1]>255 && r[2]>255){
		r[0] = 255;
		r[1] = 255;
		r[2] = 255;
	}
	//else if (r[0]>255 && r[1]>255
	return r;
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
  MX_TIM2_Init();
  MX_USB_DEVICE_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
	DFPLAYER_Init(&MP3, &huart1);
	DFPLAYER_SetVolume(&MP3, 30);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim3);
  /* USER CODE BEGIN 2 */
	//DFPLAYER_Play(&MP3);
	S0_L;
	S1_H;
	HAL_Delay(4000);
	RGB_Scale[0] = 255.0/cnt[0];
	RGB_Scale[1] = 255.0/cnt[1];
	RGB_Scale[2] = 255.0/cnt[2];
	//sprintf(buf,"R = %d, G = %d, B = %d\n", (int)(RGB_Scale[0]*cnt[0]), (int)(RGB_Scale[1]*cnt[1]), (int)(RGB_Scale[2]*cnt[2]));
	//CDC_Transmit_FS(buf, strlen(buf));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		/*DFPLAYER_PlayFileInFolder(&MP3, 1, 2);
		HAL_Delay(4000);
		DFPLAYER_Stop(&MP3);*/
		flag = 0;
		count = 0;
		HAL_Delay(1000);
		rgb = GetColor();
		//sprintf(buf,"R = %d, G = %d, B = %d\n", (int)(RGB_Scale[0]*cnt[0]), (int)(RGB_Scale[1]*cnt[1]), (int)(RGB_Scale[2]*cnt[2]));
		//sprintf(buf,"R = %d, G = %d, B = %d\n", rgb[0],rgb[1],rgb[2]);
		//CDC_Transmit_FS(buf, strlen(buf));
		//HAL_Delay(2000);
		if (rgb[0]<12 && rgb[1]<12 && rgb[2]<12){
			//CDC_Transmit_FS("BLACK\n", 6);
			sprintf(buf,"R = %d, G = %d, B = %d\nUNDEFINED\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
		}
		else if (rgb[0]>180 && rgb[1]>180 && rgb[2]>180 && (rgb[0]-rgb[1])<40){
			sprintf(buf,"R = %d, G = %d, B = %d\nWHITE\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 9);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
			//CDC_Transmit_FS("WHITE\n", 6);
		}
		else if (rgb[0]>rgb[1] && abs(rgb[1]-rgb[2])>=2 && (rgb[0]-rgb[1])>10 && (rgb[0]-rgb[1])<40){
			//CDC_Transmit_FS("PINK\n", 6);
			sprintf(buf,"R = %d, G = %d, B = %d\nBROWN\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 10);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else if (rgb[0]<40 && rgb[1]<40 && rgb[2]<40){
			//CDC_Transmit_FS("BLACK\n", 6);
			sprintf(buf,"R = %d, G = %d, B = %d\nBLACK\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 8);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else if ((rgb[0]-rgb[1])>=50 && (rgb[0]-rgb[2])>=50 && (rgb[1]<100) && (rgb[2]<100)){
			sprintf(buf,"R = %d, G = %d, B = %d\nRED\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 1);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else if (rgb[0]>rgb[1] && rgb[1]>rgb[2] && (rgb[0]-rgb[1])<60 && rgb[2]<180 && rgb[0]>120 && (rgb[0]-rgb[1])>10){
			//CDC_Transmit_FS("PINK\n", 6);
			sprintf(buf,"R = %d, G = %d, B = %d\nYELLOW\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 5);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else if (rgb[0]>rgb[1] && abs(rgb[1]-rgb[2])<20 && (rgb[0]-rgb[1])>60 && rgb[2]<180){
			//CDC_Transmit_FS("PINK\n", 6);
			sprintf(buf,"R = %d, G = %d, B = %d\nORANGE\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 7);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else if (rgb[0]>rgb[1] && rgb[2]>rgb[1] && rgb[0]>rgb[2] && (rgb[0]-rgb[1])>50){
			//CDC_Transmit_FS("PINK\n", 6);
			sprintf(buf,"R = %d, G = %d, B = %d\nPINK\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 2);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else if ((rgb[2]-rgb[1])>=4 && (rgb[2]-rgb[0])>=4){
			sprintf(buf,"R = %d, G = %d, B = %d\nBLUE\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 4);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else if ((rgb[2]-rgb[1])>=1 && (rgb[2]-rgb[0])>=-3){
			sprintf(buf,"R = %d, G = %d, B = %d\nPURPLE\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 3);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else 	if ((rgb[1]-rgb[0])>=1 && (rgb[1]-rgb[2])>=10){
			sprintf(buf,"R = %d, G = %d, B = %d\nGREEN\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 6);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else if ((rgb[0]>=50 && rgb[0]<180) && (rgb[1]>=50 && rgb[1]<180) && (rgb[2]>=50 && rgb[2]<180)){
			//CDC_Transmit_FS("GRAY\n", 6);
			sprintf(buf,"R = %d, G = %d, B = %d\nGRAY\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
			HAL_Delay(1000);
			DFPLAYER_PlayFileInFolder(&MP3, 1, 11);
			HAL_Delay(1500);
			DFPLAYER_Stop(&MP3);
		}
		else{
			sprintf(buf,"R = %d, G = %d, B = %d\nUNDEFINED\n", rgb[0],rgb[1],rgb[2]);
			CDC_Transmit_FS(buf, strlen(buf));
		}
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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 719;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9999;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == htim3.Instance){
		count = __HAL_TIM_GetCounter(&htim2);
		switch(flag){
			case 0:
				TSC_WB(0, 0);
				break;
			case 1:
				cnt[0] = count;
				TSC_WB(1,1);
				break;
			case 2:
				cnt[1] = count;
				TSC_WB(0,1);
				break;
			case 3:
				cnt[2] = count;
				TSC_WB(1,0);
				break;
			default: count = 0; break;
		}
	__HAL_TIM_SetCounter(&htim2, 0);
	}
}
			
			
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
