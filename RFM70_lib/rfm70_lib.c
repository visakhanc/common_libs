/**********************************************************
 RFM70 RF module library FOR AVR ATMEGA8 based hardware 
 **********************************************************
	Major Hardware Changes:
		1. On-chip SPI interface is used instead of bit-banged SPI
		2. IRQ interrupt from RFM70 is used - If AVR sleep mode is used, this will reduce power consumption when used in Rx mode. 
			The software can chose Interrupt or Polled mode. In case of polled mode, RFM70 IRQ pin can be left unconnected
			If interrupt mode is used, INT1 pin of AVR should be connected to RFM70 IRQ.
	
 **********************************************************
 HARDWARE CONNECTIONS	
 ***********************************************************
	An example of connection is shown below: 

	RFM70		ATmega8
	-------------------
	SCK			SCK(PB5)
	MISO		MISO(PB4)
	MOSI		MOSI(PB3)
	CSN			SS#(PB2)
	CE			PB1
	IRQ			INT1(PD3)
	VCC			VCC(3.3V)
	GND			GND
	
	4 pins of SPI interface (SCK,MISO,MOSI,CSN) and IRQ pin(if interrupt mode is used) should not be changed
	Only CE pin can be changed. This pin is configured in rfm70_config.h

TODO:

 * rfm70_receive_packet(): how to handle data from multiple pipes?
 * PRX recovers only if PTX is in interrupt mode: If PRX comes up later it does not receive(no LED blink)
		Happens if PTX is in polled mode (PRX can be int/polled). Seen during testing of ACK packet 
******************************************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "avr_spi.h"
#include "rfm70.h"

//#define LED_DEBUG

#define CSN_LOW()	SS_LOW()
#define CSN_HIGH()	SS_HIGH()

#define CE_OUT()	(CE_DDR |= (1 << CE_PIN))
#define CE_LOW()	(CE_PORT &= ~(1 << CE_PIN))
#define CE_HIGH()	(CE_PORT |= (1 << CE_PIN))

#define CE_PULSE() CE_HIGH(); \
	_delay_us(100); \
	CE_LOW();
	
	
/* Verify the configurations */
#if (CONFIG_RFM70_ADDR_LEN < 3) || (CONFIG_RFM70_ADDR_LEN > 5)
#error "Incorrect CONFIG_RFM70_ADDR_LEN: can be only 3, 4, or 5 - modify in rfm70_config.h"
#endif

#if !defined CONFIG_RFM70_AUTOACK_ENABLED
#error "CONFIG_RFM70_AUTOACK_ENABLED not defined - define in rfm70_config.h"
#endif

#if (CONFIG_RFM70_STATIC_PL_LENGTH > 32)
#error "CONFIG_RFM70_STATIC_PL_LENGTH cannot be >32 - modify in rfm70_config.h"
#endif

#if (!CONFIG_RFM70_AUTOACK_ENABLED) && (CONFIG_RFM70_DYNAMIC_PL_ENABLED)
#error "CONFIG_RFM70_AUTOACK_ENABLED should be defined to 1 for dynamic payload length"
#endif

#if !defined CONFIG_RFM70_DATA_RATE
#error "CONFIG_RFM70_DATA_RATE not defined - define in rfm70_config.h"
#endif

#if !defined CONFIG_RFM70_TX_PWR
#error "CONFIG_RFM70_TX_PWR not defined - define in rfm70_config.h"
#endif

#if !defined CONFIG_RFM70_RF_CHANNEL
#error "CONFIG_RFM70_RF_CHANNEL not defined - define in rfm70_config.h"
#endif

#if (CONFIG_RFM70_ACK_PL_ENABLED)
	#if ((!defined CONFIG_RFM70_ACK_PL_LENGTH) || (CONFIG_RFM70_ACK_PL_LENGTH == 0))
		#error "CONFIG_RFM70_ACK_PL_LENGTH not defined - define in rfm70_config.h"
	#endif
#endif

