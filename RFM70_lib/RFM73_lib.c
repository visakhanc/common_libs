#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "RFM73.h"
#include "avr_spi.h"

#define CSN_LOW()	SS_LOW()
#define CSN_HIGH()	SS_HIGH()

static void mcu_init(void);

static volatile bool tx_done;
static volatile bool rx_ready;
static volatile bool max_retries;


//Bank1 register initialization value

//In the array RegArrFSKAnalog,all the register value is the byte reversed!!!!!!!!!!!!!!!!!!!!!
const unsigned long Bank1_Reg0_13[]={       //latest config txt
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

const UINT8 Bank1_Reg14[]=
{
	0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF
};

//Bank0 register initialization value
const UINT8 Bank0_Reg[][2]={
{0,0x0F},//reflect RX_DR\TX_DS\MAX_RT,Enable CRC ,2byte,POWER UP,PRX
{1,0x3F},//Enable auto acknowledgement data pipe5\4\3\2\1\0
{2,0x3F},//Enable RX Addresses pipe5\4\3\2\1\0
{3,0x03},//RX/TX address field width 5byte
{4,0x15},//auto retransmission dalay (500us),auto retransmission count(5)
{5,0x28},//23 channel
{6,0x07},//air data rate-1Mbps,out power 5dbm,setup LNA gain \bit4 must set up to 0
{7,0x07},//
{8,0x00},//
{9,0x00},
{12,0xc3},//only LSB Receive address data pipe 2, MSB bytes is equal to RX_ADDR_P1[39:8]
{13,0xc4},//only LSB Receive address data pipe 3, MSB bytes is equal to RX_ADDR_P1[39:8]
{14,0xc5},//only LSB Receive address data pipe 4, MSB bytes is equal to RX_ADDR_P1[39:8]
{15,0xc6},//only LSB Receive address data pipe 5, MSB bytes is equal to RX_ADDR_P1[39:8]
{17,0x20},//Number of bytes in RX payload in data pipe0(32 byte) 
{18,0x20},//Number of bytes in RX payload in data pipe1(32 byte)
{19,0x20},//Number of bytes in RX payload in data pipe2(32 byte)
{20,0x20},//Number of bytes in RX payload in data pipe3(32 byte)
{21,0x20},//Number of bytes in RX payload in data pipe4(32 byte)
{22,0x20},//Number of bytes in RX payload in data pipe5(32 byte)
{23,0x00},//fifo status
{28,0x3F},//Enable dynamic payload length data pipe5\4\3\2\1\0
{29,0x07}//Enables Dynamic Payload Length,Enables Payload with ACK,Enables the W_TX_PAYLOAD_NOACK command 
};


//const UINT8 RX0_Address[]={0x34,0x43,0x10,0x10,0x01};//Receive address data pipe 0
const UINT8 RX0_Address[]={0x11,0x22,0x33,0x44,0x55};//Receive address data pipe 0
const UINT8 RX1_Address[]={0x39,0x38,0x37,0x36,0xc2};////Receive address data pipe 1


///////////////////////////////////////////////////////////////////////////////
//                  SPI access                                               //
///////////////////////////////////////////////////////////////////////////////

                                                       
/**************************************************         
Function: SPI_Write_Reg();                                  
                                                            
Description:                                                
	Writes value 'value' to register 'reg'              
**************************************************/        
void SPI_Write_Reg(UINT8 reg, UINT8 value)                 
{
	UINT8 status;
	
	CSN_LOW();                   // CSN low, init SPI transaction
	status = SPI_TxRx(reg);      // select register
	SPI_TxRx(value);             // ..and write value to it..
	CSN_HIGH();                   // CSN high again
}                                                         
/**************************************************/        
                                                            
/**************************************************         
Function: SPI_Read_Reg();                                   
                                                            
Description:                                                
	Read one UINT8 from BK2421 register, 'reg'           
**************************************************/        
UINT8 SPI_Read_Reg(UINT8 reg)                               
{                                                           
	UINT8 value, status;
	
	CSN_LOW();                // CSN low, initialize SPI communication...
	status=SPI_TxRx(reg);            // Select register to read from..
	value = SPI_TxRx(0);    // ..then read register value
	CSN_HIGH();                // CSN high, terminate SPI communication

	return(value);        // return register value
}                                                           

                                                            
/**************************************************         
Function: SPI_Read_Buf();                                   
                                                            
Description:                                                
	Reads 'length' #of length from register 'reg'         

**************************************************/        
void SPI_Read_Buf(UINT8 reg, UINT8 *pBuf, UINT8 length)     
{                                                           
	UINT8 status;                              
                                                            
	CSN_LOW();                    		// Set CSN l
	status = SPI_TxRx(reg);       		// Select register to write, and read status UINT8
	SPI_RxBuf(pBuf, length);			// Read bytes from RFM70 
	CSN_HIGH();                           // Set CSN high again
               
}                                                           
                                                            
/**************************************************         
Function: SPI_Write_Buf();                                  
                                                            
Description:                                                
	Writes contents of buffer '*pBuf' to RFM73         
**************************************************/        
void SPI_Write_Buf(UINT8 reg, UINT8 *pBuf, UINT8 length)    
{                                                                                     
	UINT8 status;
	
	CSN_LOW();                   // Set CSN low, init SPI tranaction
	status = SPI_TxRx(reg);    // Select register to write to and read status UINT8
	SPI_TxBuf(pBuf, length);	   // Write buffer to RFM70
	CSN_HIGH();                 // Set CSN high again      

}



/**************************************************
Function: SetChannelNum();
Description:
	set channel number

**************************************************/
void SetChannelNum(UINT8 ch)
{
	SPI_Write_Reg((UINT8)(WRITE_REG|5),(UINT8)(ch));
}



///////////////////////////////////////////////////////////////////////////////
//                  RFM73 initialization                                    //
///////////////////////////////////////////////////////////////////////////////


/**************************************************         
Function: mcu_init                                 

Description:                                                
	initialization of MCU needed for RFM73
**************************************************/ 
static void mcu_init(void)
{
	CE_OUT();
	SPI_Init(SPI_MODE0, SPI_CLKDIV_4);
	
#if !RFM70_POLLED_MODE
	/* configure INT1 as LOW-level triggered */
	MCUCR &= ~(1 << ISC11);
	MCUCR &= ~(1 << ISC10);
	
	/* Enable INT1 interrupt for RFM70 IRQ */
	GICR |= (1 << INT1);
#endif

}

/**************************************************         
Function: RFM73_Initialize();                                  

Description:                                                
	register initialization
**************************************************/   
void RFM73_Initialize()
{
	UINT8 i,j;
 	UINT8 WriteArr[12];

	mcu_init();
	
	//delay more than 50ms.
	_delay_ms(200);
	
	SwitchCFG(0);

	for(i=0;i<20;i++)
	{
		SPI_Write_Reg((WRITE_REG|Bank0_Reg[i][0]),Bank0_Reg[i][1]);
	}
	
/*//reg 10 - Rx0 addr
	SPI_Write_Buf((WRITE_REG|10),RX0_Address,5);
	
//REG 11 - Rx1 addr
	SPI_Write_Buf((WRITE_REG|11),RX1_Address,5);

//REG 16 - TX addr
	SPI_Write_Buf((WRITE_REG|16),RX0_Address,5);*/

//reg 10 - Rx0 addr
	for(j=0;j<5;j++)
	{
		WriteArr[j]=RX0_Address[j];
	}
	SPI_Write_Buf((WRITE_REG|10),&(WriteArr[0]),5);
	
//REG 11 - Rx1 addr
	for(j=0;j<5;j++)
	{
		WriteArr[j]=RX1_Address[j];
	}
	SPI_Write_Buf((WRITE_REG|11),&(WriteArr[0]),5);
//REG 16 - TX addr
	for(j=0;j<5;j++)
	{
		WriteArr[j]=RX0_Address[j];
	}
	SPI_Write_Buf((WRITE_REG|16),&(WriteArr[0]),5);
	
//	printf("\nEnd Load Reg");

	i=SPI_Read_Reg(29);//read Feature Register 如果要支持动态长度或者 Payload With ACK，需要先给芯片发送 ACTIVATE命令（数据为0x73),然后使能动态长度或者 Payload With ACK (REG28,REG29).
	if(i==0) // i!=0 showed that chip has been actived.so do not active again.
		SPI_Write_Reg(ACTIVATE_CMD,0x73);// Active
	for(i=22;i>=21;i--)
	{
		SPI_Write_Reg((WRITE_REG|Bank0_Reg[i][0]),Bank0_Reg[i][1]);
		//SPI_Write_Reg_Bank0(Bank0_Reg[i][0],Bank0_Reg[i][1]);
	}
	
//********************Write Bank1 register******************
	SwitchCFG(1);
	
	for(i=0;i<=8;i++)//reverse
	{
		for(j=0;j<4;j++)
			WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(j) ) )&0xff;

		SPI_Write_Buf((WRITE_REG|i),&(WriteArr[0]),4);
	}

	for(i=9;i<=13;i++)
	{
		for(j=0;j<4;j++)
			WriteArr[j]=(Bank1_Reg0_13[i]>>(8*(3-j) ) )&0xff;

		SPI_Write_Buf((WRITE_REG|i),&(WriteArr[0]),4);
	}

	//SPI_Write_Buf((WRITE_REG|14),&(Bank1_Reg14[0]),11);
	for(j=0;j<11;j++)
	{
		WriteArr[j]=Bank1_Reg14[j];
	}
	SPI_Write_Buf((WRITE_REG|14),&(WriteArr[0]),11);

