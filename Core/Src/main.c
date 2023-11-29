/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


int fputc(int ch,FILE *f)
{
HAL_UART_Transmit(&huart2,(uint8_t*)&ch,1,1000);
	return 0;
}

/* USER CODE BEGIN PV */
uint32_t capture_Buf[3]={0};   //����ֵ
uint8_t capture_Cnt=0;  //����״̬��־λ
uint8_t overload_Cnt=0; //�����־
uint32_t high_time;     //�ߵ�ƽʱ��
uint32_t low_time;      //�͵�ƽʱ��
double HL_time;
double LL_time;
double fre;		//Ƶ��
double duty;  //ռ�ձ�

/* USER CODE END PV */
/* USER CODE BEGIN PFP */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance==htim3.Instance)
	{
	switch(capture_Cnt){
		case 0:
			capture_Buf[0]=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_4);  //�õ�����ֵ
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_4,TIM_ICPOLARITY_FALLING);  //�����½�����
		capture_Cnt++;
		break;
		
		case 1:
			capture_Buf[1]=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_4);  //�õ�����ֵ
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_4,TIM_ICPOLARITY_RISING);  //������������
		capture_Cnt++;
		break;
		
		case 2:
			capture_Buf[2]=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_4);  //�õ�����ֵ
		HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_4);  //ֹͣ����
		high_time=capture_Buf[1]-capture_Buf[0]+overload_Cnt*0xFFFFFFFF;  //�ߵ�ƽʱ��
		low_time=capture_Buf[2]-capture_Buf[1]+overload_Cnt*0xFFFFFFFF;  //�͵�ƽʱ��
		HL_time=high_time*0.001;
		LL_time=low_time*0.001;
		fre=1/(HL_time+LL_time)*1000;
		duty=HL_time/(HL_time+LL_time)*100;
		capture_Cnt=0;  //?????
		overload_Cnt=0;  //???????
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_4,TIM_INPUTCHANNELPOLARITY_RISING);
		HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_4);
		break;
		
	}
	}
}
/* USER CODE END PFP */



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
	int pwm_dut_v=0;
	int rate=65535/100;
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE BEGIN 2 */
	
	// HAL_UART_Transmit(&huart1,"1",1,0);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_3,TIM_INPUTCHANNELPOLARITY_RISING);  //????????
	HAL_TIM_IC_Start_IT(&htim3,	TIM_CHANNEL_4);  //??????

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		///pwm���
		pwm_dut_v++;
		HAL_Delay(5);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,pwm_dut_v*rate);
		if(pwm_dut_v>=100)
			pwm_dut_v=0;
		
		
		///pwm����
		//printf("Ƶ��:%7.31fHz,%4.11f%%,�ߵ�ƽʱ��:%7.31fms,�͵�ƽʱ��:%7.31fms\r\n\r\n",fre,duty,HL_time,LL_time);
		printf("Ƶ��:%7.31fHz,ռ�ձ�:%4.11f%%,�ߵ�ƽʱ��:%7.31fms,�͵�ƽʱ��:%7.31fms\r\n\r\n",fre,duty,HL_time,LL_time);
		HAL_Delay(1000);
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
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
