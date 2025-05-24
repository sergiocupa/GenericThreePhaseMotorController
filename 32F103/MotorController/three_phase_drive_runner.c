/*
 * motor_controller_runner.c
 *
 *  Created on: May 12, 2025
 *      Author: sergio
 */
#include "stm32f1xx_hal.h"
#include "three_phase_drive_preparer.h"
#include "logger.h"
#include <math.h>

float theta = 0.0f;
float Ia, Ib, Ic;           // Correntes das fases
float I_alpha, I_beta;      // Correntes no sistema αβ
float I_d, I_q;             // Correntes no sistema dq
float Iq_ref = 1.0f;        // torque reference
float Id_ref = 0.0f;        // torque reference
float int_d = 0, int_q = 0; // PI controller integrals
float V_d, V_q;
float V_alpha, V_beta;
float va, vb, vc;           // Tensões trifásicas
uint32_t current_values[2] = {0,0};
ThreePhaseDriveData *Instance;

//static bool USE_ZERO_SEQUENCE = false;



void logger_voltages(float va, float vb, float vc)
{
	LoggerParam params[] =
	{
		{.DataType = TYPE_FLOAT, .Address = 1, .Size = sizeof(float), .Data = &va},
		{.DataType = TYPE_FLOAT, .Address = 2, .Size = sizeof(float), .Data = &vb},
		{.DataType = TYPE_FLOAT, .Address = 3, .Size = sizeof(float), .Data = &vc}
	};

	logger_send(params, 3);
}




void get_adc_1_2(uint32_t values[2])
{
	values[0] = Instance->AdcCurrent[0] & 0xFFFF;
	values[1] = (Instance->AdcCurrent[0] >> 16) & 0xFFFF;
}



void three_phase_drive_run(ThreePhaseDriveData *instance)
{
	Instance = instance;

	while (1)
	{
		get_adc_1_2(current_values);

		// 01. Avancar rotor. Se usar encoder ou pulso externo, entao avancar por algumas destas referencias
		theta += 0.01;
		if (theta > PI_2) theta -= PI_2;

		// 02. Read currents (Ia and Ib) updated in DMA callback
		Ia = ((float)current_values[0] - 2048) * ADC_RESOLUTION;
		Ib = ((float)current_values[1] - 2048) * ADC_RESOLUTION;

		// 03. Clarke transform
		I_alpha = Ia;
		I_beta  = (Ia + 2.0f * Ib) / SQUARE_3;

		// 04. Park transform
		float c = cosf(theta);// para STM32F407, usar arm_cos_f32
		float s = sinf(theta);// para STM32F407, usar arm_sin_f32
		I_d =  c * I_alpha + s * I_beta;
		I_q = -s * I_alpha + c * I_beta;

		// 05. PI controllers
		float err_d = Id_ref - I_d;
		float err_q = Iq_ref - I_q;
		int_d += err_d;
		int_q += err_q;
		V_d = Kp*err_d + Ki*int_d;
		V_q = Kp*err_q + Ki*int_q;

		// 06. Inverse Park
		V_alpha =  c*V_d - s*V_q;
		V_beta  =  s*V_d + c*V_q;

		// 07. Inverse clarke transform
		va = V_alpha;
		vb = (-V_alpha + SQUARE_3 * V_beta) / 2;
		vc = (-V_alpha - SQUARE_3 * V_beta) / 2;

		//if (USE_ZERO_SEQUENCE)
		//{
//			vmax = fmaxf(fmaxf(va, vb), vc);
//			vmin = fminf(fminf(va, vb), vc);
//			v0 = -0.5f * (vmax + vmin);
//			va += v0;  vb += v0;  vc += v0;
		//}

		logger_voltages(va, vb, vc);

		// 08. Converter tensões para duty cycles (0 a PWM_PERIOD)
		//TIM1->CCR1 = (va + 1.0) * PWM_PERIOD / 2; // Normalizar de -1 a 1 para 0 a PWM_PERIOD
		//TIM1->CCR2 = (vb + 1.0) * PWM_PERIOD / 2;
		//TIM1->CCR3 = (vc + 1.0) * PWM_PERIOD / 2;
	}
}
