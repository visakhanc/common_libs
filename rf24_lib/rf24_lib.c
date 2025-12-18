/**
 *	@file 	rf24_lib.c
 *	@brief	nRF24L01/RFM7x 2.4GHz RF Module library for AVR
 *
 *			**HARDWARE CONNECTION**
 *
 *			An example connection is shown below:
 *
 *
 *			| nRF24L01/RFM7x	| ATmega8	|  Notes
 *			|-------------------|-----------|---------
 *			| SCK				| SCK(PB5)	|
 *			| MISO				| MISO(PB4)	|
 *			| MOSI				| MOSI(PB3)	|
 *			| CSN				| SS#(PB2)	|  Changeable in spi_config.h
 *			| CE				| PB1		|  Changeable in rf24_config.h
 *			| IRQ				| INT1(PD3)	|  This driver uses INT1 AVR interrupt
 *			| VCC				| VCC(3.3V) |
 *			| GND				| GND		|
 *
 *	@note	IRQ interrupt from RF module is used - If AVR sleep mode is used, this will reduce power consumption when RF module is used in Rx mode.
 * 			The software can chose Interrupt or Polled mode. In case of polled mode, RF module IRQ pin can be left unconnected
 * 			if interrupt mode is used, IRQ pin should be connected to INT1 of AVR.
 *
 *
 *	@todo	Handle complexities of Multiceiver configuration
 *
 *
 *	@author	Visakhan C
 *	@date	2019/9/24
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "avr_spi.h"

#include "rf24_reg.h"
#include "rf24.h"

/* rf24_config.h is project specific specific and should be present in the project directory */
#include "rf24_config.h"

//#define LED_DEBUG

#define CSN_LOW()	SS_LOW()
#define CSN_HIGH()	SS_HIGH()

#define CE_OUT()	(CE_DDR |= (1 << CE_PIN))
#define CE_LOW()	(CE_PORT &= ~(1 << CE_PIN))
#define CE_HIGH()	(CE_PORT |= (1 << CE_PIN))

#define CE_PULSE() CE_HIGH(); \
	_delay_us(20); \
	CE_LOW();

/* Verify the configurations */
#if (CONFIG_RF24_ADDR_LEN < 3) || (CONFIG_RF24_ADDR_LEN > 5)
#error "Incorrect CONFIG_RF24_ADDR_LEN: can be only 3, 4, or 5 - modify in rf24_config.h"
#endif

#if !defined CONFIG_RF24_AUTOACK_ENABLED
#error "CONFIG_RF24_AUTOACK_ENABLED not defined - define in rf24_config.h"
#endif

#if (CONFIG_RF24_STATIC_PL_LENGTH > 32)
#error "CONFIG_RF24_STATIC_PL_LENGTH cannot be >32 - modify in rf24_config.h"
#endif

#if (!CONFIG_RF24_DYNAMIC_PL_ENABLED) && (CONFIG_RF24_ACK_PL_LENGTH)
#error "CONFIG_RF24_DYNAMIC_PL_ENABLED should be defined to 1 for ACK payload"
#endif

#if !defined CONFIG_RF24_DATA_RATE
#error "CONFIG_RF24_DATA_RATE not defined - define in rf24_config.h"
#endif

#if !defined CONFIG_RF24_TX_PWR
#error "CONFIG_RF24_TX_PWR not defined - define in rf24_config.h"
#endif

#if !defined CONFIG_RF24_RF_CHANNEL
#error "CONFIG_RF24_RF_CHANNEL not defined - define in rf24_config.h"
#endif

#if (CONFIG_RF24_DYNAMIC_PL_ENABLED) && (!defined CONFIG_RF24_ACK_PL_LENGTH)
#error "CONFIG_RF24_ACK_PL_LENGTH not defined - define in rf24_config.h"
#endif


#if (CONFIG_RF24_AUTOACK_ENABLED) && (!defined CONFIG_RF24_TX_RETRANSMITS)
#error "CONFIG_RF24_TX_RETRANSMITS not defined - modify in rf24_config.h"
#endif

