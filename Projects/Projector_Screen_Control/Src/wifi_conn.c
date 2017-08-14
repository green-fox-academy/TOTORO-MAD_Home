/* Includes ------------------------------------------------------------------*/
#include "wifi_conn.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SSID     				"A66 Guest"
#define PASSWORD 				"Hello123"
#define SERVER_PORT 			8003
#define WIFI_WRITE_TIMEOUT 		10000
#define WIFI_READ_TIMEOUT  		10000
#define CONNECTION_TRIAL_MAX    10
/* Private macro -------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
uint8_t rx_data;
uint8_t mac_addr[6];
uint8_t ip_addr[4];

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
