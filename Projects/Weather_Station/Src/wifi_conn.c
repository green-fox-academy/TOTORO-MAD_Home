/* Includes ------------------------------------------------------------------*/
#include "wifi_conn.h"

/* Private typedef -----------------------------------------------------------*/
// Structure that defines the 48 char NTP packet protocol.
// Check TWICE size of fields !!
typedef struct
{
  unsigned char li   : 2;          // Only two bits. Leap indicator.
  unsigned char vn   : 3;          // Only three bits. Version number of the protocol.
  unsigned char mode : 3;          // Only three bits. Mode. Client will pick mode 3 for client.

  unsigned char stratum;           // Eight bits. Stratum level of the local clock.
  unsigned char poll;              // Eight bits. Maximum interval between successive messages.
  unsigned char precision;         // Eight bits. Precision of the local clock.

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
}ntp_packet;                         // Total: 384 bits or 48 chars.

typedef struct tm {
   int tm_sec;         /* seconds,  range 0 to 59          */
   int tm_min;         /* minutes, range 0 to 59           */
   int tm_hour;        /* hours, range 0 to 23             */
   int tm_mday;        /* day of the month, range 1 to 31  */
   int tm_mon;         /* month, range 0 to 11             */
   int tm_year;        /* The number of years since 1900   */
   int tm_wday;        /* day of the week, range 0 to 6    */
   int tm_yday;        /* day in the year, range 0 to 365  */
   int tm_isdst;       /* daylight saving time             */
}rtc_time;

/* Private define ------------------------------------------------------------*/
#define SSID     				"A66 Guest"
#define PASSWORD 				"Hello123"
#define SERVER_PORT 			8005
#define WIFI_WRITE_TIMEOUT 		1000
#define CONNECTION_TRIAL_MAX    10

#define PACKET_LENGHT			48
#define NTP_TIMESTAMP_DELTA 	2208988800ull
#define NTOHL(x) 				((((x) & 0xff) << 24) | \
                     	 	 	(((x) & 0xff00) << 8) | \
								(((x) & 0xff0000UL) >> 8) | \
								(((x) & 0xff000000UL) >> 24))
#define UTC_PLUS_2				7200 							//Addition of 2 hours for local time

/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* TCP client variables */
uint8_t remote_ip[] = {10, 27, 99, 63};
float tx_data[3];
uint8_t mac_addr[6];
uint8_t ip_addr[4];
int8_t socket = 0;
uint16_t datalen;
uint8_t conn_flag;

/* NTP server variables*/
char* host_name = "europe.pool.ntp.org";
uint8_t host_port = 123;
uint8_t host_ip_addr[4];
ntp_packet packet = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t ntp_datalen;
int8_t ntp_socket = 0;
extern rtc_time *rtc_data;

/* Private function prototypes -----------------------------------------------*/
void error_handling(const char *error_string, uint8_t error_code);
void wifi_init();
/* Private functions ---------------------------------------------------------*/