/* Auto Retransmission delay (us) */
#if (CONFIG_RF24_DATA_RATE == RF24_RATE_250KBPS)
	#if (CONFIG_RF24_ACK_PL_ENABLED) /* ACK with payload */
		#if (CONFIG_RF24_ACK_PL_LENGTH < 8)
		#define CONFIG_RF24_RETRANS_DELAY 800
		#elif (CONFIG_RF24_ACK_PL_LENGTH < 16)
		#define CONFIG_RF24_RETRANS_DELAY 1100
		#elif (CONFIG_RF24_ACK_PL_LENGTH < 24)
		#define CONFIG_RF24_RETRANS_DELAY 1300
		#else
		#define CONFIG_RF24_RETRANS_DELAY 1600
		#endif
	#else
		#define CONFIG_RF24_RETRANS_DELAY 700  /* Empty ACK */
	#endif
#else /* 1Mbps or 2Mbps */
	#if (CONFIG_RF24_ACK_PL_ENABLED) /* ACK with payload */
		#define CONFIG_RF24_RETRANS_DELAY 700
	#else /* Empty ACK */
		#define CONFIG_RF24_RETRANS_DELAY 300
	#endif
#endif

/*  This is the Pipe1 address to be configured for PRX device
 TODO: Address assignment is rudimentary now. Needs work on assigning dynamic address to all 6 pipes at runtime  */
#define CONFIG_RF24_PIPE1_ADDR 	{0xC2, 0xC2, 0xC2, 0xC2, 0xC2}

static volatile bool tx_done;
static volatile bool rx_ready;
static volatile bool max_retries;
static uint8_t pipe1_addr[] = CONFIG_RF24_PIPE1_ADDR;

static void rf24_irq(void);

#ifdef LED_DEBUG
#define DBG_LED					PD6
#define DBG_LED_DDR				DDRD
#define DBG_LED_PORT			PORTD

#define DBG_LED_OUT()			(DBG_LED_DDR |= (1 << DBG_LED))
#define DBG_LED_OFF()			(DBG_LED_PORT |= (1 << DBG_LED))
#define DBG_LED_ON()			(DBG_LED_PORT &= ~(1 << DBG_LED))
#define DBG_LED_TOGGLE()		(DBG_LED_PORT ^= (1 << DBG_LED))
static void LED_Debug(uint8_t value);
#endif

#if RFM7x_INIT
/* Bank1 register initialization value for RFM7x
 In the array the register value is the byte reversed!!!!!!!!!!!!!!!!!!!!!
 */
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
#endif

static void mcu_init(void) {
    CE_OUT();
    SPI_Init(SPI_MODE0, SPI_CLKDIV_4);  // SPI_CLKDIV_4

#if !CONFIG_RF24_POLLED_MODE /* configure INT1 as LOW-level triggered */

#if defined  EICRA 
    EICRA &= ~((1 << ISC10)|(1 << ISC11));
    EIMSK |= (1 << INT1);
#else
    MCUCR &= ~((1 << ISC11) | (1 << ISC10));
    GICR |= (1 << INT1);
#endif

#endif
}

/* Reads from a register */
static uint8_t rf24_read_reg(uint8_t reg) {
    uint8_t value;

    CSN_LOW();
    SPI_TxRx(reg); /* Transmit register to read */
    value = SPI_TxRx(0); /* Then get the register value */
    CSN_HIGH();

    return value;
}

/* Writes to a register */
static uint8_t rf24_write_reg(uint8_t reg, uint8_t val) {
    uint8_t status;

    CSN_LOW();
    if (reg < WRITE_REG) { /* write register with data */
        status = SPI_TxRx(WRITE_REG | reg);
        SPI_TxRx(val);
    } else { /* command with (optional) data */
        status = SPI_TxRx(reg);
        if (val) {
            SPI_TxRx(val);
        }
    }
    CSN_HIGH();

    return status;
}

/**
 * \brief Writes a buffer of data to the location specified in the command
 * \param command - specifies the multibyte register to be written
 * \param buf - data to be written
 * \param len - size in bytes of data
 */
static void rf24_write_buf(uint8_t command, const uint8_t *buf, uint8_t len) {
    CSN_LOW();
    SPI_TxRx(command);
    SPI_TxBuf(buf, len);
    CSN_HIGH();
}

static inline uint8_t rf24_nop(void) {
    return rf24_write_reg(NOP, 0);
}

static inline uint8_t rf24_get_rx_pipe(void) {
    return ((rf24_write_reg(NOP, 0) >> 1) & 0x7);
}

