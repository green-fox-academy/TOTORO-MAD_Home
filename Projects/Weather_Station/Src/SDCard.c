/* Includes ------------------------------------------------------------------*/
#include "sd_card.h"
#include "stdint.h"
#include "stm32l4xx_hal.h"

/* IOT includes component */
#include "stm32l475e_iot01.h"
#include <stdio.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SPIx_NSS_PIN                     GPIO_PIN_2
#define SPIx_NSS_GPIO_PORT               GPIOA
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint64_t _sdSize = 0;
/** GO_IDLE_STATE - init card in spi mode if CS low */
uint8_t const CMD0 = 0X00;
/** SEND_IF_COND - verify SD Memory Card interface operating condition.*/
uint8_t const CMD8 = 0X08;
/** SEND_CSD - read the Card Specific Data (CSD register) */
uint8_t const CMD9 = 0X09;
/** SEND_CID - read the card identification information (CID register) */
uint8_t const CMD10 = 0X0A;
/** SEND_STATUS - read the card status register */
uint8_t const CMD13 = 0X0D;
/** READ_BLOCK - read a single data block from the card */
uint8_t const CMD17 = 0X11;
/** WRITE_BLOCK - write a single data block to the card */
uint8_t const CMD24 = 0X18;
/** WRITE_MULTIPLE_BLOCK - write blocks of data until a STOP_TRANSMISSION */
uint8_t const CMD25 = 0X19;
/** ERASE_WR_BLK_START - sets the address of the first block to be erased */
uint8_t const CMD32 = 0X20;
/** ERASE_WR_BLK_END - sets the address of the last block of the continuous
 range to be erased*/
uint8_t const CMD33 = 0X21;
/** ERASE - erase all previously selected blocks */
uint8_t const CMD38 = 0X26;
/** APP_CMD - escape for application specific command */
uint8_t const CMD55 = 0X37;
/** READ_OCR - read the OCR register of a card */
uint8_t const CMD58 = 0X3A;
/** SET_WR_BLK_ERASE_COUNT - Set the number of write blocks to be
 pre-erased before writing */
uint8_t const ACMD23 = 0X17;
/** SD_SEND_OP_COMD - Sends host capacity support information and
 activates the card's initialization process */
