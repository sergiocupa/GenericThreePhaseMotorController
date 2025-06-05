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

static float MotionStep = 0.2f;

float Theta = 0.0f;
float Ia, Ib, Ic;           // Correntes das fases
float I_alpha, I_beta;      // Correntes no sistema αβ
float I_d, I_q;             // Correntes no sistema dq
float Iq_ref = 1.0f;        // torque reference
float Id_ref = 0.0f;        // torque reference
float int_d = 0, int_q = 0; // PI controller integrals
float V_d, V_q;
float V_alpha, V_beta;
float va, vb, vc;           // Tensões trifásicas
uint32_t current_values[3] = {0,0,0};
ThreePhaseDriveData *Instance;

//static bool USE_ZERO_SEQUENCE = false;
const float CPU_FREQ = 72000000.0f; // 72 MHz

static byte half_cycle_count = 0;
static float frequency_y = 0.0f;
static float Frequency = 0.0f;
static uint32_t last_fullcycle_ticks = 0;
static uint32_t delta_ticks;


typedef struct _LogFrame
{
	byte  Start01;
	byte  Start02;
	uint  FrameSize;
	uint  Counter;
	uint  TicksByIteration;
	float Adc01;
	float Adc02;
	float Adc03;
	float TotalCurrent;
	float Frequency;
	float MotionStep;
	float Theta;
	float WaveR;
	float WaveS;
	float WaveT;
}
__attribute__((packed)) LogFrame;

static LogFrame CurrentFrame = { .Start01 = 254, .Start02 = 253, .FrameSize = sizeof(LogFrame), .Counter = 0, .TicksByIteration = 0, .Adc01 = 0.0f, .Adc02 = 0.0f, .Adc03 = 0.0f, .TotalCurrent = 0.0f, .Frequency = 0.0f, .MotionStep = 0.0f, .Theta = 0.0f, .WaveR = 0.0f, .WaveS = 0.0f, .WaveT = 0.0f };


void logger_voltages(float va, float vb, float vc)
{
	if (CurrentFrame.Counter == UINT64_MAX) CurrentFrame.Counter = 0;
	CurrentFrame.Counter++;

	CurrentFrame.Adc01      = current_values[0];
	CurrentFrame.Adc02      = current_values[1];
	CurrentFrame.Adc03      = current_values[2];
	CurrentFrame.Frequency  = Frequency;
	CurrentFrame.MotionStep = MotionStep;
	CurrentFrame.Theta      = Theta;
	CurrentFrame.WaveR      = va;
	CurrentFrame.WaveS      = vb;
	CurrentFrame.WaveT      = vc;

	logger_send(&CurrentFrame, sizeof(LogFrame));
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

			Frequency = CPU_FREQ / (float)delta_ticks;

			last_fullcycle_ticks = agora_ticks;
			half_cycle_count = 0;
		}
	}

	frequency_y = y;
	return Frequency;
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


void three_phase_drive_run(ThreePhaseDriveData *instance)
{
	Instance = instance;

	DWT_Init();

	while (1)
	{
		//HAL_Delay(1);

		get_adc_1_2(current_values);

		// 01. Avancar rotor. Se usar encoder ou pulso externo, entao avancar por algumas destas referencias
		Theta += MotionStep;
		if (Theta > PI_2) Theta -= PI_2;

		// 02. Read currents (Ia and Ib) updated in DMA callback
		Ia = ((float)current_values[0] - 2048) * ADC_SCALE;
		Ib = ((float)current_values[1] - 2048) * ADC_SCALE;

		// 03. Clarke transform
		I_alpha = Ia;
		I_beta  = (Ia + 2.0f * Ib) / SQUARE_3;

		// 04. Park transform
		float c = cosf(Theta);// para STM32F407, usar arm_cos_f32
		float s = sinf(Theta);// para STM32F407, usar arm_sin_f32

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

		logger_voltages(va, vb, vc);

		// 08. Converter tensões para duty cycles (0 a PWM_PERIOD)
		//TIM1->CCR1 = (va + 1.0) * PWM_PERIOD / 2; // Normalizar de -1 a 1 para 0 a PWM_PERIOD
		//TIM1->CCR2 = (vb + 1.0) * PWM_PERIOD / 2;
		//TIM1->CCR3 = (vc + 1.0) * PWM_PERIOD / 2;
	}
}
