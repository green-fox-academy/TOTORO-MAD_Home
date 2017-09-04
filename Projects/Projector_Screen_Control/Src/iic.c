/* Includes ------------------------------------------------------------------*/
#include "iic.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStruct;
I2C_HandleTypeDef i2c_handle;
uint8_t data;

/* TCP client variables */
#define I2C_TIMING		   0x0010104D // 40kHz, rise 100ns, fall 10ns
#define I2C_ADDRESS        0b1001000
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void i2c_init() {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9; // we are going to use PB6 and PB7
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;			// set pins to alternate function
	GPIO_InitStruct.Pull = GPIO_PULLUP;			// enable pull up resistors
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB

	i2c_handle.Instance             = I2C1;
	i2c_handle.Init.Timing          = I2C_TIMING;
	i2c_handle.Init.OwnAddress1     = I2C_ADDRESS;
	i2c_handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	i2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c_handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

	HAL_I2C_Init(&i2c_handle);


	HAL_I2CE1_ConfigAnalogFilter(&i2c_handle, I2C_ANALOGFILTER_ENABLE);
}

//void get_temp() {
//
//
//	while(HAL_I2C_Master_Transmit(&i2c_handle, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 10000)!= HAL_OK)
//	  {
//	    /* Error_Handler() function is called when Timeout error occurs.
//	       When Acknowledge failure occurs (Slave don't acknowledge its address)
//	       Master restarts communication */
//	    if (HAL_I2C_GetError(&i2c_handle) != HAL_I2C_ERROR_AF)
//	    {
//	      Error_Handler();
//	    }
//	  }
//}

