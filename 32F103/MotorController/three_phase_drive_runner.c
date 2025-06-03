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


#define V_REF 3.3f

double ADC_SCALE = V_REF / ADC_RESOLUTION;

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
const float CPU_FREQ = 72000000.0f; // 72 MHz

static uint counter = 0;
static byte half_cycle_count = 0;
static float frequency_y = 0.0f;
static float frequency = 0.0f;
static uint32_t last_fullcycle_ticks = 0;
static uint32_t delta_ticks;


void logger_voltages(float x, float va, float vb, float vc)
{
	if (counter == UINT64_MAX) counter = 0;
	counter++;
	logger_send(counter, "%d:%.3f|%d:%.3f|%d:%.3f|%d:%.3f|%d:%.3f|%d:%u", 1,x, 2,va, 3,vb, 4,vc, 5,frequency, 6,delta_ticks);
}


static inline void DWT_Init(void)
{
    // Habilita o DWT_CYCCNT no Cortex-M3
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // habilita Tracing
    DWT->CYCCNT = 0;                                // zera contador
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // liga contador de ciclos

    last_fullcycle_ticks = DWT->CYCCNT;
}


void get_adc_1_2(uint32_t values[2])
{
	values[0] = Instance->AdcCurrent[0] & 0xFFFF;
	values[1] = (Instance->AdcCurrent[0] >> 16) & 0xFFFF;
}


float get_frequency(float y)
{
	if ((frequency_y >= 0.0f && y < 0.0f) || (frequency_y <  0.0f && y >= 0.0f))
	{
		half_cycle_count++;

		if (half_cycle_count >= 2)
		{
			uint32_t agora_ticks = DWT->CYCCNT;

			delta_ticks = agora_ticks - last_fullcycle_ticks;

			frequency = CPU_FREQ / (float)delta_ticks;

			last_fullcycle_ticks = agora_ticks;
			half_cycle_count = 0;
		}
	}

	frequency_y = y;
	return frequency;
}


void three_phase_drive_run(ThreePhaseDriveData *instance)
{
	Instance = instance;

	DWT_Init();

	while (1)
	{
		//get_adc_1_2(current_values);

		// 01. Avancar rotor. Se usar encoder ou pulso externo, entao avancar por algumas destas referencias
		theta += 0.1;
		//if (theta > PI_2) theta -= PI_2;

		// 02. Read currents (Ia and Ib) updated in DMA callback
		Ia = ((float)current_values[0] - 2048) * ADC_SCALE;
		Ib = ((float)current_values[1] - 2048) * ADC_SCALE;

		// 03. Clarke transform
		I_alpha = Ia;
		I_beta  = (Ia + 2.0f * Ib) / SQUARE_3;

		// 04. Park transform
		float c = cosf(theta);// para STM32F407, usar arm_cos_f32
		float s = sinf(theta);// para STM32F407, usar arm_sin_f32

		get_frequency(c);

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

		logger_voltages(theta, va, vb, vc);

		// 08. Converter tensões para duty cycles (0 a PWM_PERIOD)
		//TIM1->CCR1 = (va + 1.0) * PWM_PERIOD / 2; // Normalizar de -1 a 1 para 0 a PWM_PERIOD
		//TIM1->CCR2 = (vb + 1.0) * PWM_PERIOD / 2;
		//TIM1->CCR3 = (vc + 1.0) * PWM_PERIOD / 2;
	}
}
