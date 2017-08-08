/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INIT_H
#define __INIT_H

/* Defines -------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void uart_init();
void time_base_init();
void pwm_init();
void delay(uint16_t delay_value);

#endif /* __INIT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
