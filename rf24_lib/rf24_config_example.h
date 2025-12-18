/*
 * 	@file 		rf24_config_example.h
 *	@brief		This file is a sample for \a rf24_config.h configuration file for rf24 library.
 *	@details 	rf24_config.h file defines various parameters used to initialize and configure the RF module for a project.
 *				To use this sample file, copy it to the project directory, rename it to rf24_config.h and modify
 *				the parameters according to the project's need.
 *	@attention 	IMPORTANT: Each project using rf24 library should have \a rf24_config.h present in the project directory.
 *				Project should be rebuilt after any modification of this file.
 */

#ifndef RF24_CONFIG_H_
#define RF24_CONFIG_H_



/*---------------- I/O PIN DEFINITIONS FOR AVR -----------------*/

/**
 * \brief	Define which AVR pin is connected to the RF module Chip Enable (CE) 
 */
#define CE_DDR		DDRB
#define CE_PORT		PORTB
#define CE_PIN   	1



/*-----------------POLLED/INTERRUPT MODE --------------------*/
/**
 * \brief	Define to 0 if interrupt mode is used. Define to 1 for polled mode.
 * \details	In case of interrupt mode, AVR INT1 pin should be connected to RF Module's IRQ pin. In
 *			case of polled mode, IRQ pin can be left unconnected.
 */
#define CONFIG_RF24_POLLED_MODE 		0



/*----------------- FOR RFM7x Modules ONLY -----------------*/
/**
 * \brief	Define to 1 if RFM70/RFM73/RFM75 module is used. 
 * \details	This will add necessary initialization for extra Bank1 registers of the RFM7x module 
 */
#define RFM7x_INIT	0



/*----------------- ADDRESS CONFIGURATION -----------------*/
/**
 * \brief 	Address of the radio and Address length
 * \details This address is used for configuration of PIPE 0 address and Tx Address (in case of PTX mode) only. Address
 * 			length can be 3 to 5.
 */
#define CONFIG_RF24_ADDRESS		{0x11, 0x22, 0x33, 0x44, 0x55}
#define CONFIG_RF24_ADDR_LEN		5



 
 /*---------------- ENABLE AUTOACK -----------------------*/
/**
 * \brief	Enable or Disable Automatic Acknowledgement/Retransmit feature
 */
 #define CONFIG_RF24_AUTOACK_ENABLED 		1

 
 
/*----------------- ENABLE DYNAMIC PAYLOAD --------------*/
/**
 * \brief	Enable or disable dynamic payload width
 * \details Define as 1 to enable, 0 to disable
 */
#define CONFIG_RF24_DYNAMIC_PL_ENABLED		1


/*----------------- STATIC PAYLOAD LENGTH --------------*/
/**
 * \brief	Define length of the static payload
 * \details	Define this from 0 to 32 (bytes)
 */
#define CONFIG_RF24_STATIC_PL_LENGTH		32


/*---------------- ENABLE ACK PAYLOAD ------------------*/
/**
 * \brief	Define to 1 to enable transmission of ACK payload packets
 * \details	Also define maximum ACK payload length (0 to 32) 
 */
#define CONFIG_RF24_ACK_PL_ENABLED			0
#define CONFIG_RF24_ACK_PL_LENGTH			4



/*---------------- OUTPUT POWER LEVEL -----------------*/
/**
 * \brief	Define transmit output power level
 * \details	Define this to:
 *
 *			RF24_PWR_0DBM 	:  0 dBm	\n
 *			RF24_PWR_M6DBM 	: -6 dBm	\n
 * 			RF24_PWR_M12DBM : -12 dBm	\n
 * 			RF24_PWR_M18DBM : -18 dBm
 */
#define CONFIG_RF24_TX_PWR		RF24_PWR_0DBM



/*---------------- DATA RATE --------------------------*/
/**
 * \brief	Define air data rate
 * \details	Define this to:
 *
 *			RF24_RATE_250KBPS	: 250 kbits/sec		\n
 *			RF24_RATE_1MBPS		: 1 Mbits/sec		\n
 *			RF24_RATE_2MBPS		: 2 Mbits/sec		
 */
#define CONFIG_RF24_DATA_RATE	RF24_RATE_2MBPS


/*---------------- RF CHANNEL ------------------------*/
/**
 * \brief	Define the RF channel for communication
 * \details	Define this to a value in 0 - 127
 */
#define CONFIG_RF24_RF_CHANNEL 	40



/*----------------- RETRANSMIT COUNT ----------------*/
/**
 * \brief	Define how many retransmitts that should be performed
 * \details	Define this to a value in 0 - 15
 */
#define CONFIG_RF24_TX_RETRANSMITS		15



#endif /* RF24_CONFIG_H_ */
