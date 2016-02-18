/* 
 * Register definitions for RFM70 RF modules 
 */
 
#ifndef RFM70_REG_H
#define RFM70_REG_H


/* RFM70 Instruction Definitions */
#define WRITE_REG     	0x20  
#define RD_RX_PLOAD_W   0x60
#define RD_RX_PLOAD   	0x61  
#define WR_TX_PLOAD   	0xA0  
#define WR_ACK_PLOAD  	0xA8  
#define WR_NAC_TX_PLOAD 0xB0
#define FLUSH_TX      	0xE1  
#define FLUSH_RX      	0xE2  
#define REUSE_TX_PL   	0xE3  
#define LOCK_UNLOCK   	0x50  
#define NOP           	0xFF 


/* RFM70 Register address definitions */
#define CONFIG        0x00
#define EN_AA         0x01
#define EN_RXADDR     0x02
#define SETUP_AW      0x03
#define SETUP_RETR    0x04
#define RF_CH         0x05
#define RF_SETUP      0x06
#define STATUS        0x07
#define OBSERVE_TX    0x08
#define CD            0x09
#define RX_ADDR_P0    0x0A
#define RX_ADDR_P1    0x0B
#define RX_ADDR_P2    0x0C
#define RX_ADDR_P3    0x0D
#define RX_ADDR_P4    0x0E
#define RX_ADDR_P5    0x0F
#define TX_ADDR       0x10
#define RX_PW_P0      0x11
#define RX_PW_P1      0x12
#define RX_PW_P2      0x13
#define RX_PW_P3      0x14
#define RX_PW_P4      0x15
#define RX_PW_P5      0x16
#define FIFO_STATUS   0x17
#define DYNPD         0x1C
#define FEATURE       0x1D

/********** Register bit definitions **************/
/* STATUS Reg bits */
#define STAT_MAX_RT		(1 << 4)
#define STAT_TX_DS		(1 << 5)
#define STAT_RX_DR		(1 << 6)
#define STAT_RX_P_NO	(7 << 1)
#define STAT_TX_FULL	(1 << 0)

/* CONFIG register bits */
#define CONFIG_RX_DR    (1 << 6)
#define CONFIG_TX_DS    (1 << 5)
#define CONFIG_MAX_RT   (1 << 4)
#define CONFIG_EN_CRC   (1 << 3)
#define CONFIG_CRCO     (1 << 2)
#define CONFIG_PWR_UP   (1 << 1)
#define CONFIG_PRIM_RX  (1 << 0)


/* RF_SETUP register bit definitions */
#define RF_CONT_WAVE	(1 << 7)
#define RF_DR_LOW     	(1 << 5)     
#define RF_PLL_LOCK   	(1 << 4)     
#define RF_DR_HIGH     	(1 << 3)     
#define RF_PWR1       	(1 << 2)     
#define RF_PWR0       	(1 << 1)     
#define RF_LNA     	 	(1 << 0)     


/* FIFO_STATUS register bits */
#define TX_REUSE 		(1 << 6)    
#define TX_FIFO_FULL  	(1 << 5)    
#define TX_EMPTY      	(1 << 4)    
#define RX_FULL       	(1 << 1)    
#define RX_EMPTY      	(1 << 0)    


/* Operation mode */
typedef enum {
    RFM70_MODE_PTX = 0,            
    RFM70_MODE_PRX             
} rfm70_opmode_t;


/* Output power modes */
typedef enum {
    RFM70_PWR_M10DBM = 0,         
    RFM70_PWR_M5DBM,
    RFM70_PWR_0DBM, 
    RFM70_PWR_P5DBM  
} rfm70_power_t;

/* data rate */
typedef enum {        
	RFM70_RATE_250KBPS = 0,
    RFM70_RATE_1MBPS,          
    RFM70_RATE_2MBPS           
} rfm70_datarate_t;


/* pipe numbers */
typedef enum {
    RFM70_PIPE0 = 0,         
    RFM70_PIPE1,         
    RFM70_PIPE2,         
    RFM70_PIPE3,         
    RFM70_PIPE4,         
    RFM70_PIPE5,
	RFM70_TX_PIPE,
	RFM70_TX_PLOAD, /* for writing tx payload */
	RFM70_TX_PLOAD_NOACK, /* for tx payload with no ACK */
	RFM70_RX_PLOAD, /* for reading rx payload */
    RFM70_PIPE_ALL = 0xFF
} rfm70_pipe_t;



#endif
