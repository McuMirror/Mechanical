#include "Actuator_Lib.h"
#include <math.h>

#define N_SAMPLES 20.0
#define RPM_CLOCK_FREQ 5e5 // In Hz
#define PI 3.1415926535

#define BRAKE_ENABLE GPIO_PIN_RESET
#define BRAKE_DISABLE GPIO_PIN_SET

static TIM_HandleTypeDef *ht_rpm, *ht_pwm;
static uint32_t FWD_REV_Pin, BRAKE_Pin;
static GPIO_TypeDef *FWD_REV_Port, *BRAKE_Port;
static uint16_t time_val;
static uint8_t IC_flag = 0;

static float avg_speed = 0;

void initActuators(TIM_HandleTypeDef *htim_pwm, TIM_HandleTypeDef *htim_rpm, 
	uint32_t rw_fwd_rev_pin, GPIO_TypeDef* rw_fwd_rev_port, uint32_t rw_brake_pin, GPIO_TypeDef* rw_brake_port)
{
    // initialize local module level timer handlers
    ht_rpm = htim_rpm; // timer handler for rpm calc
    ht_pwm = htim_pwm; // timer handler for pwm output
    
    // start timer for PWM
    HAL_TIM_Base_Start(ht_pwm);
    
    // start interrupt handler
    HAL_TIM_Base_Start_IT(ht_rpm);
    HAL_TIM_IC_Start_IT(ht_rpm, RW_PWM_CHANNEL);
    
    // set PWM signal frequency
    PWM_Set_Frequency(ht_pwm, 100);
    
    // starts PWM signal generation
    HAL_TIM_PWM_Start(ht_pwm, RW_PWM_CHANNEL);
    
    // start timer for RPM calculations
    ht_rpm->Instance->CCER |= 1;
    HAL_TIM_Base_Start(ht_rpm);

    // save pins
    FWD_REV_Pin = rw_fwd_rev_pin;
		FWD_REV_Port = rw_fwd_rev_port;
		BRAKE_Pin = rw_brake_pin;
		BRAKE_Port = rw_brake_port;
}

void rw_get_speed (float *speed)
{
  /* CODE TO CALCULATE RPM USING XOR OF 3 HALL EFF SENSORS */
	// local variable to store pulse time
	float xor_pulse_time;

	// time for rotation (us) assuming pulses are equidistant
	if(time_val != 0) {
		xor_pulse_time = 6.0 * time_val;        // time to rotate in us
		xor_pulse_time = xor_pulse_time         // (us/rev)
						 * (1.0/RPM_CLOCK_FREQ)    // (s/us)
						 * (1.0/60);            // (min/s)

		// calculate speed (RPM)
		*speed = (1.0/xor_pulse_time);
	}
	else {
		*speed = 0;
	}

	// calculate speed (RPM)
	/*float time = time_val           // (ticks)
			 * (1.0/RPM_CLOCK_FREQ); 		// (s/tick)
	
	if (!time_val) {
			*speed = 0;
	} else {
			*speed = 2*PI/time;
	}*/

	// calculate average speed (moving average)
	avg_speed -= avg_speed/N_SAMPLES;
	avg_speed += *speed/N_SAMPLES;

	*speed = avg_speed;
}

void rw_set_speed(float pwm, uint8_t brake) {
	if(brake) {
		HAL_GPIO_WritePin(BRAKE_Port, BRAKE_Pin, BRAKE_ENABLE);
		
		// Invert PWM
//		if(pwm > 0) { 
//			PWM_Set_Duty_Cycle(ht_pwm, 100.0 - pwm, RW_PWM_CHANNEL);
//		}
//		else {
//			PWM_Set_Duty_Cycle(ht_pwm, -100.0 - pwm, RW_PWM_CHANNEL);
//		}
		
		PWM_Set_Duty_Cycle(ht_pwm, fabsf(pwm), RW_PWM_CHANNEL);
	}
	else {
		HAL_GPIO_WritePin(BRAKE_Port, BRAKE_Pin, BRAKE_DISABLE);
		
		// check direction
		if (pwm > 0) {
				HAL_GPIO_WritePin(FWD_REV_Port, FWD_REV_Pin, GPIO_PIN_RESET);
		} else {
				pwm *= -1;
				HAL_GPIO_WritePin(FWD_REV_Port, FWD_REV_Pin, GPIO_PIN_SET);
		}
		
		PWM_Set_Duty_Cycle(ht_pwm, pwm, RW_PWM_CHANNEL);
	}
}

void HAL_TIM_IC_CaptureCallback (TIM_HandleTypeDef *htim) {
    time_val = htim->Instance->CCR1;
    IC_flag = 1;
}

void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim) {
    if (!IC_flag) {
        time_val = 0;
    }
    IC_flag = 0;
}