#ifndef _SD_CARD_H_
#define _SD_CARD_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
/* Defines -------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t sdcard_init();
uint32_t get_size();
uint8_t write_block(uint32_t blockaddr, uint8_t* buffer);
uint8_t read_block(uint32_t blockaddr, uint8_t* buffer);
#endif /* _SD_CARD_H_ */
