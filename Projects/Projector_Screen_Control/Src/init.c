/* Includes ------------------------------------------------------------------*/
#include "init.h"
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "wifi.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
UART_HandleTypeDef hDiscoUart;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void uart_init()
{
	/* Initialize all configured peripherals */
	hDiscoUart.Instance = DISCOVERY_COM1;
	hDiscoUart.Init.BaudRate = 115200;
	hDiscoUart.Init.WordLength = UART_WORDLENGTH_8B;
	hDiscoUart.Init.StopBits = UART_STOPBITS_1;
	hDiscoUart.Init.Parity = UART_PARITY_NONE;
	hDiscoUart.Init.Mode = UART_MODE_TX_RX;
	hDiscoUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	hDiscoUart.Init.OverSampling = UART_OVERSAMPLING_16;
	hDiscoUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	hDiscoUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	BSP_COM_Init(COM1, &hDiscoUart);

}