uint8_t const ACMD41 = 0X29;
//------------------------------------------------------------------------------
/** status for card in the ready state */
uint8_t const R1_READY_STATE = 0X00;
/** status for card in the idle state */
uint8_t const R1_IDLE_STATE = 0X01;
/** status bit for illegal command */
uint8_t const R1_ILLEGAL_COMMAND = 0X04;
/** start data token for read or write single block*/
uint8_t const DATA_START_BLOCK = 0XFE;
/** stop token for write multiple blocks*/
uint8_t const STOP_TRAN_TOKEN = 0XFD;
/** start data token for write multiple blocks*/
uint8_t const WRITE_MULTIPLE_TOKEN = 0XFC;
/** mask for data response tokens after a write block operation */
uint8_t const DATA_RES_MASK = 0X1F;
/** write data accepted token */
uint8_t const DATA_RES_ACCEPTED = 0X05;
//------------------------------------------------------------------------------
typedef struct CID {
	// byte 0
	uint8_t mid;  // Manufacturer ID
	// byte 1-2
	char oid[2];  // OEM/Application ID
	// byte 3-7
	char pnm[5];  // Product name
	// byte 8
	unsigned prv_m :4;  // Product revision n.m
	unsigned prv_n :4;
	// byte 9-12
	uint32_t psn;  // Product serial number
	// byte 13
	unsigned mdt_year_high :4;  // Manufacturing date
	unsigned reserved :4;
	// byte 14
	unsigned mdt_month :4;
	unsigned mdt_year_low :4;
	// byte 15
	unsigned always1 :1;
	unsigned crc :7;
} cid_t;
//------------------------------------------------------------------------------
// CSD for version 1.00 cards
typedef struct CSDV1 {
	// byte 0
	unsigned reserved1 :6;
	unsigned csd_ver :2;
	// byte 1
	uint8_t taac;
	// byte 2
	uint8_t nsac;
	// byte 3
	uint8_t tran_speed;
	// byte 4
	uint8_t ccc_high;
	// byte 5
	unsigned read_bl_len :4;
	unsigned ccc_low :4;
	// byte 6
	unsigned c_size_high :2;
	unsigned reserved2 :2;
	unsigned dsr_imp :1;
	unsigned read_blk_misalign :1;
	unsigned write_blk_misalign :1;
	unsigned read_bl_partial :1;
	// byte 7
	uint8_t c_size_mid;
	// byte 8
	unsigned vdd_r_curr_max :3;
	unsigned vdd_r_curr_min :3;
	unsigned c_size_low :2;
	// byte 9
	unsigned c_size_mult_high :2;
	unsigned vdd_w_cur_max :3;
	unsigned vdd_w_curr_min :3;
	// byte 10
	unsigned sector_size_high :6;
	unsigned erase_blk_en :1;
	unsigned c_size_mult_low :1;
	// byte 11
	unsigned wp_grp_size :7;
	unsigned sector_size_low :1;
	// byte 12
	unsigned write_bl_len_high :2;
	unsigned r2w_factor :3;
	unsigned reserved3 :2;
	unsigned wp_grp_enable :1;
	// byte 13
	unsigned reserved4 :5;
	unsigned write_partial :1;
	unsigned write_bl_len_low :2;
	// byte 14
	unsigned reserved5 :2;
	unsigned file_format :2;
	unsigned tmp_write_protect :1;
	unsigned perm_write_protect :1;
	unsigned copy :1;
	unsigned file_format_grp :1;
	// byte 15
	unsigned always1 :1;
	unsigned crc :7;
} csd1_t;
//------------------------------------------------------------------------------
// CSD for version 2.00 cards
typedef struct CSDV2 {
	// byte 0
	unsigned reserved1 :6;
	unsigned csd_ver :2;
	// byte 1
	uint8_t taac;
	// byte 2
	uint8_t nsac;
	// byte 3
	uint8_t tran_speed;
	// byte 4
	uint8_t ccc_high;
	// byte 5
	unsigned read_bl_len :4;
	unsigned ccc_low :4;
	// byte 6
	unsigned reserved2 :4;
	unsigned dsr_imp :1;
	unsigned read_blk_misalign :1;
	unsigned write_blk_misalign :1;
	unsigned read_bl_partial :1;
	// byte 7
	unsigned reserved3 :2;
	unsigned c_size_high :6;
	// byte 8
	uint8_t c_size_mid;
	// byte 9
	uint8_t c_size_low;
	// byte 10
	unsigned sector_size_high :6;
	unsigned erase_blk_en :1;
	unsigned reserved4 :1;
	// byte 11
	unsigned wp_grp_size :7;
	unsigned sector_size_low :1;
	// byte 12
	unsigned write_bl_len_high :2;
	unsigned r2w_factor :3;
	unsigned reserved5 :2;
	unsigned wp_grp_enable :1;
	// byte 13
	unsigned reserved6 :5;
	unsigned write_partial :1;
	unsigned write_bl_len_low :2;
	// byte 14
	unsigned reserved7 :2;
	unsigned file_format :2;
	unsigned tmp_write_protect :1;
	unsigned perm_write_protect :1;
	unsigned copy :1;
	unsigned file_format_grp :1;
	// byte 15
	unsigned always1 :1;
	unsigned crc :7;
} csd2_t;
//------------------------------------------------------------------------------
// union of old and new style CSD register
union csd_t {
	csd1_t v1;
	csd2_t v2;
};
extern SPI_HandleTypeDef spihandle;
uint8_t pdata_val[1] = { 0 };
/* Private function prototypes -----------------------------------------------*/
void deselect_card();
void select_card();
uint8_t card_command(uint8_t command, uint32_t arg);
void wait_until_ready();
HAL_StatusTypeDef spi_receive(uint8_t* pdata, uint16_t size);
HAL_StatusTypeDef lspi_wait_flag_state_until_timeout(SPI_HandleTypeDef *hspi, uint32_t Flag, uint32_t State, uint32_t Timeout, uint32_t Tickstart);
uint8_t card_a_cmd(uint8_t cmd, uint32_t arg);

