/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PS_CONTROL_H
#define __PS_CONTROL_H
/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
/* Defines -------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void send_signal(const uint16_t *array);
void make_signal(const uint8_t *array);
void set_ac(uint8_t command);

#endif /* __PS_CONTROL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