void get_time()
{
	/* Zero out the packet. All 48 bytes worth*/
	memset(&packet, 0, sizeof(ntp_packet));

	/* Set the first byte's bits to 00,011,011 for li = 0,vn = 3,and mode = 3. The rest will be left set to zero.
	 * Represents 27 in base 10 or 00011011 in base 2.
	 */
	*((char*)&packet + 0) = 0x1b;

	/*Checks if packet contains passed secs*/
	while (packet.txTm_s == 0) {
		/* Convert URL to IP */
		if (WIFI_GetHostAddress(host_name, host_ip_addr) == WIFI_STATUS_OK) {
			printf("> connecting to IP Address : %d.%d.%d.%d\n",
				host_ip_addr[0],
				host_ip_addr[1],
				host_ip_addr[2],
				host_ip_addr[3]);

			if (WIFI_OpenClientConnection(ntp_socket, WIFI_UDP_PROTOCOL, "UDP_CLIENT", host_ip_addr, host_port, 10) == WIFI_STATUS_OK) {
				printf("Client is connected, sending packet to server!\n");

				if (WIFI_SendData(ntp_socket, (char*)&packet, sizeof(ntp_packet), &ntp_datalen, WIFI_WRITE_TIMEOUT) == WIFI_STATUS_OK && ntp_datalen == PACKET_LENGHT) {
					printf("Packet has been sent to server, waiting for packet to arrive back!\n");

					if(WIFI_ReceiveData(ntp_socket, (char*)&packet, sizeof(ntp_packet), &ntp_datalen, WIFI_WRITE_TIMEOUT) == WIFI_STATUS_OK && ntp_datalen == PACKET_LENGHT) {
						printf("Packet has been received from server!\n");

						if (WIFI_CloseClientConnection(ntp_socket) == WIFI_STATUS_OK) {
							printf("Client socket has been closed!\n");

						} else {
							error_handling("> ERROR : CANNOT close client socket\n", WIFI_STATUS_ERROR);
						}
					} else {
						error_handling("> ERROR : CANNOT receive data from NTP server\n", WIFI_STATUS_ERROR);
						if (ntp_datalen < PACKET_LENGHT)
							printf("Received bytes are (%d) less than %d, data has been lost\n", ntp_datalen, PACKET_LENGHT);
					}
				} else {
					error_handling("> ERROR : CANNOT send data to NTP server\n", WIFI_STATUS_ERROR);
					if (ntp_datalen < PACKET_LENGHT)
						printf("Sent bytes are (%d) less than %d, data has been lost!\n", ntp_datalen, PACKET_LENGHT);
				}
			} else {
				error_handling("> ERROR : CANNOT connect to NTP server\n", WIFI_STATUS_ERROR);
			}
		} else {
			error_handling("> ERROR : CANNOT get NTP server IP address\n", WIFI_STATUS_ERROR);
		}
	}
	/* These two fields contain the time-stamp seconds as the packet left the NTP server.
	 * The number of seconds correspond to the seconds passed since 1900 NTOHL() converts the bit/byte order from the network's to host's "endianness".
	 */
	packet.txTm_s = NTOHL(packet.txTm_s);
	packet.txTm_f = NTOHL(packet.txTm_f);

   /* Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
    * Subtract 70 years worth of seconds from the seconds since 1900.
    * This leaves the seconds since the UNIX epoch of 1970.
    * (1900)------------------(1970)**************************************(Time Packet Left the Server) */
   time_t txTm = (time_t)(packet.txTm_s - NTP_TIMESTAMP_DELTA + UTC_PLUS_2);

   /*Print the time we got from the server,accounting for local timezone and conversion from UTC time. */
   printf("Time: %s\n",ctime((const time_t*)&txTm));
   rtc_data = gmtime((const time_t*)&txTm);
   rtc_init();
}
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

		get_time();

    	/*checking connection with WIFI AP */
		do {
			printf("> Trying to connect to Server: %d.%d.%d.%d:%d ...\n",
					remote_ip[0],
					remote_ip[1],
					remote_ip[2],
					remote_ip[3],
					SERVER_PORT);
	    	/*Creating socket and connecting to HQ server */
			while (WIFI_OpenClientConnection(socket, WIFI_TCP_PROTOCOL, "TCP_CLIENT", remote_ip, SERVER_PORT, 10) != WIFI_STATUS_OK);

		    	/*Loading sensor data into buffer */
				tx_data[0] = get_temperature();
				tx_data[1] = get_humidity();
				tx_data[2] = get_pressure();
				conn_flag = 0;

		    	/*Sending data when connected in every 10 seconds */
				while (WIFI_SendData(socket, (uint8_t*)tx_data, sizeof(tx_data), &datalen, WIFI_WRITE_TIMEOUT) == WIFI_STATUS_OK) {
					tx_data[0] = get_temperature();
					tx_data[1] = get_humidity();
					tx_data[2] = get_pressure();
					conn_flag = 1;
					HAL_Delay(10000);
				}

		    	/*When connection is lost conn_flag equals 1, else client could'not send data to server, timeout happens*/
				if (conn_flag == 1) {
					printf("> Disconnected from server!\n");
				} else {
					conn_flag = 0;
				}
			/*Closing socket when connection is lost or could'not connect */
			WIFI_CloseClientConnection(socket);
		} while (wifi_isconnected() == 1);	//do-while
		printf("> Disconnected from WIFI!\n");
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