/* Private functions ---------------------------------------------------------*/

uint8_t sdcard_init(){
	spihandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	HAL_SPI_Init(&spihandle);
	deselect_card();
	//We must supply at least 74 clocks with CS high
	uint8_t buffer[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
	HAL_SPI_Transmit(&spihandle, buffer, 4, 100);
	HAL_SPI_Transmit(&spihandle, buffer, 4, 100);
	HAL_SPI_Transmit(&spihandle, buffer, 4, 100);
	HAL_Delay(5);
	select_card();
	uint8_t status;
	while ((status = card_command(CMD0, 0)) != R1_IDLE_STATE) {

	}
	// check SD version
		if ((card_command(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND)) {
			deselect_card();
			return 0; //Unsupported
		} else {
			// only need last byte of r7 response
			HAL_SPI_Receive(&spihandle, buffer, 4, 100);
			if (buffer[3] != 0XAA) {
				return 1; //failed check
			}
		}
		// initialize card and send host supports SDHC
		while ((status = card_a_cmd(ACMD41, 0X40000000)) != R1_READY_STATE) {

		}
		// if SD2 read OCR register to check for SDHC card
		if (card_command(CMD58, 0)) {
			deselect_card();
			return 0;
		}
		//discard OCR reg
		spi_receive(buffer, 4);
		deselect_card();
		spihandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; //speed back up
		HAL_SPI_Init(&spihandle); //apply the speed change
		return 1;
}

void select_card()
{
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT, SPIx_NSS_PIN, GPIO_PIN_RESET);
}

void deselect_card() {
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT, SPIx_NSS_PIN, GPIO_PIN_SET);
}

uint8_t card_command(uint8_t command, uint32_t arg) {
	uint8_t res = 0xFF;
	HAL_SPI_Transmit(&spihandle, &res, 1, 100);
	HAL_SPI_Transmit(&spihandle, &res, 1, 100);
	HAL_SPI_Transmit(&spihandle, &res, 1, 100);
	HAL_SPI_Transmit(&spihandle, &res, 1, 100);
	HAL_SPI_Transmit(&spihandle, &res, 1, 100);
	HAL_SPI_Transmit(&spihandle, &res, 1, 100);
	HAL_SPI_Transmit(&spihandle, &res, 1, 100);
	select_card();
	wait_until_ready(); //wait for card to no longer be busy
	uint8_t command_sequence[] = { (uint8_t)(command | 0x40), (uint8_t)(
			arg >> 24), (uint8_t)(arg >> 16), (uint8_t)(arg >> 8), (uint8_t)(
			arg & 0xFF), 0xFF };
	if (command == CMD0)
		command_sequence[5] = 0x95;
	else if (command == CMD8)
		command_sequence[5] = 0x87;

	HAL_SPI_Transmit(&spihandle, command_sequence, 6, 100);
	//Data sent, now await Response
	uint8_t count = 20;
	HAL_Delay(3000);
	while ((res & 0x80) && count) {
		spi_receive(&res, 1);
		printf("res%d count%d\n", res, count);
		count--;
	}
	return res;
}

void wait_until_ready() {
	uint8_t ans[1] = { 0 };
	if (pdata_val[0] == 25) {
		while (ans[0] != pdata_val[0]) {
			printf("ans%d spi rec%d\n", ans[0], spi_receive(ans, 1));
			spi_receive(ans, 1);
		}
	} else if (pdata_val[0] == 255) {
		while (ans[0] != pdata_val[0]) {
			printf("ans%d spi rec%d\n", ans[0], spi_receive(ans, 1));
			spi_receive(ans, 1);
		}
	}
}