static inline uint8_t rf24_get_address_width(void) {
    return (rf24_read_reg(SETUP_AW) + 2);
}

#if RFM7x_INIT

/* Writes a array of data to location specified by reg; specifically used for writing to bank1 registers  */
static void rf24_write_arr(uint8_t reg, uint8_t *buf, uint8_t len)
{
    CSN_LOW();
    SPI_TxRx(WRITE_REG|reg);
    SPI_TxBuf(buf, len);
    CSN_HIGH();
}

/* -- For RFM7x modules only --
 * Switches access between Bank1 and Bank0
 Parameter:
 1:register bank1
 0:register bank0
 */
static void rfm7x_switch_bank(char bank)
{
    uint8_t status;

    status=rf24_read_reg(STATUS);
    status=status&0x80;

    if( (status&&(bank==0))||((status==0)&&bank) )
    {
        rf24_write_reg(ACTIVATE, 0x53);
    }
}
#endif

/**
 * \brief	This function configures address of the specified pipe(RX_ADDR_Pn) or the transmit address(TX_ADDR).
 * \details For Pipe0, Pipe1 and Tx address, multibyte address (3 - 5 bytes) is given in addr buffer.
 *   		For other pipes (Pipe2 - Pipe5), single byte is given (*addr), which is used to replace LSByte of Pipe1 address
 *   		to form the pipe's unique address.
 * \param 	pipe - Pipe number (See \ref rf24_pipe_t)
 * \param 	addr - buffer containing address
 */
void rf24_set_address(rf24_pipe_t pipe, const uint8_t *addr) {
    uint8_t len;
    if ((pipe > RF24_PIPE1) && (pipe < RF24_TX_ADDR)) {
        rf24_write_reg(RX_ADDR_P0 + pipe, *addr);
    } else {
        len = rf24_get_address_width();
        rf24_write_buf(WRITE_REG + RX_ADDR_P0 + pipe, addr, len);
    }

}

/**
 * \brief	This function initializes the RF module
 * \details Assigns \a address to Pipe 0 and Tx address \n
 * 			Enable pipes (with/without auto ack) \n
 * 			Set data rate, power, payload length and other things \n
 *			Set mode to Tx/Rx
 * \param 	mode	Specifies Tx or Rx mode
 * \param 	address	buffer containing address to be used as Pipe 0 and Tx address
 * \return	Status of initialization
 * 			* 0 - Success
 * 			* 1 - Error with SPI communication
 */