#if (CONFIG_RFM70_AUTOACK_ENABLED)
	#if ((!defined CONFIG_RFM70_TX_RETRANSMITS) || (CONFIG_RFM70_TX_RETRANSMITS == 0))
		#error "CONFIG_RFM70_TX_RETRANSMITS not properly defined - modify in rfm70_config.h"
	#endif
#endif


/* Auto Retransmission delay (us) */
#if (CONFIG_RFM70_DATA_RATE == RFM70_RATE_250KBPS)
	#if (CONFIG_RFM70_ACK_PL_ENABLED) /* ACK with payload */
		#if (CONFIG_RFM70_ACK_PL_LENGTH < 8)
		#define CONFIG_RFM70_RETRANS_DELAY 800
		#elif (CONFIG_RFM70_ACK_PL_LENGTH < 16)
		#define CONFIG_RFM70_RETRANS_DELAY 1100
		#elif (CONFIG_RFM70_ACK_PL_LENGTH < 24)
		#define CONFIG_RFM70_RETRANS_DELAY 1300
		#else
		#define CONFIG_RFM70_RETRANS_DELAY 1600
		#endif
	#else
		#define CONFIG_RFM70_RETRANS_DELAY 700  /* Empty ACK */
	#endif
#else /* 1Mbps or 2Mbps */
	#if (CONFIG_RFM70_ACK_PL_ENABLED) /* ACK with payload */
		#define CONFIG_RFM70_RETRANS_DELAY 700
	#else /* Empty ACK */
		#define CONFIG_RFM70_RETRANS_DELAY 300
	#endif
#endif


/* This is the pipe1 address to be configured for PRX device
	pipe2...pipe6 address will change only in LSB byte as C3, C4...C6 */
#define CONFIG_RFM70_PIPE1_ADDR 	{0xC2, 0xC2, 0xC2, 0xC2, 0xC2}


static volatile bool tx_done;
static volatile bool rx_ready;
static volatile bool max_retries;
static uint8_t pipe1_addr[] = CONFIG_RFM70_PIPE1_ADDR;

static void rfm70_irq(void);

#ifdef LED_DEBUG
#define RED_LED					PB0
#define RED_LED_DDR				DDRB
#define RED_LED_PORT			PORTB

#define RED_LED_OUT()			(RED_LED_DDR |= (1 << RED_LED))
#define RED_LED_OFF()			(RED_LED_PORT |= (1 << RED_LED))
#define RED_LED_ON()			(RED_LED_PORT &= ~(1 << RED_LED))
#define RED_LED_TOGGLE()		(RED_LED_PORT ^= (1 << RED_LED))
static void LED_Debug(uint8_t value);
#endif

//Bank1 register initialization value

//In the array the register value is the byte reversed!!!!!!!!!!!!!!!!!!!!!
const unsigned long Bank1_Reg0_13[] = {       //latest config txt
0xE2014B40,
0x00004BC0,
0x028CFCD0,
0x41390099,
0x1B8296d9,
0xA67F0224,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00127300,
0x46B48000,
};

const uint8_t Bank1_Reg14[]=
{
	0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF
};


static void mcu_init(void)
{
	CE_OUT();
	SPI_Init(SPI_MODE0, SPI_CLKDIV_4);  // SPI_CLKDIV_4
	
#if !CONFIG_RFM70_POLLED_MODE /* configure INT1 as LOW-level triggered */

#if defined  EICRA 
	EICRA &= ~((1 << ISC10)|(1 << ISC11));
	EIMSK |= (1 << INT1);
#else
	MCUCR &= ~((1 << ISC11)|(1 << ISC10));
	GICR |= (1 << INT1);
#endif
	
#endif
}

/* Reads from a register */
static uint8_t rfm70_read_reg(uint8_t reg)
{
	uint8_t value;
	
	CSN_LOW(); 
	SPI_TxRx(reg);	/* Transmit register to read */
	value = SPI_TxRx(0);  /* Then get the register value */
	CSN_HIGH();
	
	return value;
}

