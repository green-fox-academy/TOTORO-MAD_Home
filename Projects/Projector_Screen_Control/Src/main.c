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
#define SSID     "A66 Guest"
#define PASSWORD "Hello123"
#define SERVER_PORT 8002
#define COMMAND_SIZE 24

#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000

#define CONNECTION_TRIAL_MAX          10
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
uint8_t remote_ip[] = {10, 27, 99, 50};
uint8_t rx_data [500];
float tx_data[3];
uint16_t rx_Len;
uint8_t  mac_addr[6];
uint8_t  ip_addr[4];
uint8_t ctrl_up_arr[COMMAND_SIZE] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
									 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0};

uint8_t ctrl_down_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
									  1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1};

uint8_t ctrl_stop_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
		   	   	   	   	   	   	   	  1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1};

uint8_t ctrl_verification_arr[COMMAND_SIZE]= {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
		   	   	   	   	   	   	   	  	  	  1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1};

/* Prescaler declaration */
extern TIM_HandleTypeDef tim_pwm_handle;
extern TIM_HandleTypeDef tim_base_handle;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void delay(uint64_t Delay);
void end_bit();
void bit_one();
void bit_zero();
void ctrl_up();
void ctrl_down();
void ctrl_stop();

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void) {
	int32_t socket = -1;
	uint16_t datalen;
	uint16_t trials = CONNECTION_TRIAL_MAX;

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Configure LED2 */
	BSP_LED_Init(LED2);

	uart_init();

	//pwm_init();
	time_base_init();

	pwm_init();
	/*Initialize  WIFI module */
		if(WIFI_Init() ==  WIFI_STATUS_OK)
		{
			printf("> WIFI Module Initialized.\n");
			if(WIFI_GetMAC_Address(mac_addr) == WIFI_STATUS_OK)
			{
				printf("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\n",
					   mac_addr[0],
					   mac_addr[1],
					   mac_addr[2],
					   mac_addr[3],
					   mac_addr[4],
					   mac_addr[5]);
			} else {
				printf("> ERROR : CANNOT get MAC address\n");
				BSP_LED_On(LED2);
			}

			if (WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK) == WIFI_STATUS_OK) {
				printf("> es-wifi module connected \n");
				if (WIFI_GetIP_Address(ip_addr) == WIFI_STATUS_OK) {
					printf("> es-wifi module got IP Address : %d.%d.%d.%d\n",
							ip_addr[0],
							ip_addr[1],
							ip_addr[2],
							ip_addr[3]);

					printf("> Trying to connect to Server: %d.%d.%d.%d:%d ...\n",
							remote_ip[0],
							remote_ip[1],
							remote_ip[2],
							remote_ip[3],
							SERVER_PORT);

					while (trials--) {
						if (WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, "TCP_CLIENT", remote_ip, SERVER_PORT, 0) == WIFI_STATUS_OK) {
							printf("> TCP Connection opened successfully.\n");
							socket = 0;
						}
					}

					if (!trials) {
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
			if (socket != -1) {
				do {
					if(WIFI_SendData(socket, (uint8_t*)tx_data, sizeof(tx_data), &datalen, WIFI_WRITE_TIMEOUT) != WIFI_STATUS_OK) {
						printf("disconnected from server\n");
						WIFI_CloseClientConnection(socket);
						socket = -1;
					}
					HAL_Delay(10000);
				} while (datalen > 0);
			} else {
				printf("trying to reconnect\n");
				if (WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, "TCP_CLIENT", remote_ip, SERVER_PORT, 0) == WIFI_STATUS_OK) {
					printf("> TCP Connection opened successfully.\n");
					socket = 0;
				}
				HAL_Delay(5000);
			}//else
		}//while
	}//main



void delay(uint64_t Delay)
{
  uint64_t tickstart = __HAL_TIM_GET_COUNTER(&tim_base_handle);
  while ((__HAL_TIM_GET_COUNTER(&tim_base_handle) - tickstart) < Delay) {
  }

}

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