uint8_t rf24_init(rf24_opmode_t mode, const uint8_t *address) {
    uint8_t reg_val, ret = 0;
#if RFM7x_INIT
    uint8_t i, j, WriteArr[12];
#endif

#ifdef LED_DEBUG
	DBG_LED_OUT();
#endif

    /* low level initialization */
    mcu_init();

#if RFM7x_INIT
    _delay_ms(20); //delay more than 50ms?
    /* For RFM7x only: configure bank 0 registers */
    rfm7x_switch_bank(0);
#endif

    /* Set Address */
    rf24_write_reg(SETUP_AW, (uint8_t) (CONFIG_RF24_ADDR_LEN - 2)); /* Address width */
    rf24_set_address(RF24_TX_ADDR, address); /* Set same address for Tx and Rx-Pipe0, for Auto-ACK */
    rf24_set_address(RF24_PIPE0, address);
    /* Set other pipe addresses */
    rf24_set_address(RF24_PIPE1, pipe1_addr);
    /* Open channels */
    reg_val = (1 << 0) | (1 << 1); /* open Pipe0, Pipe1 */
    rf24_write_reg(EN_RXADDR, reg_val);
    reg_val = 0;
    if (CONFIG_RF24_AUTOACK_ENABLED) {
        reg_val = (1 << 0) | (1 << 1); /* Enable auto ack (only for Pipe 0, Pipe 1) */
    }
    rf24_write_reg(EN_AA, reg_val);
    /* Set payload length (only for Pipe 0)*/
	rf24_write_reg(RX_PW_P0, CONFIG_RF24_STATIC_PL_LENGTH);
	rf24_write_reg(RX_PW_P1, CONFIG_RF24_STATIC_PL_LENGTH);

    /* FEATURE reg */
	reg_val = (1 << 0); /* EN_DYN_ACK bit */
	if(CONFIG_RF24_DYNAMIC_PL_ENABLED) {
		reg_val |= (1 << 2)|(1 << 1); /* EN_DPL and EN_ACK_PAY bits */
	}
	if (0 == rf24_read_reg(FEATURE)) {
		rf24_write_reg(ACTIVATE, 0x73); /* Unlock FEATURE register */
	}
	rf24_write_reg(FEATURE, reg_val);

    /* DYNPD register */
    if (CONFIG_RF24_DYNAMIC_PL_ENABLED) { /* Enable dynamic payload length (for Pipe 0, Pipe 1 only) */
        reg_val = (1 << 0) | (1 << 1);
        rf24_write_reg(DYNPD, reg_val);
    }

    /* Retransmit reg */
    reg_val = 0;
    if (CONFIG_RF24_AUTOACK_ENABLED) {
        reg_val = (((CONFIG_RF24_RETRANS_DELAY / 250) - 1) << 4)
                | (CONFIG_RF24_TX_RETRANSMITS & 0xF);
    }
    rf24_write_reg(SETUP_RETR, reg_val);

    /* RF setup reg */
    reg_val = rf24_read_reg(RF_SETUP);
    reg_val &= ~(RF_DR_LOW | RF_DR_HIGH | RF_PWR1 | RF_PWR0);
    if (CONFIG_RF24_DATA_RATE == RF24_RATE_250KBPS) {
        reg_val |= RF_DR_LOW;
    } else if (CONFIG_RF24_DATA_RATE == RF24_RATE_2MBPS) {
        reg_val |= RF_DR_HIGH;
    }
    reg_val |= (CONFIG_RF24_TX_PWR << 1);
    rf24_write_reg(RF_SETUP, reg_val);

    /* RF Channel reg*/
    rf24_write_reg(RF_CH, CONFIG_RF24_RF_CHANNEL);
    reg_val = rf24_read_reg(RF_CH);
#ifdef LED_DEBUG
    LED_Debug(reg_val);
#endif
    if (CONFIG_RF24_RF_CHANNEL != reg_val) {
        ret = 1;
    }

#if RFM7x_INIT
    /* For RFM7x only: Write Bank1 registers */
    rfm7x_switch_bank(1);
    for(i=0;i<=8;i++) //reverse
    {
        for(j=0;j<4;j++)
        WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(j) ) )&0xff;
        rf24_write_arr(i,&(WriteArr[0]),4);
    }
    for(i=9;i<=13;i++)
    {
        for(j=0;j<4;j++)
        WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(3-j) ) )&0xff;
        rf24_write_arr(i,&(WriteArr[0]),4);
    }
    for(j=0;j<11;j++)
    {
        WriteArr[j]=Bank1_Reg14[j];
    }
    rf24_write_arr(14,&(WriteArr[0]),11);
    //toggle REG4<25,26>
    for(j=0;j<4;j++)
    WriteArr[j]=(Bank1_Reg0_13[4]>>(8*(j) ) )&0xff;
    WriteArr[0]=WriteArr[0]|0x06;
    rf24_write_arr(4,&(WriteArr[0]),4);

    WriteArr[0]=WriteArr[0]&0xf9;
    rf24_write_arr(4,&(WriteArr[0]),4);
    /* Switch back to bank 0 */
    rfm7x_switch_bank(0);
    _delay_ms(1);
#endif

    /* Config reg */
    reg_val = (CONFIG_EN_CRC | CONFIG_CRCO | CONFIG_PWR_UP);
    if (mode == RF24_MODE_PRX) {
        reg_val |= 1;
    }
    rf24_write_reg(CONFIG, reg_val);
    rf24_write_reg(FLUSH_RX, 0); //flush Rx fifo
    rf24_write_reg(FLUSH_TX, 0); //flush Tx fifo
    rf24_write_reg(STATUS, STAT_TX_DS | STAT_MAX_RT | STAT_RX_DR);  /* Clear interrupt flags */
    if (mode == RF24_MODE_PRX) {
    	CE_HIGH(); /* Set CE High for Rx mode */
    }
    _delay_ms(5); /* delay after power up */

    return ret;
}

/**
 * \brief Switch to Rx mode
 */