/* Writes to a register */
static uint8_t rfm70_write_reg(uint8_t reg, uint8_t val)
{
	uint8_t status;
	
	CSN_LOW();
	if(reg < WRITE_REG) {  /* write register with data */
		status = SPI_TxRx(WRITE_REG|reg);  
		SPI_TxRx(val);
	}
	else { /* command with (optional) data */
		status = SPI_TxRx(reg);
		if(val) {
			SPI_TxRx(val);
		}
	}
	CSN_HIGH();
	
	return status;
}


static inline uint8_t rfm70_nop(void)
{
	return rfm70_write_reg(NOP, 0);
}

static inline uint8_t rfm70_get_rx_pipe(void)
{
	return ((rfm70_write_reg(NOP,0) >> 1) & 0x7);
}

static inline uint8_t rfm70_get_address_width(void)
{
	return (rfm70_read_reg(SETUP_AW) + 2);
}

#if 0
/* Reads Multibyte register 
 * return value: MSByte - Rx payload pipe number. LSByte - read length
 */
uint16_t rfm70_read_multibyte_reg(uint8_t reg, uint8_t *buf)
{
	uint8_t ctr = 0, length = 0;
	
	switch(reg)
	{
		case RFM70_PIPE0:
		case RFM70_PIPE1:
		case RFM70_TX_PIPE:  /* Read pipe address */
			length = ctr = rfm70_get_address_width();
			CSN_LOW();
			SPI_TxRx(RX_ADDR_P0 + reg);
			break;
      
		case RFM70_RX_PLOAD:  /* Read Rx payload */
			if( (reg = rfm70_get_rx_pipe()) < 7) {
				length = ctr = rfm70_read_reg(RD_RX_PLOAD_W);
				CSN_LOW();
				SPI_TxRx(RD_RX_PLOAD);
			}
			break;

		default:
			break;
	}

	while(ctr--) {
		*buf++ = SPI_TxRx(0);
	}

	CSN_HIGH();

	return (((uint16_t) reg << 8) | length);
}

#endif

/* Writes to Multibyte register 
 * 
 */
static void rfm70_write_multibyte_reg(uint8_t reg, const uint8_t *buf, uint8_t length)
{
	switch(reg)
	{
		case RFM70_PIPE0:
		case RFM70_PIPE1:
		case RFM70_TX_PIPE:  /* Write pipe address */
			length = rfm70_get_address_width();
			CSN_LOW();
			SPI_TxRx(WRITE_REG + RX_ADDR_P0 + reg);
			break;
		case RFM70_TX_PLOAD:  /* Write Tx payload */
			CSN_LOW();
			SPI_TxRx(WR_TX_PLOAD);
			break;
		case RFM70_TX_PLOAD_NOACK: /* Write Tx payload with no ACK */
			CSN_LOW();
			SPI_TxRx(WR_NAC_TX_PLOAD);
			break;
		default:
			break;
	}

	while(length--) {
		SPI_TxRx(*buf++);
	}

	CSN_HIGH();
}


/* Set address of Pipes 
 * Tx address, Rx Pipe0,Pipe1 address are 5 bytes long
 * Rx pipe2-5 are given 1 byte address
 */
static void rfm70_set_address(rfm70_pipe_t pipe, const uint8_t *addr)
{
  switch(pipe)
  {
    case RFM70_TX_PIPE:
    case RFM70_PIPE0:
    case RFM70_PIPE1:
      rfm70_write_multibyte_reg((uint8_t) pipe, addr, 0);
      break;

    case RFM70_PIPE2:
    case RFM70_PIPE3:
    case RFM70_PIPE4:
    case RFM70_PIPE5:
      rfm70_write_reg(RX_ADDR_P0 + (uint8_t) pipe, *addr);
      break;

    default:
      break;
  }
}


/* Writes a buffer of data to location specified by reg; specifically used for writing to bank1 registers  */
static void rfm70_write_buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
	CSN_LOW();
	SPI_TxRx(WRITE_REG|reg);
	SPI_TxBuf(buf, len);
	CSN_HIGH();
}

