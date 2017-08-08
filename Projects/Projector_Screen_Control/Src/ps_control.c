/* Includes ------------------------------------------------------------------*/
#include "ps_control.h"
#include "stm32l4xx_hal.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define COMMAND_SIZE 24
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* Command arrays */
uint8_t ctrl_up_arr[COMMAND_SIZE] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
									 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0};

uint8_t ctrl_down_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
									  1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1};

uint8_t ctrl_stop_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
		   	   	   	   	   	   	   	  1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1};

uint8_t ctrl_verification_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
		   	   	   	   	   	   	   	  	  	  1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1};
/* PWM variables */
extern TIM_HandleTypeDef tim_pwm_handle;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void end_bit()
{
	HAL_TIM_PWM_Start(&tim_pwm_handle, TIM_CHANNEL_1);
	delay(1);
	HAL_TIM_PWM_Stop(&tim_pwm_handle, TIM_CHANNEL_1);
}

void bit_one()
{
	HAL_TIM_PWM_Start(&tim_pwm_handle, TIM_CHANNEL_1);
	delay(1);
	HAL_TIM_PWM_Stop(&tim_pwm_handle, TIM_CHANNEL_1);
	delay(3);
}

void bit_zero()
{
	HAL_TIM_PWM_Start(&tim_pwm_handle, TIM_CHANNEL_1);
	delay(3);
	HAL_TIM_PWM_Stop(&tim_pwm_handle, TIM_CHANNEL_1);
	delay(1);
}

void ctrl_up()
{
	for (int i = 0; i < COMMAND_SIZE; i++) {
		if (ctrl_up_arr[i] == 1) {
			bit_one();
		} else {
			bit_zero();
		}
	}
	end_bit();
	delay(31);
	for (int i = 0; i < COMMAND_SIZE; i++) {
		if (ctrl_up_arr[i] == 1) {
			bit_one();
		} else {
			bit_zero();
		}
	}
	end_bit();
	delay(31);
	for (int i = 0; i < COMMAND_SIZE; i++) {
		if (ctrl_verification_arr[i] == 1) {
			bit_one();
		} else {
			bit_zero();
		}
	}
	end_bit();
}

void ctrl_down()
{
	for (int i = 0; i < COMMAND_SIZE; i++) {
		if (ctrl_down_arr[i] == 1) {
			bit_one();
		} else {
			bit_zero();
		}
	}
	end_bit();
	delay(31);
	for (int i = 0; i < COMMAND_SIZE; i++) {
		if (ctrl_down_arr[i] == 1) {
			bit_one();
		} else {
			bit_zero();
		}
	}
	end_bit();
	delay(31);
	for (int i = 0; i < COMMAND_SIZE; i++) {
		if (ctrl_verification_arr[i] == 1) {
			bit_one();
		} else {
			bit_zero();
		}
	}
	end_bit();
}

void ctrl_stop()
{
	for (int i = 0; i < COMMAND_SIZE; i++) {
		if (ctrl_stop_arr[i] == 1) {
			bit_one();
		} else {
			bit_zero();
		}
	}
	end_bit();
	delay(31);
	for (int i = 0; i < COMMAND_SIZE; i++) {
		if (ctrl_stop_arr[i] == 1) {
			bit_one();
		} else {
			bit_zero();
		}
	}
	end_bit();
	delay(31);
	for (int i = 0; i < COMMAND_SIZE; i++) {
		if (ctrl_verification_arr[i] == 1) {
			bit_one();
		} else {
			bit_zero();
		}
	}
	end_bit();
}