HAL_StatusTypeDef spi_receive(uint8_t* pdata, uint16_t size) {

	HAL_StatusTypeDef errorcode = HAL_OK;

	/* Process Locked */
	__HAL_LOCK(&spihandle);/* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
	if (spihandle.State == HAL_SPI_STATE_READY) {
		spihandle.State = HAL_SPI_STATE_BUSY_TX_RX;
	}
	/* Set the transaction information */
	spihandle.ErrorCode = HAL_SPI_ERROR_NONE;
	spihandle.pRxBuffPtr = pdata;
	spihandle.RxXferCount = size;
	spihandle.RxXferSize = size;
	spihandle.pTxBuffPtr = pdata;
	spihandle.TxXferCount = size;
	spihandle.TxXferSize = size;

	/*Init field not used in handle to zero */
	spihandle.RxISR = NULL;
	spihandle.TxISR = NULL;
	/* Check if the SPI is already enabled */
	if ((spihandle.Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
		/* Enable SPI peripheral */
		__HAL_SPI_ENABLE(&spihandle);
	}
	/* Transmit and Receive data in 8 Bit mode */
	while ((spihandle.RxXferCount > 0U)) {
		*(__IO uint8_t *) &spihandle.Instance->DR = 0xFF; //send data
		while (!(__HAL_SPI_GET_FLAG(&spihandle, SPI_FLAG_TXE)))
			;
		while (!(__HAL_SPI_GET_FLAG(&spihandle, SPI_FLAG_RXNE)))
			;
		(*(uint8_t *) pdata++) = spihandle.Instance->DR;
		spihandle.RxXferCount--;
		HAL_Delay(1000);
	}
	pdata_val[0] = pdata;
	if (lspi_wait_flag_state_until_timeout(&spihandle, SPI_FLAG_BSY, RESET, 100, HAL_GetTick()) != HAL_OK) {
		spihandle.ErrorCode |= HAL_SPI_ERROR_FLAG;

		errorcode = HAL_TIMEOUT;
	}
	spihandle.State = HAL_SPI_STATE_READY;
	__HAL_UNLOCK(&spihandle);
	return errorcode;
}

HAL_StatusTypeDef lspi_wait_flag_state_until_timeout(SPI_HandleTypeDef *hspi, uint32_t Flag, uint32_t State, uint32_t Timeout, uint32_t Tickstart) {
	while ((hspi->Instance->SR & Flag) != State) {
		if (Timeout != HAL_MAX_DELAY) {
			if ((Timeout == 0U) || ((HAL_GetTick() - Tickstart) >= Timeout)) {
				/* Disable the SPI and reset the CRC: the CRC value should be cleared
				 on both master and slave sides in order to resynchronize the master
				 and slave for their respective CRC calculation */

				/* Disable TXE, RXNE and ERR interrupts for the interrupt process */
				__HAL_SPI_DISABLE_IT(hspi,
						(SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

				if ((hspi->Init.Mode == SPI_MODE_MASTER)
						&& ((hspi->Init.Direction == SPI_DIRECTION_1LINE)
								|| (hspi->Init.Direction
										== SPI_DIRECTION_2LINES_RXONLY))) {
					/* Disable SPI peripheral */
					__HAL_SPI_DISABLE(hspi);
				}

				/* Reset CRC Calculation */
				if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE) {
					SPI_RESET_CRC(hspi);
				}

				hspi->State = HAL_SPI_STATE_READY;

				/* Process Unlocked */
				__HAL_UNLOCK(hspi);

				return HAL_TIMEOUT;
			}
		}
	}

	return HAL_OK;
}

uint8_t card_a_cmd(uint8_t cmd, uint32_t arg)
{
	card_command(CMD55, 0);
	return card_command(cmd, arg);
}