/* Switches access between Bank1 and Bank0 
Parameter:
	 1:register bank1
	 0:register bank0
*/
static void rfm70_switch_bank(char bank)
{
	uint8_t status;

	status=rfm70_read_reg(STATUS);
	status=status&0x80;

	if( (status&&(bank==0))||((status==0)&&bank) )
	{
		rfm70_write_reg(LOCK_UNLOCK,0x53);
	}
}

   
/*
 *  Initialize RFM70 module
 *		Set address
 *		Enable pipes (with/without auto ack)
 * 		Set data rate, power, payload length and other things
 *		Set mode to Tx/Rx
 *
 *		Returns: 0 on success, 1 on error with SPI communication
 */  
uint8_t rfm70_init(rfm70_opmode_t mode, const uint8_t *address)
{
	uint8_t i, j, reg_val, ret = 0;
 	uint8_t WriteArr[12];

	/* low level initialization */
	mcu_init();
	
	_delay_ms(20); //delay more than 50ms?
	/*** configure bank 0 registers ***/
	
	rfm70_switch_bank(0);
	/* Set Address */
	rfm70_write_reg(SETUP_AW, (uint8_t)(CONFIG_RFM70_ADDR_LEN-2));  /* Address width */
	/* Set same address for Tx and Rx-Pipe0 (for auto ack) */
	rfm70_set_address(RFM70_TX_PIPE, address);  
	rfm70_set_address(RFM70_PIPE0, address);
	/* Set other pipe addresses */
	rfm70_set_address(RFM70_PIPE1, pipe1_addr);
	/* Open channels */
	reg_val = (1 << 0)|(1 << 1); /* open Pipe0, Pipe1 */
	rfm70_write_reg(EN_RXADDR, reg_val);
	if(CONFIG_RFM70_AUTOACK_ENABLED) {
		reg_val = (1 << 0)|(1 << 1);  /* Enable auto ack (only for Pipe 0) */
		rfm70_write_reg(EN_AA, reg_val);
	}
	/* Set payload length (only for Pipe 0)*/
	if(mode == RFM70_MODE_PRX) {
		rfm70_write_reg(RX_PW_P0, CONFIG_RFM70_STATIC_PL_LENGTH);
		rfm70_write_reg(RX_PW_P1, CONFIG_RFM70_STATIC_PL_LENGTH);
	}
	/* FEATURE reg */
	reg_val = 0;
	if(CONFIG_RFM70_DYNAMIC_PL_ENABLED) {
		reg_val = (1 << 2);
	}
	if(CONFIG_RFM70_ACK_PL_ENABLED) {
		reg_val |= (1 << 1);
	}
	if(!CONFIG_RFM70_AUTOACK_ENABLED) {
		reg_val |= (1 << 0);
	}
	if(0 == rfm70_read_reg(FEATURE)) {
		rfm70_write_reg(LOCK_UNLOCK,0x73);  /* Unlock FEATURE register */		
	}
	rfm70_write_reg(FEATURE, reg_val);
	if(CONFIG_RFM70_DYNAMIC_PL_ENABLED||CONFIG_RFM70_ACK_PL_ENABLED) { /* Enable dynamic payload length (for Pipe 0, Pipe 1 only) */
		reg_val = (1 << 0)|(1 << 1);
		rfm70_write_reg(DYNPD, reg_val);
	}
	/* Retransmit reg */
	reg_val = 0;
	if(CONFIG_RFM70_AUTOACK_ENABLED) {
		reg_val = (((CONFIG_RFM70_RETRANS_DELAY/250)-1) << 4)|(CONFIG_RFM70_TX_RETRANSMITS & 0xF);
	}
	rfm70_write_reg(SETUP_RETR, reg_val);
	/* RF setup reg */
	reg_val = rfm70_read_reg(RF_SETUP);
	reg_val &= ~(RF_DR_LOW|RF_DR_HIGH|RF_PWR1|RF_PWR0);
	if(CONFIG_RFM70_DATA_RATE == RFM70_RATE_250KBPS) {
		reg_val |= RF_DR_HIGH;
	} else if(CONFIG_RFM70_DATA_RATE == RFM70_RATE_2MBPS) {
		reg_val |=  RF_DR_LOW;
	}
	reg_val |= (CONFIG_RFM70_TX_PWR << 1);
	rfm70_write_reg(RF_SETUP, reg_val);
	/* RF Channel reg*/
	rfm70_write_reg(RF_CH, CONFIG_RFM70_RF_CHANNEL);	
	reg_val = rfm70_read_reg(RF_CH);
#ifdef LED_DEBUG
	LED_Debug(reg_val);
#endif
	if(CONFIG_RFM70_RF_CHANNEL != reg_val) {
		ret = 1;
	}
	
	/***** Write Bank1 registers *****/
	rfm70_switch_bank(1);
	for(i=0;i<=8;i++)//reverse
	{
		for(j=0;j<4;j++)
			WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(j) ) )&0xff;
		rfm70_write_buf(i,&(WriteArr[0]),4);
	}
	for(i=9;i<=13;i++)
	{
		for(j=0;j<4;j++)
			WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(3-j) ) )&0xff;
		rfm70_write_buf(i,&(WriteArr[0]),4);
	}
	for(j=0;j<11;j++)
	{
		WriteArr[j]=Bank1_Reg14[j];
	}
	rfm70_write_buf(14,&(WriteArr[0]),11);
	//toggle REG4<25,26>
	for(j=0;j<4;j++)
		WriteArr[j]=(Bank1_Reg0_13[4]>>(8*(j) ) )&0xff;
	WriteArr[0]=WriteArr[0]|0x06;
	rfm70_write_buf(4,&(WriteArr[0]),4);
	
	WriteArr[0]=WriteArr[0]&0xf9;
	rfm70_write_buf(4,&(WriteArr[0]),4);
	/* Switch back to bank 0 */
	rfm70_switch_bank(0);
	_delay_ms(1); 

	/* Config reg */
	reg_val = (CONFIG_EN_CRC|CONFIG_CRCO|CONFIG_PWR_UP);
	if(mode == RFM70_MODE_PRX) {	
		reg_val |= 1;
		rfm70_write_reg(FLUSH_RX,0);//flush Rx fifo
		CE_HIGH(); /* Set CE High for Rx mode */
	}
	rfm70_write_reg(CONFIG, reg_val);
	rfm70_write_reg(FLUSH_TX,0);//flush Tx fifo
	/* Clear interrupt flags */
	rfm70_write_reg(STATUS,STAT_TX_DS|STAT_MAX_RT|STAT_RX_DR); 
	
	_delay_ms(2); /* delay after power up */
	
	return ret;
}