void rf24_rx_mode(void) {
    uint8_t value;

    rf24_write_reg(FLUSH_RX, 0); /* flush Rx FIFO */
    value = rf24_read_reg(STATUS);
    rf24_write_reg(STATUS, value); /* clear interrupt flags of STATUS register */
    CE_LOW();
    value = rf24_read_reg(CONFIG); /* Read CONFIG register and  */
    value |= CONFIG_PRIM_RX; /* set PRIM_RX to enable Rx mode */
    rf24_write_reg(CONFIG, value);
    CE_HIGH(); /* Set CE high to enter Rx mode */
}

/**
 * \brief	Switch to Tx mode
 */
void rf24_tx_mode(void) {
    uint8_t value;

    rf24_write_reg(FLUSH_TX, 0); /* Flush Tx FIFO */
    CE_LOW(); /* Set CE low to exit Rx mode */
    value = rf24_read_reg(CONFIG); /* Read CONFIG register and  */
    value &= ~CONFIG_PRIM_RX; /* clear PRIM_RX to enable Tx mode */
    rf24_write_reg(CONFIG, value);
}

/**
 * \brief 	Enter Power down mode
 */
void rf24_powerdown(void) {

    uint8_t value = rf24_read_reg(CONFIG);
    value &= ~CONFIG_PWR_UP; /* Clear PWR_UP bit in CONFIG register to enter Power down mode */
    rf24_write_reg(CONFIG, value);
}



/**
 * \brief 	Exit Power down mode
 */
void rf24_powerup(void) {

    uint8_t value = rf24_read_reg(CONFIG);
    value |= CONFIG_PWR_UP; /* Set PWR_UP bit in CONFIG register to exit Power down mode */
    rf24_write_reg(CONFIG, value);
	_delay_ms(5);	// 1.5ms settling time from Power down mode
}



/**
 * \brief 	This function transmits an RF packet in the specified buffer of the given length
 * \param 	packet - buffer containing packet data
 * \param 	length - size of the packet
 * \return 	Status after transmit operation
 * 			0 - Successfully transmitted
 * 			1 - Not successful (ACK not received)
 * 			2 - Tx FIFO full
 */
uint8_t rf24_transmit_packet(const uint8_t *packet, uint8_t length) {
    uint8_t ret;

    if (rf24_read_reg(FIFO_STATUS) & TX_FIFO_FULL) {
        return 2;
    }
    //rf24_write_multibyte_reg(RF24_TX_PLOAD, packet, length);
    rf24_write_buf(WR_TX_PLOAD, packet, length);
    CE_PULSE()
    ;
    do {
#if CONFIG_RF24_POLLED_MODE
        rf24_irq();
#endif
        if (tx_done == true) {
            tx_done = false;
            ret = 0;
            break;
        }
        if (max_retries == true) {
            max_retries = false;
            ret = 1;
            break;
        }
    } while (1);

    return ret;
}

/**
 * \brief 	This function transmits an RF packet in the specified buffer of the given length, without Auto-Acknowlegment feature
 * \details	The transmitted packet will have NO_ACK flag set, telling the receiver to not acknowledge the packet
 * \param 	packet - buffer containing packet data
 * \param 	length - size of the packet
 * \return 	Status after transmit operation
 * 			0 - Successfully transmitted
 * 			1 - Not successful (ACK not received)
 * 			2 - Tx FIFO full
 */
uint8_t rf24_transmit_packet_noack(const uint8_t *packet, uint8_t length) {
    uint8_t ret;

    if (rf24_read_reg(FIFO_STATUS) & TX_FIFO_FULL) {
        return 2;
    }
    //rf24_write_multibyte_reg(RF24_TX_PLOAD, packet, length);
    rf24_write_buf(WR_NAC_TX_PLOAD, packet, length);
    CE_PULSE()
    ;
    do {
#if CONFIG_RF24_POLLED_MODE
        rf24_irq();
#endif
        if (tx_done == true) {
            tx_done = false;
            ret = 0;
            break;
        }
        if (max_retries == true) {
            max_retries = false;
            ret = 1;
            break;
        }
    } while (1);

    return ret;
}

/**
 * \brief	This function stores a received packet, if available, into the specified buffer
 * \note	After calling the function, verify non-zero value of \a size to know whether a packet is received.
 * \param	buf		Buffer to store the received packet
 * \param 	size	Location to return the size of the received packet. Zero is returned if packet is invalid or not available.
 * \return	Pipe number of the received packet
 */
