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
#include "sensors.h"
#include "timer.h"
#include "stm32l4xx_hal_conf.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
#include "stm32l4xx_hal_rtc.h"
#include "stm32l4xx_hal_def.h"


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define SSID     "A66 Guest"
#define PASSWORD "Hello123"
#define SERVER_PORT 8002

#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000

#define TERMINAL_USE

#define CONNECTION_TRIAL_MAX          10

#define NTP_TIMESTAMP_DELTA 2208988800ull


/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
#if defined (TERMINAL_USE)
extern UART_HandleTypeDef hDiscoUart;
#endif /* TERMINAL_USE */
uint8_t RemoteIP[] = {10, 27, 99, 50};
char RxData [500];
char* modulename;
float TxData[3];
uint16_t RxLen;
uint8_t  MAC_Addr[6];
uint8_t  IP_Addr[4];

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void Error_Handler(void);
#if defined (TERMINAL_USE)
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
#endif /* TERMINAL_USE */
/* Private functions ---------------------------------------------------------*/


RTC_HandleTypeDef RtcHandle;

/* Buffers used for displaying Time and Date */
uint8_t aShowTime[50] = {0}, aShowTimeStamp[50] = {0};
uint8_t aShowDate[50] = {0}, aShowDateStamp[50] = {0};


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
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
	//Initialize sensors
	sensor_inits();

/*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follow:
	  - Hour Format    = Format 12
	  - Asynch Prediv  = Value according to source clock
	  - Synch Prediv   = Value according to source clock
	  - OutPut         = Output Disable
	  - OutPutPolarity = High Polarity
	  - OutPutType     = Open Drain */

	HAL_RTC_STATE_RESET(&RtcHandle);
	RtcHandle.Instance = RTC;
	RtcHandle.Init.HourFormat = RTC_HOURFORMAT_12;
	//RtcHandle.Init.AsynchPrediv = ;
	//RtcHandle.Init.SynchPrediv = ;
	RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	RtcHandle.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;

	 if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
	  {
		/* Initialization Error */
		Error_Handler();
	  }

	  /*##-2-  Configure RTC Timestamp ############################################*/
	  RTC_TimeStampConfig();

	  /* Infinite loop */
	  while (1)
	  {
		/*##-3- Display the updated Time and Date ################################*/
		RTC_CalendarShow();
	  }



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
				TxData[0] = get_temperature();
				TxData[1] = get_humidity();
				TxData[2] = get_pressure();

				printf("temperature: %.2f°C\t humidity: %.2f""%""\t pressure: %.2f\n", TxData[0], TxData[1], TxData[2]);

				if(WIFI_SendData(Socket, TxData, sizeof(TxData), &Datalen, WIFI_WRITE_TIMEOUT) != WIFI_STATUS_OK) {
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

void ntp_client(int argc, char* argv[ ])
{
	int sockfd;
	int n;             // Socket file descriptor and the n return result from sending/receiving from the socket.
	int port_number = 123;  // NTP UDP port number.

	char* host_name = "europe.pool.ntp.org"; // NTP server host-name

	 // Structure that defines the 48 byte NTP packet protocol.
	typedef struct
	{
	  unsigned li   : 2;          // Only two bits. Leap indicator.
	  unsigned vn   : 3;          // Only three bits. Version number of the protocol.
	  unsigned mode : 3;          // Only three bits. Mode. Client will pick mode 3 for client.

	  uint8_t stratum;           // Eight bits. Stratum level of the local clock.
	  uint8_t poll;              // Eight bits. Maximum interval between successive messages.
	  uint8_t precision;         // Eight bits. Precision of the local clock.

	  uint32_t rootDelay;        // 32 bits. Total round trip delay time.
	  uint32_t rootDispersion;   // 32 bits. Max error aloud from primary clock source.
	  uint32_t refId;            // 32 bits. Reference clock identifier.

	  uint32_t refTm_s;          // 32 bits. Reference time-stamp seconds.
	  uint32_t refTm_f;          // 32 bits. Reference time-stamp fraction of a second.

	  uint32_t origTm_s;         // 32 bits. Originate time-stamp seconds.
	  uint32_t origTm_f;         // 32 bits. Originate time-stamp fraction of a second.

	  uint32_t rxTm_s;           // 32 bits. Received time-stamp seconds.
	  uint32_t rxTm_f;           // 32 bits. Received time-stamp fraction of a second.

	  uint32_t txTm_s;           // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
	  uint32_t txTm_f;           // 32 bits. Transmit time-stamp fraction of a second.


	}ntp_packet;                         // Total: 384 bits or 48 bytes.

	//create packet
	ntp_packet packet = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	memset(&packet, 0, sizeof(ntp_packet));

	// Set the first byte's bits to 00,011,011 for li = 0,vn = 3,and mode = 3. The rest will be left set to zero.
	*((char*)&packet + 0) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.

	// Create a UDP socket, convert the host-name to an IP address, set the port number,
	// connect to the server,send the packet,and then read in the return packet.
	struct sockaddr_in serv_addr;  // Server address data structure.
	struct hostent* server;   // Server data structure.


}

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

#if defined (TERMINAL_USE)
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
#endif /* TERMINAL_USE */

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
