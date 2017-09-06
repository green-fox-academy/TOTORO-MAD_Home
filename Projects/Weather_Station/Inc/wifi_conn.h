/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WIFI_CONN_H
#define __WIFI_CONN_H

/* Includes ------------------------------------------------------------------*/
#include "wifi.h"
#include "stm32l475e_iot01.h"
#include "stdio.h"
#include "sensors.h"
#include "iic.h"


/* Defines -------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void send_sensor_data();
#endif /* __WIFI_CONN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
