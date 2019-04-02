/*************************************************
 *
 *	spi.c
 *
 *  Polled SPI driver for AVR
 *
 ************************************************/

#include <avr/io.h> 
#include "avr_spi.h"

/* Initialize the SPI interface in Master mode with given SPI mode and clock rate division */
void SPI_Init(SPI_MODE_t mode, SPI_CLKDIV_t clk_div)
{
	/* Set MOSI, SCK, SS as ouput */
	SPI_DDR |= (1 << MOSI_BIT)|(1 << SCK_BIT)|(1 << SS_BIT);
	
	/* Slave Select pin is initially HIGH */
	SS_HIGH();
	
	/* Initialize SPI */
	SPCR = (1 << 6)|(1 << 4)|(mode << 2);  /* Enable SPI, master mode, MSB first, given mode */
	SPCR |= (clk_div & 0x3); /* Clock rate division */
	if(clk_div & (1 << 2))
	{
		SPSR = 0x1;  /* SPI2X bit */
	}
	
}


uint8_t SPI_TxRx(uint8_t data)
{
	/* Start transmission */
	SPDR = data;
	
	/* Wait for transmission complete */
	while(!(SPSR & (1 << SPIF)) )
		;
	
	return SPDR;
}


void SPI_TxBuf(uint8_t *buf, uint16_t length)
{
	while(length--)
	{
		SPI_TxRx(*buf++);
	}
}

void SPI_RxBuf(uint8_t *buf, uint16_t length)
{
	while(length--)
	{
		*buf++ = SPI_TxRx(0x00);
	}	
}

