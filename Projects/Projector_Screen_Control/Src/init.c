/* Includes ------------------------------------------------------------------*/
#include "init.h"
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "wifi.h"
/* Private typedef -----------------------------------------------------------*/
#define  PERIOD_VALUE       0xFFFF	/* Period Value  */

/* Private define ------------------------------------------------------------*/
#define  PULSE_VALUE       (PERIOD_VALUE/2)	/* Duty cycle 50%  */

/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* UART variables */
UART_HandleTypeDef uart_handle;

/* PWM variables */
TIM_HandleTypeDef tim_pwm_handle;
TIM_OC_InitTypeDef pwm_conf;

/* TIM_Base variables */
TIM_HandleTypeDef tim_base_handle;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void error_handling(const char *error_string, uint8_t error_code);
void delay(uint16_t delay_value);

void uart_init()
{
	/* Initialize UART: COM1 port, Baudrate 115200, 8bit buffer length, 1 stop bit, no parity mode */
	uart_handle.Instance = DISCOVERY_COM1;
	uart_handle.Init.BaudRate = 115200;
	uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	uart_handle.Init.StopBits = UART_STOPBITS_1;
	uart_handle.Init.Parity = UART_PARITY_NONE;
	uart_handle.Init.Mode = UART_MODE_TX_RX;
	uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;
	uart_handle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	uart_handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	BSP_COM_Init(COM1, &uart_handle);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&uart_handle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

void time_base_init()
{
	/* Compute the prescaler value to have TIM2 counter clock equal to 1742 Hz, period Time 574 micro sec */
	uint32_t prescalervalue = (uint32_t)((SystemCoreClock) / 50) - 1;

	/* Set TIM2 instance */
	tim_base_handle.Instance = TIM2;

	/* Initialize TIMx peripheral as follows:
	   + Period = 65535
	   + Prescaler = ((SystemCoreClock)/1742) - 1
	   + ClockDivision = 0
	   + Counter direction = Up
	*/
	tim_base_handle.Init.Period            = PERIOD_VALUE;
	tim_base_handle.Init.Prescaler         = prescalervalue;
	tim_base_handle.Init.ClockDivision     = 0;
	tim_base_handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	tim_base_handle.Init.RepetitionCounter = 0;

	if (HAL_TIM_Base_Init(&tim_base_handle) != HAL_OK)
	{
		/* Initialization Error */
		error_handling("TIM Base initialization has failed!", HAL_ERROR);
	}

	/*##-2- Start the TIM Base generation in normal mode ####################*/
	/* Start Channel1 */
	if (HAL_TIM_Base_Start(&tim_base_handle) != HAL_OK)
	{
		/* Starting Error */
		error_handling("Starting TIM Base has failed!", HAL_ERROR);
	}
}

void delay(uint16_t delay_value)
{
  uint16_t tickstart = __HAL_TIM_GET_COUNTER(&tim_base_handle);
  while ((__HAL_TIM_GET_COUNTER(&tim_base_handle) - tickstart) < delay_value) {
  }
}

void pwm_init()
{
	/* Compute the prescaler value to have TIM3 counter clock equal to 40000 Hz */
	uint8_t prescalervalue = 20;

	/* Set TIM3 instance */
	tim_pwm_handle.Instance = TIM3;

	/* Initialize TIMx peripheral as follows:
	   + Prescaler = (SystemCoreClock / 380000) - 1
	   + Period = 65535
	   + ClockDivision = 0
	   + Counter direction = Up
	*/
	tim_pwm_handle.Init.Prescaler         = prescalervalue;
	tim_pwm_handle.Init.Period            = PERIOD_VALUE;
	tim_pwm_handle.Init.ClockDivision     = 0;
	tim_pwm_handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	tim_pwm_handle.Init.RepetitionCounter = 0;

	if (HAL_TIM_PWM_Init(&tim_pwm_handle) != HAL_OK)
	{
		/* Initialization Error */
		error_handling("TIM PWM initialization has failed!", HAL_ERROR);
	}

	pwm_conf.OCFastMode = TIM_OCFAST_DISABLE;
	pwm_conf.OCIdleState = TIM_OCIDLESTATE_RESET;
	pwm_conf.OCMode = TIM_OCMODE_PWM1;
	pwm_conf.OCPolarity = TIM_OCPOLARITY_HIGH;
	pwm_conf.Pulse = PULSE_VALUE;

	/* Set the pulse value for channel 1 */
	if (HAL_TIM_PWM_ConfigChannel(&tim_pwm_handle, &pwm_conf, TIM_CHANNEL_1) != HAL_OK)
	{
		/* Configuration Error */
		error_handling("TIM PWM channel configuration has failed!", HAL_ERROR);
	}
}

void error_handling(const char *error_string, uint8_t error_code)
{
	printf("Error: %s Error code: %d", error_string, error_code);
	BSP_LED_On(LED2);
}
