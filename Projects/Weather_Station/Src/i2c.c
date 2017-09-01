/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStruct;
I2C_InitTypeDef I2C_InitStruct;

/* TCP client variables */
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void i2c_init() {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_DISABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9; // we are going to use PB6 and PB7
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;			// set pins to alternate function
	GPIO_InitStruct.Pull = GPIO_PULLUP;			// enable pull up resistors
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB

}
