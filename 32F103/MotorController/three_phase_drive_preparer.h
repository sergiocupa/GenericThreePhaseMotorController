/*
 * motor_controller_preparer.h
 *
 *  Created on: May 12, 2025
 *      Author: sergio
 */
#include "stm32f1xx_hal.h"

#ifndef INC_THREE_PHASE_DRIVE_PREPARER_H_
#define INC_THREE_PHASE_DRIVE_PREPARER_H_


#define PWM_PERIOD     4096        // PWM timer period
#define DEAD_TIME      100         // adjustable dead time
#define ADC_RESOLUTION 4096.0f     //
#define Kp             0.1f        // PI controller gains
#define Ki             0.01f       // PI controller gains
#define PI             3.14159f
#define PI_2           6.28318f
#define SQUARE_3       1.73205f


typedef void (*TransmissionCompletedCallback) (uint8_t *Buf, uint32_t *Len, uint8_t epnum);


typedef struct _ThreePhaseDriveData
{
	uint32_t AdcCurrent[1];
}
ThreePhaseDriveData;


void SystemClock_Config(void);
//void MX_ADC1_Init(ADC_HandleTypeDef *hadc1);
//void MX_ADC2_Init(ADC_HandleTypeDef *hadc2);
//void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void MX_TIM1_Init(void);
//void MX_ADC_DMA_Init(ADC_HandleTypeDef *hadc1);
void MX_GPIO_Init(void);
void Error_Handler(void);



#endif /* INC_THREE_PHASE_DRIVE_PREPARER_H_ */
