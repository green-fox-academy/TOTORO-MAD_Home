/* Includes ------------------------------------------------------------------*/
#include "ac_control.h"
#include "stm32l4xx_hal.h"
#include "init.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define COMMAND_SIZE	268
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* Command arrays */
uint16_t degree_16[] = {4709, 2620, 326, 408, 367, 982, 326, 1023, 326, 408, 367, 982, 326, 408, 326, 449,
					 326, 408, 326, 408, 367, 982, 326, 408, 367, 408, 326, 1023, 326, 408, 326, 408,
					 367, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 982, 367, 408, 326, 408,
					 326, 449, 326, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 408,
					 326, 408, 326, 449, 326, 408, 326, 408, 367, 408, 326, 982, 367, 408, 326, 408,
				 	 367, 408, 326, 408, 367, 408, 326, 408, 326, 449, 326, 982, 367, 408, 326, 408,
					 367, 367, 367, 408, 326, 1023, 326, 982, 367, 408, 326, 982, 367, 408, 326, 408,
					 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 982,
					 326, 408, 326, 20151, 4668, 6593, 9337, 4996, 367, 408, 326, 408, 367, 408, 326,
					 408, 326, 449, 326, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367,
					 408, 326, 408, 326, 449, 326, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326,
					 408, 367, 408, 326, 408, 326, 449, 326, 408, 326, 408, 367, 408, 326, 408, 367,
					 408, 326, 408, 367, 408, 326, 408, 326, 449, 326, 408, 326, 408, 367, 408, 326,
					 408, 367, 408, 326, 408, 367, 408, 326, 408, 326, 449, 326, 408, 326, 408, 367,
					 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 326, 449, 326, 408, 326,
					 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 326, 408, 367,
					 408, 326, 408, 367, 408, 326, 408, 367, 367, 367, 20151, 4668, 15318};

uint16_t onoff[] = {4709, 2620, 367, 408, 326, 982, 367, 982, 326, 408, 367, 982,
					326, 408, 367, 408, 326, 408, 367, 408, 326, 982, 367, 408, 326, 408, 367, 982,
					326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 326, 1023,
					326, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408,
					367, 408, 326, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 982,
					326, 408, 326, 449, 326, 408, 326, 449, 326, 408, 326, 408, 367, 408, 326, 982,
					367, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 982,
					367, 982, 326, 408, 326, 449, 326, 408, 326, 449, 326, 408, 326, 1023, 326, 408,
					326, 408, 367, 408, 326, 982, 367, 20151, 4668, 6593, 9296, 4996, 367, 408, 326, 449,
					326, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408,
					326, 449, 326, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 367, 408,
					326, 408, 326, 449, 326, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408,
					367, 408, 326, 408, 326, 449, 326, 408, 326, 408, 367, 408, 326, 408, 367, 408,
					326, 408, 367, 408, 326, 408, 326, 449, 326, 408, 326, 408, 367, 408, 326, 408,
					367, 408, 326, 408, 367, 408, 326, 408, 326, 449, 326, 408, 326, 408, 367, 408,
					326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 326, 408, 367, 408, 326, 408,
					367, 408, 326, 408, 367, 408, 326, 408, 367, 408, 326, 408, 326, 20151, 4668, 15318};

/* PWM variables */
extern TIM_HandleTypeDef tim_pwm_handle;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void ac_ctrl()
{
	for (int i = 0; i < COMMAND_SIZE; i=i+2) {
		HAL_TIM_PWM_Start(&tim_pwm_handle, TIM_CHANNEL_1);
		delay(onoff[i]);
		HAL_TIM_PWM_Stop(&tim_pwm_handle, TIM_CHANNEL_1);
		delay(onoff[(i+1)]);
	}
}