uint8_t rf24_receive_packet(uint8_t *buf, uint8_t *size) {
    uint8_t rx_pipe = 0;

#if CONFIG_RF24_POLLED_MODE
    rf24_irq();
#endif
	*size = 0;
    if (rx_ready == true) {
        rx_ready = false;
        rx_pipe = rf24_get_rx_pipe();
        do {
#if CONFIG_RF24_DYNAMIC_PL_ENABLED
            if ((*size = rf24_read_reg(RD_RX_PLOAD_W)) > 32) { // Invalid packet size : discard packet
                *size = 0;
                rf24_write_reg(FLUSH_RX, 0);
                break;
            }
#else
            *size = rf24_read_reg(RX_PW_P0);
#endif
            CSN_LOW();
            SPI_TxRx(RD_RX_PLOAD);
            SPI_RxBuf(buf, *size);
            CSN_HIGH();
        } while (!(rf24_read_reg(FIFO_STATUS) & RX_EMPTY));
    }

    return rx_pipe;
}

/**
 * \brief 	This function stores the given buffer as the ACK payload for the next received packet for a pipe
 * \param 	pipe	Pipe number for the ACK packet
 * \param 	buf		Buffer containing ACK packet data
 * \param 	length	Length of packet
 */
void rf24_set_ack_payload(uint8_t pipe, const uint8_t *buf, uint8_t length) {
    CSN_LOW();
    SPI_TxRx(WR_ACK_PLOAD | pipe);
    SPI_TxBuf(buf, length);
    CSN_HIGH();
}

/**
 * \brief	This function flushes the TX FIFO
 * \details	Used in Tx mode
 */
void rf24_flush_txfifo(void) {
    rf24_write_reg(FLUSH_TX, 0);
}

/**
 * \brief		This function returns the value of OBSERVE_TX register
 * \details		OBSERVE_TX: [PLOS_CNT(7:4)][ARC_CNT(3:0)]
 * 				ARC_CNT resets its value on each packet transmission.
 * 				PLOS_CNT counts upto 0xF without overflow. This function resets PLOS_CNT to 0
 * 				if the value is 0xF.
 * \return		OBSERVE_TX register value
 */
uint8_t rf24_get_observe_tx(void) {
    uint8_t reg_val;
    reg_val = rf24_read_reg(OBSERVE_TX);
    if (reg_val > 0xEF) { /* PLOS_CNT reached maximum value of 0xF */
        rf24_write_reg(RF_CH, rf24_read_reg(RF_CH)); /* reset OBSERVE_TX by writing into RF_CH */
    }
    return reg_val;
}

/**
 * \brief	Function to handle Interrupt of RF module. Also used to poll the status continuously in polled mode
 */
static void rf24_irq(void) {
    uint8_t status;

    //status = rf24_write_reg(STATUS, (STAT_MAX_RT|STAT_TX_DS|STAT_RX_DR)); /* Get and clear interrupt flags */
    status = rf24_read_reg(STATUS);
    if (status) {
        rf24_write_reg(STATUS, status);
        if (status & STAT_RX_DR) {
            /* received data */
            rx_ready = true;
        }

        if (status & STAT_TX_DS) {
            /* transmit done */
            tx_done = true;
        }

        if (status & STAT_MAX_RT) {
            /* Maximum retries exceeded */
            max_retries = true;
            rf24_write_reg(FLUSH_TX, 0); //flush Tx fifo
        }
    }
}

/**
 * \brief	ISR for RF module IRQ
 * \details	Reads status register and sets appropriate flags
 */
#if !CONFIG_RF24_POLLED_MODE
ISR(INT1_vect) {
    //PORTB &= ~(1 << 0); // LED OFF
    rf24_irq();
}
#endif

#ifdef LED_DEBUG
static void LED_Debug(uint8_t value)
{
    volatile uint8_t i;
    for(i = 0; i < 8; i++) {
        DBG_LED_ON();
        _delay_ms(50);
        DBG_LED_OFF();
        _delay_ms(50);
        if(value & (1 << i)) {
            DBG_LED_ON();
            _delay_ms(50);
            DBG_LED_OFF();
        }
        _delay_ms(700);
    }
}
#endif