//toggle REG4<25,26>
	for(j=0;j<4;j++)
		//WriteArr[j]=(RegArrFSKAnalog[4]>>(8*(j) ) )&0xff;
		WriteArr[j]=(Bank1_Reg0_13[4]>>(8*(j) ) )&0xff;

	WriteArr[0]=WriteArr[0]|0x06;
	SPI_Write_Buf((WRITE_REG|4),&(WriteArr[0]),4);

	WriteArr[0]=WriteArr[0]&0xf9;
	SPI_Write_Buf((WRITE_REG|4),&(WriteArr[0]),4);

	//**************************Test spi*****************************//
	//SPI_Write_Reg((WRITE_REG|Bank0_Reg[2][0]),0x0f);
	//test_data = SPI_Read_Reg(0x02);

	
	//DelayMs(10);
	_delay_ms(50);
	
//********************switch back to Bank0 register access******************
	SwitchCFG(0);
	SwitchToRxMode();//switch to RX mode
}



/**************************************************
Function: SwitchToRxMode();
Description:
	switch to Rx mode
**************************************************/
void SwitchToRxMode()
{
	UINT8 value;

	SPI_Write_Reg(FLUSH_RX,0);//flush Rx

	value=SPI_Read_Reg(STATUS);	// read register STATUS's value
	SPI_Write_Reg(WRITE_REG|STATUS,value);// clear RX_DR or TX_DS or MAX_RT interrupt flag

	CE_LOW();

	value=SPI_Read_Reg(CONFIG);	// read register CONFIG's value
	
//PRX
	value=value|0x01;//set bit 1
  	SPI_Write_Reg(WRITE_REG|CONFIG, value); // Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled..
	
	CE_HIGH();
}