/* switch to Rx mode */
void rfm70_rx_mode(void)
{
	uint8_t value;

	rfm70_write_reg(FLUSH_RX,0);//flush Rx
	value=rfm70_read_reg(STATUS);	// read register STATUS's value
	rfm70_write_reg(STATUS,value);// clear RX_DR or TX_DS or MAX_RT interrupt flag
	CE_LOW();
	value=rfm70_read_reg(CONFIG);	// read register CONFIG's value
	value=value|0x01;//set bit 0
  	rfm70_write_reg(CONFIG, value); // Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled..
	CE_HIGH();
	
	_delay_ms(2); // Power-up delay 1.5 ms
}



/* switch to Tx mode **/
void rfm70_tx_mode(void)
{
	uint8_t value;
	
	rfm70_write_reg(FLUSH_TX,0);//flush Tx
	CE_LOW();
	value=rfm70_read_reg(CONFIG);	// read register CONFIG's value
	value=value&0xfe;//clear bit 0
	value |= (1 << 1); // Set PWR_UP bit
  	rfm70_write_reg(CONFIG, value); // Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled.
	
	_delay_ms(2); // Power-up delay 1.5 ms
}


void rfm70_powerdown(void)
{
	uint8_t value;
	
	value=rfm70_read_reg(CONFIG);
	value &= ~(1 << 1); // Clear PWR_UP bit
	rfm70_write_reg(CONFIG, value); 
}


