/* Includes ------------------------------------------------------------------*/
#include "wifi_conn.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SSID     				"A66 Guest"
#define PASSWORD 				"Hello123"
#define SERVER_PORT 			8005
#define WIFI_WRITE_TIMEOUT 		1000
#define CONNECTION_TRIAL_MAX    10
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
uint8_t remote_ip[] = {10, 27, 99, 63};
float tx_data[3];
uint8_t mac_addr[6];
uint8_t ip_addr[4];
int32_t socket;
uint16_t datalen;
uint8_t conn_flag;

/* Private function prototypes -----------------------------------------------*/
void error_handling(const char *error_string, uint8_t error_code);
void wifi_init();
/* Private functions ---------------------------------------------------------*/

void send_sensor_data()
{
    /*Initialize  WIFI */
    wifi_init();

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
			printf("> Trying to connect to Server: %d.%d.%d.%d:%d ...\n",
					remote_ip[0],
					remote_ip[1],
					remote_ip[2],
					remote_ip[3],
					SERVER_PORT);
	    	/*Creating socket*/
			if (WIFI_OpenClientConnection(socket, WIFI_TCP_PROTOCOL, "TCP_CLIENT", remote_ip, SERVER_PORT, 10) != WIFI_STATUS_OK) {
				error_handling("> Cannot create socket!\n", WIFI_STATUS_ERROR);
			} else {

		    	/*Loading sensor data into buffer */
				tx_data[0] = get_temperature();
				tx_data[1] = get_humidity();
				tx_data[2] = get_pressure();
				conn_flag = 0;

		    	/*Trying to connect to server and sending data when connected in every 10 seconds */
				while (WIFI_SendData(socket, (uint8_t*)tx_data, sizeof(tx_data), &datalen, WIFI_WRITE_TIMEOUT) == WIFI_STATUS_OK) {
					tx_data[0] = get_temperature();
					tx_data[1] = get_humidity();
					tx_data[2] = get_pressure();
					conn_flag = 1;
					HAL_Delay(10000);
				}

		    	/*When connection is lost conn_flag equals 1, else client could'not connect to server */
				if (conn_flag == 1) {
					printf("> Disconnected from server!\n");
				} else {
					conn_flag = 0;
				}
			}
			/*Closing socket when connection is lost or could'not connect */
			WIFI_CloseClientConnection(socket);
		} while (WIFI_Ping(ip_addr, 1, 1) == WIFI_STATUS_OK);	//do-while
		/*If there might be a problem with pinging, disconnect from WIFI AP anyway */
		printf("> Disconnected from WIFI!\n");
		WIFI_Disconnect();
    }	//while (1)
}	//main

void wifi_init()
{
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
}

void error_handling(const char *error_string, uint8_t error_code)
{
	printf("Error: %s Error code: %d\n", error_string, error_code);
	BSP_LED_On(LED2);
}