/**************************************************
Function: SwitchToTxMode();
Description:
	switch to Tx mode
**************************************************/
void SwitchToTxMode()
{
	UINT8 value;
	SPI_Write_Reg(FLUSH_TX,0);//flush Tx

	CE_LOW();
	
	value=SPI_Read_Reg(CONFIG);	// read register CONFIG's value
//PTX
	value=value&0xfe;//set bit 0
	value |= (1 << 1); // Set PWR_UP bit
  	SPI_Write_Reg(WRITE_REG|CONFIG, value); // Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled.
	
	CE_HIGH();
	
	_delay_ms(2); // Power-up delay 1.5 ms
}


void SwitchToPowerDownMode(void)
{
	UINT8 value;
	
	value=SPI_Read_Reg(CONFIG);
	value &= ~(1 << 1); // Clear PWR_UP bit
	SPI_Write_Reg(WRITE_REG|CONFIG, value); 
}


/**************************************************
Function: SwitchCFG();
                                                            
Description:
	 access switch between Bank1 and Bank0 

Parameter:
	_cfg      1:register bank1
	          0:register bank0
Return:
     None
**************************************************/
void SwitchCFG(char _cfg)//1:Bank1 0:Bank0
{
	UINT8 Tmp;

	Tmp=SPI_Read_Reg(7);
	Tmp=Tmp&0x80;

	if( ( (Tmp)&&(_cfg==0) )
	||( ((Tmp)==0)&&(_cfg) ) )
	{
		SPI_Write_Reg(ACTIVATE_CMD,0x53);
	}
}



