/*****************************************
 *
 * 	spi.h
 *
 *  Polled SPI driver for AVR
 *
 ************************************************/
 
#include <stdint.h>
#include "spi_config.h" 


typedef enum spi_mode
{
	SPI_MODE0,
	SPI_MODE1,
	SPI_MODE2,
	SPI_MODE3
} SPI_MODE_t;


typedef enum spi_clk_div 
{
	SPI_CLKDIV_4,
	SPI_CLKDIV_16,
	SPI_CLKDIV_64,
	SPI_CLKDIV_128,
	SPI_CLKDIV_2,
	SPI_CLKDIV_8,
	SPI_CLKDIV_32
} SPI_CLKDIV_t;


#define SS_HIGH()		(SPI_PORT |= (1 << SS_BIT))
#define SS_LOW()		(SPI_PORT &= ~(1 << SS_BIT))

	
/* Prototypes */

void SPI_Init(SPI_MODE_t mode, SPI_CLKDIV_t clk_div);
uint8_t SPI_TxRx(uint8_t data);
void SPI_TxBuf(uint8_t *buf, uint16_t length);
void SPI_RxBuf(uint8_t *buf, uint16_t length);
