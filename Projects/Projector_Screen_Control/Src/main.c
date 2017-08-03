/**
  ******************************************************************************
  * @file    BSP/Src/main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    21-April-2017
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SSID     "A66 Guest"
#define PASSWORD "Hello123"
#define SERVER_PORT 8002
#define COMMAND_SIZE 24

#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000

#define CONNECTION_TRIAL_MAX          10
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
extern UART_HandleTypeDef hDiscoUart;
uint8_t RemoteIP[] = {10, 27, 99, 50};
uint8_t RxData [500];
char* modulename;
float TxData[3];
uint16_t RxLen;
uint8_t  MAC_Addr[6];
uint8_t  IP_Addr[4];
uint8_t ctrl_up_arr[COMMAND_SIZE] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
									 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0};

uint8_t ctrl_down_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
									  1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1};

uint8_t ctrl_stop_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
		   	   	   	   	   	   	   	  1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1};

uint8_t ctrl_verification_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
		   	   	   	   	   	   	   	  	  	  1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1};
TIM_HandleTypeDef timh;
TIM_OC_InitTypeDef occonf;
GPIO_InitTypeDef GPIO_tim;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void Error_Handler(void);
void timer_init();
void end_bit();
void bit_one();
void bit_zero();
void ctrl_up();
void ctrl_down();
void ctrl_stop();
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void) {
	int32_t Socket = -1;
	uint16_t Datalen;
	uint16_t Trials = CONNECTION_TRIAL_MAX;

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Configure LED2 */
	BSP_LED_Init(LED2);

	timer_init();

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

	printf("****** WIFI Module in TCP Client mode demonstration ****** \n\n");
	printf("TCP Client Instructions :\n");
	printf("1- Make sure your Phone is connected to the same network that\n");
	printf("   you configured using the Configuration Access Point.\n");
	printf("2- Create a server by using the android application TCP Server with right port.\n");
	printf("3- Get the Network Name or IP Address of your Android from the step 2.\n\n");

	/*Initialize  WIFI module */
	if(WIFI_Init() ==  WIFI_STATUS_OK)
	{
		printf("> WIFI Module Initialized.\n");
		if(WIFI_GetMAC_Address(MAC_Addr) == WIFI_STATUS_OK)
		{
			printf("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\n",
				   MAC_Addr[0],
				   MAC_Addr[1],
				   MAC_Addr[2],
				   MAC_Addr[3],
				   MAC_Addr[4],
				   MAC_Addr[5]);
		} else {
			printf("> ERROR : CANNOT get MAC address\n");
			BSP_LED_On(LED2);
		}

		if (WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK) == WIFI_STATUS_OK) {
			printf("> es-wifi module connected \n");
			if (WIFI_GetIP_Address(IP_Addr) == WIFI_STATUS_OK) {
				printf("> es-wifi module got IP Address : %d.%d.%d.%d\n",
						IP_Addr[0],
						IP_Addr[1],
						IP_Addr[2],
						IP_Addr[3]);

				printf("> Trying to connect to Server: %d.%d.%d.%d:%d ...\n",
						RemoteIP[0],
						RemoteIP[1],
						RemoteIP[2],
						RemoteIP[3],
						SERVER_PORT);

				while (Trials--) {
					if (WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, "TCP_CLIENT", RemoteIP, SERVER_PORT, 0) == WIFI_STATUS_OK) {
						printf("> TCP Connection opened successfully.\n");
						Socket = 0;
					}
				}

				if (!Trials) {
					printf("> ERROR : Cannot open Connection\n");
					BSP_LED_On(LED2);
				}
			} else {
				printf("> ERROR : es-wifi module CANNOT get IP address\n");
				BSP_LED_On(LED2);
			}
		} else {
			printf("> ERROR : es-wifi module NOT connected\n");
			BSP_LED_On(LED2);
		}
	} else {
		printf("> ERROR : WIFI Module cannot be initialized.\n");
		BSP_LED_On(LED2);
	}

	while (1) {
		if (Socket != -1) {
			do {
				if(WIFI_ReceiveData(Socket, RxData, sizeof(RxData), &Datalen, WIFI_WRITE_TIMEOUT) != WIFI_STATUS_OK) {
					Socket = -1;
					printf("disconnected from server\n");
				}
				HAL_Delay(10000);
			} while (Datalen > 0);
		} else {
			printf("trying to reconnect\n");
			if (WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, "TCP_CLIENT", RemoteIP, SERVER_PORT, 0) == WIFI_STATUS_OK) {
				printf("> TCP Connection opened successfully.\n");
				Socket = 0;
			}
			HAL_Delay(5000);
		}//else
	}//while
}//main

void timer_init()
{
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* TIM3 Peripheral clock enable */
	__HAL_RCC_TIM3_CLK_ENABLE();
	/* Enable GPIO Channels Clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* Configure PA.15 (connected to D9) (TIM3_Channel1)*/
	GPIO_tim.Mode = GPIO_MODE_AF_PP;
	GPIO_tim.Pull = GPIO_NOPULL;
	GPIO_tim.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_tim.Alternate = GPIO_AF2_TIM3;
	GPIO_tim.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOB, &GPIO_tim);


	timh.Instance = TIM3;
	timh.Init.Prescaler         = 20;
	timh.Init.Period            = 100;
	timh.Init.ClockDivision     = 0;
	timh.Init.CounterMode       = TIM_COUNTERMODE_UP;
	timh.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&timh) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	occonf.OCFastMode = TIM_OCFAST_DISABLE;
	occonf.OCIdleState = TIM_OCIDLESTATE_RESET;
	occonf.OCMode = TIM_OCMODE_PWM1;
	occonf.OCPolarity = TIM_OCPOLARITY_HIGH;
	occonf.Pulse = 100;

	/* Set the pulse value for channel 1 */
	if (HAL_TIM_PWM_ConfigChannel(&timh, &occonf, TIM_CHANNEL_1) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}

	/*##-3- Start PWM signals generation #######################################
	/* Start channel 1
	if (HAL_TIM_PWM_Start(&timh, TIM_CHANNEL_1) != HAL_OK)
	{
	 	 PWM Generation Error
		Error_Handler();
	}*/

	while (1) {
		ctrl_up();
		HAL_Delay(1000);
		//ctrl_down();
		//HAL_Delay(1000);
		//ctrl_stop();
		//HAL_Delay(1000);
	}
}
void end_bit()
{
	HAL_TIM_PWM_Start(&timh, TIM_CHANNEL_1);
	HAL_Delay(1);
	HAL_TIM_PWM_Stop(&timh, TIM_CHANNEL_1);
}

void bit_one()
{
	HAL_TIM_PWM_Start(&timh, TIM_CHANNEL_1);
	HAL_Delay(1);
	HAL_TIM_PWM_Stop(&timh, TIM_CHANNEL_1);
	HAL_Delay(3);
}

void bit_zero()
{
	HAL_TIM_PWM_Start(&timh, TIM_CHANNEL_1);
	HAL_Delay(3);
	HAL_TIM_PWM_Stop(&timh, TIM_CHANNEL_1);
	HAL_Delay(1);
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
	HAL_Delay(34);
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
}

/**
  * @brief  Configure all GPIO's to AN to reduce the power consumption
  * @param  None
  * @retval None
  */

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&hDiscoUart, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

void Error_Handler(void)
{  
  /* User can add his own implementation to report the HAL error return state */
  printf("!!! ERROR !!!\n");
  BSP_LED_On(LED2);
  while(1) 
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