/**************************************************
Function: RFM73_Send_Packet
Description:
	fill FIFO to send a packet
Parameter:
	type: WR_TX_PLOAD or  W_TX_PAYLOAD_NOACK_CMD
	pbuf: a buffer pointer
	len: packet length
Return:
	None
**************************************************/
uint8_t RFM73_Send_Packet(UINT8 type,UINT8* pbuf,UINT8 len)
{
	UINT8 fifo_sta;
	uint8_t ret = 0;
#if RFM70_POLLED_MODE
	uint8_t status;
#endif

	SwitchToTxMode();  //switch to tx mode
	
	CE_HIGH();
	
	fifo_sta=SPI_Read_Reg(FIFO_STATUS);	// read register FIFO_STATUS's value
	if((fifo_sta&FIFO_STATUS_TX_FULL)==0)//if not full, send data (write buff)
	{
		SPI_Write_Buf(type, pbuf, len); // Writes data to buffer
	}
	
	CE_LOW();
	
	do {	
#if RFM70_POLLED_MODE
		status = SPI_Read_Reg(STATUS);
		tx_done = (status & STATUS_TX_DS) ? true:false;
		max_retries = status & STATUS_MAX_RT ? true:false;
		if(rx_ready||max_retries) {
			SPI_Write_Reg(WRITE_REG|STATUS, status);
		}
#endif
		if(tx_done == true) {
			tx_done = false;
			ret = 0;
			break;
		}
		if (max_retries == true) {
			max_retries = false;
			ret = 1;
			break;
		}
	} while(1);

	return ret;
}


/**************************************************
Function: Receive_Packet
Description:
	Waits for packet to be read from FIFO and reads 
	packet into given buffer
Parameter:
	None
Return:
	None
**************************************************/
void RFM73_Receive_Packet(UINT8* pbuf,UINT8* length)
{
	UINT8 len, fifo_sta;
#if RFM70_POLLED_MODE
	UINT8 status;
#endif
	
	/* Wait for RX_DR */
#if RFM70_POLLED_MODE
	/* Check for Rx packet ready in STATUS reg */
	status = SPI_Read_Reg(STATUS);
	rx_ready = (status & STATUS_RX_DR) ? true : false;
	if(rx_ready) {
		/* Clear flag */
		SPI_Write_Reg(WRITE_REG|STATUS, status);
	}
#endif
	
	if(rx_ready == true)
	{
		rx_ready = false;

		do
		{
			len=SPI_Read_Reg(R_RX_PL_WID_CMD);	// read len

			if(len<=MAX_PACKET_LEN)
			{
				SPI_Read_Buf(RD_RX_PLOAD,pbuf,len);// read receive payload from RX_FIFO buffer
				*length = len;
			}
			else
			{
				SPI_Write_Reg(FLUSH_RX,0);//flush Rx
				*length = 0;
			}

			fifo_sta=SPI_Read_Reg(FIFO_STATUS);	// read register FIFO_STATUS's value
							
		} while((fifo_sta&FIFO_STATUS_RX_EMPTY)==0); //while not empty
	}
	else {
		*length = 0;
	}
	
}



/**************************************************
Function: ISR for RFM73 IRQ
Description:
	Reads status register and sets appropriate flags
Parameter:
	None
Return:
	None
**************************************************/
#if !RFM70_POLLED_MODE
ISR(INT1_vect)
{
	uint8_t status;

	/* Read interrupt status */
	status = SPI_Read_Reg(STATUS);
	if(status & STATUS_RX_DR)
	{
		/* received data */
		rx_ready = true;
	}
	
	if(status & STATUS_TX_DS)
	{
		/* transmit done */
		tx_done = true;
	}
	
	if(status & STATUS_MAX_RT)
	{
		/* Maxtimum reties exceeded */
		max_retries = true;
	}
	
	/* Clear interrupt flags */
	SPI_Write_Reg(WRITE_REG|STATUS, status);
}
#endif