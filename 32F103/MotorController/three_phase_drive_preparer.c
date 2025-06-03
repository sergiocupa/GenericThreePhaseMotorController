/*
 * motor_controller_preparer.c
 *
 *  Created on: May 12, 2025
 *      Author: sergio
 */
#include "stm32f1xx_hal.h"
#include "three_phase_drive_runner.h"
#include "three_phase_drive_preparer.h"
#include "logger.h"


DMA_HandleTypeDef hdma_adc1 = {0};


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



void SystemClock_Config(void)
{
	RCC_OscInitTypeDef       RCC_OscInitStruct   = {0};
	RCC_ClkInitTypeDef       RCC_ClkInitStruct   = {0};
	//RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/* 1) Enable HSE oscillator and configure the PLL to reach 72 MHz */
	RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue      = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL9;              // 8 MHz × 9 = 72 MHz

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		//Error_Handler();
	}

	/* 2) Select PLL as system clock source and configure the HCLK, PCLK1, PCLK2 and ADC clocks dividers */
	RCC_ClkInitStruct.ClockType           = RCC_CLOCKTYPE_SYSCLK
										   | RCC_CLOCKTYPE_HCLK
										   | RCC_CLOCKTYPE_PCLK1
										   | RCC_CLOCKTYPE_PCLK2;

	RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;   // HCLK = 72 MHz
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;     // APB1 = 36 MHz (limite do periférico)
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;     // APB2 = 72 MHz

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		//Error_Handler();
	}


	/* 3) Configure USB clock as PLLCLK/1.5 → 72/1.5 = 48 MHz */
	//PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
	//PeriphClkInitStruct.UsbClockSelection    = RCC_USBCLKSOURCE_PLLCLK_DIV1_5;
	RCC->CFGR &= ~RCC_CFGR_USBPRE;
	__HAL_RCC_USB_CLK_ENABLE();

	//if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	//{
	//	Error_Handler();
	//}


	/* 4) CONFIGURA PRESCALER DO ADC EM /6 → 72/6 = 12 MHz */
	/* limpa e seta o campo ADCPRE em RCC->CFGR */
	//__HAL_RCC_ADCPCLK_ENABLE(); /* habilita escrita, se necessário */
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
	RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;


	/* 4) Configure SysTick for 1 ms interrupts (for HAL) */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* 5) SysTick IRQ priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


//void MX_ADC1_Init(ADC_HandleTypeDef *hadc1)
//{
//    ADC_MultiModeTypeDef multimode = {0};
//	ADC_ChannelConfTypeDef sConfig = {0};
//
//	hadc1->Instance = ADC1;
//	hadc1->Init.ScanConvMode = ADC_SCAN_DISABLE;
//	hadc1->Init.ContinuousConvMode = ENABLE;
//	hadc1->Init.DiscontinuousConvMode = DISABLE;
//	hadc1->Init.ExternalTrigConv = ADC_SOFTWARE_START;
//	hadc1->Init.DataAlign = ADC_DATAALIGN_RIGHT;
//	hadc1->Init.NbrOfConversion = 1;
//	HAL_ADC_Init(hadc1);
//
//	// Configura canal PA1 - ADC1_IN1
//	sConfig.Channel = ADC_CHANNEL_1;
//	sConfig.Rank = ADC_REGULAR_RANK_1;
//	sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
//	HAL_ADC_ConfigChannel(hadc1, &sConfig);
//
//	// Ativa modo dual com ADC2 simultâneo
//	multimode.Mode = ADC_DUALMODE_REGSIMULT;
//	HAL_ADCEx_MultiModeConfigChannel(hadc1, &multimode);
//}


//void MX_ADC2_Init(ADC_HandleTypeDef *hadc2)
//{
//	ADC_ChannelConfTypeDef sConfig = {0};
//
//	hadc2->Instance = ADC2;
//	hadc2->Init.ScanConvMode = ADC_SCAN_DISABLE;
//	hadc2->Init.ContinuousConvMode = ENABLE;
//	hadc2->Init.DiscontinuousConvMode = DISABLE;
//	hadc2->Init.ExternalTrigConv = ADC_SOFTWARE_START;
//	hadc2->Init.DataAlign = ADC_DATAALIGN_RIGHT;
//	hadc2->Init.NbrOfConversion = 1;
//	HAL_ADC_Init(hadc2);
//
//	// Configura canal PA2 - ADC2_IN2
//	sConfig.Channel = ADC_CHANNEL_2;
//	sConfig.Rank = ADC_REGULAR_RANK_1;
//	sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
//	HAL_ADC_ConfigChannel(hadc2, &sConfig);
//}

