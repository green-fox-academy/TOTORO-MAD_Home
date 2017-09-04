/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStruct;
I2C_HandleTypeDef i2c_handle;

/* TCP client variables */
#define I2C_TIMING		   0x0010104D // 40kHz, rise 100ns, fall 10ns
#define I2C_ADDRESS        0b1001000
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void i2c_init() {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_DISABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9; // we are going to use PB6 and PB7
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;			// set pins to alternate function
	GPIO_InitStruct.Pull = GPIO_PULLUP;			// enable pull up resistors
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB

	i2c_handle.Instance             = I2C1;
	i2c_handle.Init.Timing          = I2C_TIMING;
	i2c_handle.Init.OwnAddress1     = I2C_ADDRESS;
	i2c_handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c_handle.Init.OwnAddress2     = 0xFF;
	i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c_handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

	if(HAL_I2C_Init(&i2c_handle) != HAL_OK)
	{
	/* Initialization Error */
	Error_Handler();
	}

	HAL_I2CEx_ConfigAnalogFilter(&i2c_handle,I2C_ANALOGFILTER_ENABLE);
}

