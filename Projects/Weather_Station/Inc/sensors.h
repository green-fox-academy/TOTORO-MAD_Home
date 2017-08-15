/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SENSORS_H_
#define _SENSORS_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"

/* Defines -------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void sensor_inits();
float get_temperature();
float get_humidity();
float get_pressure();

#endif /* _SENSORS_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
