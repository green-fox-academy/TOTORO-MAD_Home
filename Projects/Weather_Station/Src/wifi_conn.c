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

/* Private define ------------------------------------------------------------*/
#define SSID     				"A66 Guest"
#define PASSWORD 				"Hello123"
#define SERVER_PORT 			8005
#define WIFI_WRITE_TIMEOUT 		1000
#define CONNECTION_TRIAL_MAX    10

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#define NTP_TIMESTAMP_DELTA 2208988800ull
#define ntohl(x) ((((x) & 0xff) << 24) | \
                     (((x) & 0xff00) << 8) | \
                     (((x) & 0xff0000UL) >> 8) | \
                     (((x) & 0xff000000UL) >> 24))

/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
uint8_t remote_ip[] = {10, 27, 99, 63};
float tx_data[3];
uint8_t mac_addr[6];
uint8_t ip_addr[4];
int8_t socket = 0;
uint16_t datalen;
uint8_t conn_flag;

char* host_name = "europe.pool.ntp.org";	// NTP server host-name.
uint8_t host_port = 123;					// NTP UDP port number.
uint8_t host_ip_addr[4];

// Create and zero out the packet. All 48 bytes worth.
ntp_packet packet = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* Private function prototypes -----------------------------------------------*/
void error_handling(const char *error_string, uint8_t error_code);
void wifi_init();
/* Private functions ---------------------------------------------------------*/

void get_time()
{
	   time_t   txTm = (time_t)(packet.txTm_s - NTP_TIMESTAMP_DELTA);

	   // Print the time we got from the server,accounting for local timezone and conversion from UTC time.
	   printf("Time: %s",ctime((const time_t*)&txTm));
	memset(&packet, 0, sizeof(ntp_packet));

	// Set the first byte's bits to 00,011,011 for li = 0,vn = 3,and mode = 3. The rest will be left set to zero.
	*((char*)&packet + 0) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.
	WIFI_GetHostAddress(host_name, host_ip_addr); // Convert URL to IP.
	printf("> connecting to IP Address : %d.%d.%d.%d\n",
			host_ip_addr[0],
			host_ip_addr[1],
			host_ip_addr[2],
			host_ip_addr[3]);
	while (WIFI_OpenClientConnection(socket, WIFI_UDP_PROTOCOL, "UDP_CLIENT", host_ip_addr, host_port, 10) != WIFI_STATUS_OK);

	WIFI_SendData(socket, (char*)&packet, sizeof(ntp_packet), &datalen, WIFI_WRITE_TIMEOUT);

	WIFI_ReceiveData(socket, (char*)&packet, sizeof(ntp_packet), &datalen, WIFI_WRITE_TIMEOUT);

	WIFI_CloseClientConnection(socket);

	// These two fields contain the time-stamp seconds as the packet left the NTP server.
   // The number of seconds correspond to the seconds passed since 1900.
   // ntohl() converts the bit/byte order from the network's to host's "endianness".
   packet.txTm_s = ntohl(packet.txTm_s); // Time-stamp seconds.
   packet.txTm_f = ntohl(packet.txTm_f); // Time-stamp fraction of a second.

   // Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
   // Subtract 70 years worth of seconds from the seconds since 1900.
   // This leaves the seconds since the UNIX epoch of 1970.
   // (1900)------------------(1970)**************************************(Time Packet Left the Server)
   txTm = (time_t)(packet.txTm_s - NTP_TIMESTAMP_DELTA);

   // Print the time we got from the server,accounting for local timezone and conversion from UTC time.
   printf("Time: %s",ctime((const time_t*)&txTm));
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

    	/*do-while connected to WIFI AP(checking connection by pinging own IP Address) */
		do {
			printf("> Trying to connect to Server: %d.%d.%d.%d:%d ...\n",
					remote_ip[0],
					remote_ip[1],
					remote_ip[2],
					remote_ip[3],
					SERVER_PORT);
	    	/*Creating socket*/
			while (WIFI_OpenClientConnection(socket, WIFI_TCP_PROTOCOL, "TCP_CLIENT", remote_ip, SERVER_PORT, 10) != WIFI_STATUS_OK);

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
			/*Closing socket when connection is lost or could'not connect */
			WIFI_CloseClientConnection(socket);
		} while (wifi_isconnected() == 1);	//do-while
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