//
//void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim)
//{
////    GPIO_InitTypeDef GPIO_InitStruct = {0};
////    if (htim->Instance == TIM1)
////    {
////        /* Ativa clocks dos GPIOs usados */
////        __HAL_RCC_GPIOA_CLK_ENABLE();
////        __HAL_RCC_GPIOB_CLK_ENABLE();
////
////        /* PA8 - TIM1_CH1 */
////        GPIO_InitStruct.Pin = GPIO_PIN_8;
////        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
////        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
////        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
////
////        /* PA9 - TIM1_CH2 */
////        GPIO_InitStruct.Pin = GPIO_PIN_9;
////        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
////
////        /* PA10 - TIM1_CH3 */
////        GPIO_InitStruct.Pin = GPIO_PIN_10;
////        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
////
////        /* PB13 - TIM1_CH1N */
////        GPIO_InitStruct.Pin = GPIO_PIN_13;
////        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
////
////        /* PB14 - TIM1_CH2N */
////        GPIO_InitStruct.Pin = GPIO_PIN_14;
////        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
////
////        /* PB15 - TIM1_CH3N */
////        GPIO_InitStruct.Pin = GPIO_PIN_15;
////        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
////    }
//}


void MX_TIM1_Init(void)
{
//	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
//
//	TIM_HandleTypeDef htim1 = {0};
//	htim1.Instance               = TIM1;
//	htim1.Init.Prescaler         = 0;
//	htim1.Init.CounterMode       = TIM_COUNTERMODE_CENTERALIGNED1;
//	htim1.Init.Period            = PWM_PERIOD;
//	htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
//	htim1.Init.RepetitionCounter = 0;
//	HAL_TIM_PWM_Init(&htim1);
//
//	// Configura dead-time e estados de segurança
//	sBreakDeadTimeConfig.OffStateRunMode  = TIM_OSSR_DISABLE;
//	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
//	sBreakDeadTimeConfig.LockLevel        = TIM_LOCKLEVEL_OFF;
//	sBreakDeadTimeConfig.DeadTime         = DEAD_TIME;       // <-- seu ajuste
//	sBreakDeadTimeConfig.BreakState       = TIM_BREAK_DISABLE;
//	sBreakDeadTimeConfig.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
//	sBreakDeadTimeConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
//	HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);
//
//	// Configura comparadores para CH1, CH2, CH3
//	TIM_OC_InitTypeDef sConfigOC = {0};
//	sConfigOC.OCMode       = TIM_OCMODE_PWM1;
//	sConfigOC.Pulse        = 0;
//	sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
//	sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;  // ativa também a polaridade do canal neg.
//	sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
//	sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
//	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET; // idem para saída complementar
//
//	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
//	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
//	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);
//
//	HAL_TIM_MspPostInit(&htim1);
}






//void MX_ADC_DMA_Init(ADC_HandleTypeDef *hadc1)
//{
////	__HAL_RCC_DMA1_CLK_ENABLE();
////
////	hdma_adc1.Instance                 = DMA1_Channel1;
////	hdma_adc1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
////	hdma_adc1.Init.PeriphInc           = DMA_PINC_DISABLE;
////	hdma_adc1.Init.MemInc              = DMA_MINC_DISABLE;// DISABLE: Atribui a ultima leitura ao buffer | ENABLE: Acumula em fila circular no buffer do DMA
////	hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
////	hdma_adc1.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
////	hdma_adc1.Init.Mode                = DMA_CIRCULAR;
////	hdma_adc1.Init.Priority            = DMA_PRIORITY_HIGH;
////
////	HAL_DMA_Init(&hdma_adc1);
////
////	__HAL_LINKDMA(hadc1, DMA_Handle, hdma_adc1);
//}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin   = GPIO_PIN_13;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}



void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	  HAL_Delay(500); // 500 ms
  }
  /* USER CODE END Error_Handler_Debug */
}



void three_phase_drive_prepare(ThreePhaseDriveData *instance)
{
	HAL_Init();
	SystemClock_Config();

	logger_init(LOGGER_COMM_USB);

	MX_GPIO_Init();

	//ADC_HandleTypeDef hadc1 = {0};
	//ADC_HandleTypeDef hadc2 = {0};
	//MX_ADC1_Init(&hadc1);
	//MX_ADC_DMA_Init(&hadc1);
	//MX_ADC2_Init(&hadc2);
//
//    MX_TIM1_Init();
//
//
//
//	HAL_ADC_Start_DMA(&hadc1, instance->AdcCurrent, 1); // Inicia ADC1 com DMA (dual mode);
}



