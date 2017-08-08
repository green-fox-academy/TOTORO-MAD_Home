/* Includes ------------------------------------------------------------------*/
#include "ps_control.h"
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "wifi.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define COMMAND_SIZE 24
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
uint8_t ctrl_up_arr[COMMAND_SIZE] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
									 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0};

uint8_t ctrl_down_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
									  1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1};

uint8_t ctrl_stop_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
		   	   	   	   	   	   	   	  1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1};

uint8_t ctrl_verification_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
		   	   	   	   	   	   	   	  	  	  1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1};

/* UART variables */

/* PWM variables */
TIM_HandleTypeDef tim_pwm_handle;
TIM_OC_InitTypeDef pwm_conf;

/* TIM_Base variables */
TIM_HandleTypeDef tim_base_handle;
uint32_t prescalervalue = 0;	/* Prescaler declaration */

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