/* Transmit a packet 
 *	Returns :
 *		0 - If successfully transmitted
 *		1 - Not successful (ACK not received)
 *		2 - Cannot transmit (Tx FIFO full)
 */
uint8_t rfm70_transmit_packet(uint8_t *packet, uint8_t length)
{
	uint8_t 	ret;

	if(rfm70_read_reg(FIFO_STATUS) & TX_FIFO_FULL) {
		return 2;
	}
	
	rfm70_write_multibyte_reg(RFM70_TX_PLOAD, packet, length);
	CE_PULSE();
	do {
#if CONFIG_RFM70_POLLED_MODE
		rfm70_irq();
#endif
		if(tx_done == true) {
			tx_done = false;
			ret = 0;
			break;
		}
		if(max_retries == true) {
			max_retries = false;
			ret = 1;
			break;
		}
	} while (1);
	
	return ret;
}


/* 
 *  Receive a packet if available
 *	Returns:  pipe number
 *		
 */
uint8_t rfm70_receive_packet(uint8_t *buf, uint8_t *length)
{
	uint8_t rx_pipe = 0;
	
#if CONFIG_RFM70_POLLED_MODE
	rfm70_irq();
#endif
	*length = 0;
	if(rx_ready == true) {
		rx_ready = false;
		rx_pipe = rfm70_get_rx_pipe();
		do {
			*length = rfm70_read_reg(RD_RX_PLOAD_W);
			CSN_LOW();
			SPI_TxRx(RD_RX_PLOAD);
			SPI_RxBuf(buf, *length);
			CSN_HIGH();
		} while (!(rfm70_read_reg(FIFO_STATUS) & RX_EMPTY));
	}
	
	return rx_pipe;
}



/* 
 * 	Write ACK payload for a pipe
 *		
 */
void rfm70_set_ack_payload(uint8_t pipe, uint8_t *buf, uint8_t length)
{
	CSN_LOW();
	SPI_TxRx(WR_ACK_PLOAD|pipe);
	SPI_TxBuf(buf, length);
	CSN_HIGH();
}


/*
 *	Flush TX FIFO
 */
 void rfm70_flush_txfifo(void)
 {
	rfm70_write_reg(FLUSH_TX,0);
 }
 
/* Function to handle RFM70 Interupt; also used to poll the status continuously */
static void rfm70_irq(void)
{
	uint8_t status;
	
	//status = rfm70_write_reg(STATUS, (STAT_MAX_RT|STAT_TX_DS|STAT_RX_DR)); /* Get and clear interrupt flags */
	status = rfm70_read_reg(STATUS);
	if(status) {
		rfm70_write_reg(STATUS, status);
		if(status & STAT_RX_DR)
		{
			/* received data */
			rx_ready = true;
		}
		
		if(status & STAT_TX_DS)
		{
			/* transmit done */
			tx_done = true;
		}
		
		if(status & STAT_MAX_RT)
		{
			/* Maximum retries exceeded */
			max_retries = true;
			rfm70_write_reg(FLUSH_TX,0);//flush Rx fifo
		}
	}
}

/**************************************************
Function: ISR for RFM70 IRQ
Description:
	Reads status register and sets appropriate flags
Parameter:
	None
Return:
	None
**************************************************/
#if !CONFIG_RFM70_POLLED_MODE
ISR(INT1_vect)
{	
	//PORTB &= ~(1 << 0); // LED OFF
	rfm70_irq();
}
#endif


#ifdef LED_DEBUG
static void LED_Debug(uint8_t value)
{
	volatile uint8_t i;
	for(i = 0; i < 8; i++) {
		RED_LED_ON();
		_delay_ms(50);
		RED_LED_OFF();
		_delay_ms(50);
		if(value & (1 << i)) {
			RED_LED_ON();
			_delay_ms(50);
			RED_LED_OFF();
		}
		_delay_ms(700);
	}
}
#endif