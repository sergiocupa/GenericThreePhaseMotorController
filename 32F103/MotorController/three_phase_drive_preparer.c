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

ADC_HandleTypeDef Hadc1    = {0};
ADC_HandleTypeDef Hadc2    = {0};

int Adc1Buffer[ADC_BUFFER_SIZE]; // Buffer para ADC1 (PA1)
int Adc2Buffer[ADC_BUFFER_SIZE]; // Buffer para ADC2 (PA2)



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
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_ADC2_CLK_ENABLE();
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
	RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;//  RCC_CFGR_ADCPRE_DIV6;


	/* 4) Configure SysTick for 1 ms interrupts (for HAL) */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* 5) SysTick IRQ priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}



void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (htim->Instance == TIM1)
    {
        /* Ativa clocks dos GPIOs usados */
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* PA8 - TIM1_CH1 */
        GPIO_InitStruct.Pin   = GPIO_PIN_8;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* PA9 - TIM1_CH2 */
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* PA10 - TIM1_CH3 */
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* PB13 - TIM1_CH1N */
        GPIO_InitStruct.Pin = GPIO_PIN_13;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* PB14 - TIM1_CH2N */
        GPIO_InitStruct.Pin = GPIO_PIN_14;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* PB15 - TIM1_CH3N */
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}


void MX_TIM1_Init(void)
{
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	TIM_HandleTypeDef htim1 = {0};
	htim1.Instance               = TIM1;
	htim1.Init.Prescaler         = 0;
	htim1.Init.CounterMode       = TIM_COUNTERMODE_CENTERALIGNED1;
	htim1.Init.Period            = PWM_PERIOD;
	htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	HAL_TIM_PWM_Init(&htim1);

	// Configura dead-time e estados de segurança
	sBreakDeadTimeConfig.OffStateRunMode  = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel        = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime         = DEAD_TIME;       // <-- seu ajuste
	sBreakDeadTimeConfig.BreakState       = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);

	// Configura comparadores para CH1, CH2, CH3
	TIM_OC_InitTypeDef sConfigOC = {0};
	sConfigOC.OCMode       = TIM_OCMODE_PWM1;
	sConfigOC.Pulse        = 0;
	sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;  // ativa também a polaridade do canal neg.
	sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET; // idem para saída complementar

	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);

	HAL_TIM_MspPostInit(&htim1);
}



void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin   = GPIO_PIN_13;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void MX_GPIOA_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configurar PA1 como Analógico
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Se usar PA2 para o ADC2 também:
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}



void Error_Handler(void)
{
  __disable_irq();
  int ix = 0;
  while (ix < 3)
  {
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
	  HAL_Delay(10);
	  ix++;
  }
}


void get_adc_1_2(uint32_t values[2])
{
	values[0] = Adc1Buffer[0];// & 0xFFFF;
	values[1] = Adc2Buffer[0];// >> 16) & 0xFFFF;
}


void __adc_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void __adc_dma_init()
{
	  __HAL_RCC_DMA1_CLK_ENABLE();
}

void __adc1_init(void)
{
	  ADC_ChannelConfTypeDef sConfig = {0};

	  Hadc1.Instance = ADC1;
	  Hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	  Hadc1.Init.ContinuousConvMode = ENABLE;
	  Hadc1.Init.DiscontinuousConvMode = DISABLE;
	  Hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	  Hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  Hadc1.Init.NbrOfConversion = 1;
	  if (HAL_ADC_Init(&Hadc1) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  sConfig.Channel = ADC_CHANNEL_1;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;// Para controle FOC, ajustar aqui. Tambem verificar sincronizacao das amostras com PWM
	  if (HAL_ADC_ConfigChannel(&Hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void __adc2_init(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};

	  Hadc2.Instance = ADC2;
	  Hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
	  Hadc2.Init.ContinuousConvMode = ENABLE;
	  Hadc2.Init.DiscontinuousConvMode = DISABLE;
	  Hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	  Hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  Hadc2.Init.NbrOfConversion = 1;
	  if (HAL_ADC_Init(&Hadc2) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  sConfig.Channel = ADC_CHANNEL_2;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	  if (HAL_ADC_ConfigChannel(&Hadc2, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}


// Embarcar funcoes de inicializacao, para evitar que o cube nao altere


void __adc_init(void)
{
	__adc_gpio_init();
	__adc_dma_init();
	__adc1_init();
	__adc2_init();

	if (HAL_ADC_Start_DMA(&Hadc1, (uint32_t*)Adc1Buffer, ADC_BUFFER_SIZE) != HAL_OK)
	{
	    Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&Hadc2, (uint32_t*)Adc2Buffer, ADC_BUFFER_SIZE) != HAL_OK)
	{
		Error_Handler();
	}
}

