/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f1xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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


DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hadc->Instance==ADC1)
  {
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);
  }
  else if(hadc->Instance==ADC2)
  {
	  __HAL_RCC_ADC2_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  GPIO_InitStruct.Pin = GPIO_PIN_2;
	  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	  hdma_adc2.Instance                 = DMA1_Channel2; // DMA1_Channel2 é o canal para ADC2 no F103
	  hdma_adc2.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	  hdma_adc2.Init.PeriphInc           = DMA_PINC_DISABLE;
	  hdma_adc2.Init.MemInc              = DMA_MINC_ENABLE;
	  hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	  hdma_adc2.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
	  hdma_adc2.Init.Mode                = DMA_CIRCULAR;
	  hdma_adc2.Init.Priority            = DMA_PRIORITY_LOW; // Ou DMA_PRIORITY_HIGH se for mais crítico

	  if (HAL_DMA_Init(&hdma_adc2) != HAL_OK)
	  {
		Error_Handler();
	  }

	  __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc2);
  }

}

/**
  * @brief ADC MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
    __HAL_RCC_ADC1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
    HAL_DMA_DeInit(hadc->DMA_Handle);
  }
  else if(hadc->Instance==ADC2)
  {
	  __HAL_RCC_ADC2_CLK_DISABLE();
	  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
	  HAL_DMA_DeInit(hadc->DMA_Handle);
  }

}


void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /** DISABLE: JTAG-DP Disabled and SW-DP Disabled
  */
  __HAL_AFIO_REMAP_SWJ_DISABLE();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
