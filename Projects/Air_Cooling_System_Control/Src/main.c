/**
  ******************************************************************************
\  * @file    BSP/Src/main.c
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
#define SSID     				"A66 Guest"
#define PASSWORD 				"Hello123"
#define SERVER_PORT 			8002
#define WIFI_WRITE_TIMEOUT 		10000
#define WIFI_READ_TIMEOUT  		10000
#define CONNECTION_TRIAL_MAX    10
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
uint8_t remote_ip[] = {10, 27, 99, 50};
char rx_data[3];
uint8_t mac_addr[6];
uint8_t ip_addr[4];

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void) {
	int8_t socket = -1;
	uint16_t datalen = 0;
	uint16_t trials = CONNECTION_TRIAL_MAX;
	uint8_t comm;

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Configure LED2 */
	BSP_LED_Init(LED2);

	/* Initialize UART */
	uart_init();

	/* Initialize Timer for for proper delay */
	time_base_init();

	/* Initialize timer in PWM mode */
	pwm_init();
	 /*Initialize  WIFI module */
	if(WIFI_Init() ==  WIFI_STATUS_OK) {
		printf("> WIFI Module Initialized.\n");
	} else {
		error_handling("> ERROR : WIFI Module cannot be initialized.\n", WIFI_STATUS_ERROR);
	}
	/*Getting MAC address */
	if(WIFI_GetMAC_Address(mac_addr) == WIFI_STATUS_OK) {
		printf("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\n",
			   mac_addr[0],
			   mac_addr[1],
			   mac_addr[2],
			   mac_addr[3],
			   mac_addr[4],
			   mac_addr[5]);
	} else {
		error_handling("> ERROR : CANNOT get MAC address\n", WIFI_STATUS_ERROR);
	}
	while (1) {

		/*Waiting for connection with WIFI AP */
		printf("> Trying to connect to %s.\n", SSID);
		while (WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK) != WIFI_STATUS_OK);
		printf("> Connected to %s!\n", SSID);

		/*Getting IP Address */
		if (WIFI_GetIP_Address(ip_addr) == WIFI_STATUS_OK) {
			printf("> es-wifi module got IP Address : %d.%d.%d.%d\n",
					ip_addr[0],
					ip_addr[1],
					ip_addr[2],
					ip_addr[3]);
		} else {
			error_handling("> ERROR : es-wifi module CANNOT get IP address\n", WIFI_STATUS_ERROR);
		}
		/*do-while connected to WIFI AP(checking connection by pinging own IP Address) */
		do {
			int8_t socket = 0;
			printf("Start TCP Server...\n");
			while(WIFI_StartServer(socket, WIFI_TCP_PROTOCOL, "asd", SERVER_PORT) != WIFI_STATUS_OK);
			printf("----------------------------------------- \n");
			printf("TCP Server Started \n");
			printf("receiving data...\n");
			/*trying to connect to server and sending data when connected in every 10 seconds */
			do {
				if(datalen > 0) {
					comm = atoi(rx_data);
					printf("comm: %d\n", comm);
					set_ac(comm);
					datalen = 0;
				}
			} while (WIFI_ReceiveData(socket, &rx_data, sizeof(rx_data), &datalen, 0) == WIFI_STATUS_OK);
			/*Closing socket when connection is lost or could'not connect */
			printf("Closing the socket...\n");
			WIFI_CloseClientConnection(socket);
			WIFI_StopServer(socket);
		} while (wifi_isconnected() == 1); //do-while
		/*If there might be a problem with pinging, disconnect from WIFI AP anyway */
		printf("> Disconnected from WIFI!\n");
		WIFI_Disconnect();
	    }	//while (1)

}//main

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
