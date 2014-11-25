#ifndef __CC2500_H__
#define __CC2500_H__

#include "stm32f4xx_conf.h"
#include <math.h>
#include "SMARTRF_CC2500.h"
#include "Utilities.h"

/**
  * @brief  Register Addresses
  */

#define CC2500REG_IOCFG2 		0x00
#define CC2500REG_IOCFG0 		0x02
#define CC2500REG_FIFOTHR 	0x03
#define CC2500REG_PKTLEN 		0x06
#define CC2500REG_PKTCTRL1 	0x07
#define CC2500REG_PKTCTRL0 	0x08
#define CC2500REG_ADDR 			0x09
#define CC2500REG_CHANNR 		0x0A
#define CC2500REG_FSCTRL1 	0x0B
#define CC2500REG_FSCTRL0 	0x0C
#define CC2500REG_FREQ2 		0x0D
#define CC2500REG_FREQ1 		0x0E
#define CC2500REG_FREQ0 		0x0F
#define CC2500REG_MDMCFG4 	0x10
#define CC2500REG_MDMCFG3 	0x11
#define CC2500REG_MDMCFG2 	0x12
#define CC2500REG_MDMCFG1 	0x13
#define CC2500REG_MDMCFG0 	0x14
#define CC2500REG_DEVIATN 	0x15
#define CC2500REG_MCSM0 		0x18
#define CC2500REG_FOCCFG 		0x19
#define CC2500REG_BSCFG 		0x1A
#define CC2500REG_AGCCTRL2 	0x1B
#define CC2500REG_AGCCTRL1	0x1C
#define CC2500REG_AGCCTRL0	0x1D
#define CC2500REG_FREND1 		0x21
#define CC2500REG_FREND0 		0x22
#define CC2500REG_FSCAL3 		0x23
#define CC2500REG_FSCAL2 		0x24
#define CC2500REG_FSCAL1 		0x25
#define CC2500REG_FSCAL0 		0x26
#define CC2500REG_FSTEST	 	0x29
#define CC2500REG_TEST2 		0x2C
#define CC2500REG_TEST1 		0x2D
#define CC2500REG_TEST0 		0x2E
/* 	Sync	*/
#define CC2500REG_SYNC1 		0x04
/*	Status registers	(Testing purposes)	*/
#define CC2500REG_PARTNUM 	(0xC0|0x30)
#define CC2500REG_VERSION 	(0xC0|0x31)
/*	Buffer status registers	(Read and Burst)	*/
#define CC2500REG_TXBYTES   (0x3A|0xFA)
#define CC2500REG_RXBYTES   (0x3B|0xFB)
/*	Burst access to FIFOs	*/
//#define CC2500REG_TX_FIFO		0x7F
#define CC2500REG_TX_FIFO		0x3F
//#define CC2500REG_RX_FIFO		0xFF
#define CC2500REG_RX_FIFO		0xBF
/*	RSSI Status Register	*/
#define CC2500REG_RSSI			(0xF4|0x34)

/**
  * @brief  CC2500 Command Strobes
	* More details can be found in Table 34 page 57
  */
	
#define SRES 								0x30
#define SCAL 								0x33
#define SRX 								0x34
#define STX 								0x35
#define SIDLE 							0x36
#define SFRX								0x3A
#define SNOP 								0x3D

/**
  * @brief  CC2500 SPI Interface pins
  */

#define CC2500_SPI                       SPI4
#define CC2500_SPI_CLK                   RCC_APB2Periph_SPI4

/*	Clock	*/
#define CC2500_SPI_SCK_PIN               GPIO_Pin_2                 /* PE.02 */
#define CC2500_SPI_SCK_GPIO_PORT         GPIOE                      /* GPIOE */
#define CC2500_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOE
#define CC2500_SPI_SCK_SOURCE            GPIO_PinSource2
#define CC2500_SPI_SCK_AF                GPIO_AF_SPI4

/*	MISO	*/
/*	GDO1	*/
#define CC2500_SPI_MISO_PIN       		   GPIO_Pin_5                  /* PE.05 */
#define CC2500_SPI_MISO_GPIO_PORT   	   GPIOE                       /* GPIOE */
#define CC2500_SPI_MISO_GPIO_CLK    	   RCC_AHB1Periph_GPIOE
#define CC2500_SPI_MISO_SOURCE      	 	 GPIO_PinSource5
#define CC2500_SPI_MISO_AF          		 GPIO_AF_SPI4

/*	MOSI	*/
#define CC2500_SPI_MOSI_PIN              GPIO_Pin_6                  /* PE.06 */
#define CC2500_SPI_MOSI_GPIO_PORT        GPIOE                       /* GPIOE */
#define CC2500_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define CC2500_SPI_MOSI_SOURCE           GPIO_PinSource6
#define CC2500_SPI_MOSI_AF               GPIO_AF_SPI4

/*	NSS	*/ 
#define CC2500_SPI_CS_PIN                GPIO_Pin_4                   /* PE.04 */
#define CC2500_SPI_CS_GPIO_PORT          GPIOE										  	/* GPIOE*/
#define CC2500_SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOE

/*	Interrupt	*/
#define CC2500_SPI_INT1_PIN              GPIO_Pin_12                   /* PE.00 */
#define CC2500_SPI_INT1_GPIO_PORT        GPIOB                        /* GPIOE */
#define CC2500_SPI_INT1_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define CC2500_SPI_INT1_EXTI_LINE        EXTI_Line0
#define CC2500_SPI_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOB
#define CC2500_SPI_INT1_EXTI_PIN_SOURCE  EXTI_PinSource0
#define CC2500_SPI_INT1_EXTI_IRQn        EXTI0_IRQn 

#define CC2500_SPI_INT2_PIN              GPIO_Pin_1                   /* PE.01 */
#define CC2500_SPI_INT2_GPIO_PORT        GPIOE                        /* GPIOE */
#define CC2500_SPI_INT2_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define CC2500_SPI_INT2_EXTI_LINE        EXTI_Line1
#define CC2500_SPI_INT2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define CC2500_SPI_INT2_EXTI_PIN_SOURCE  EXTI_PinSource1
#define CC2500_SPI_INT2_EXTI_IRQn        EXTI1_IRQn 


/* Read/Write command -> Read */
#define READWRITE_CMD              ((uint8_t)0x80) 
/* Multiple byte read/write command -> Burst */ 
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device. */
#define DUMMY_BYTE                 ((uint8_t)0x00)
#define WRITE											 ((uint8_t)0x00)
/*	Maximum length of FIFO is 64 bytes	*/
#define MAX_FIFO_LENGTH						 ((uint8_t) 0x40)
#define CC2500_FLAG_TIMEOUT        ((uint32_t)0x1000)
#define PREAMBLE_BYTE							 ((uint8_t) 0xAA)
// ?? Arbitrary
#define CC2500REG_NODE1						 ((uint8_t) 0x91)
#define CC2500REG_NODE2						 ((uint8_t) 0x92)
#define CC2500REG_NODE3						 ((uint8_t) 0x93)

// fxoc = 26Mhz. fcarrier = 26Mhz/ 2^16 * f[23..0] (freq3,2,1)


#define CC2500_CS_LOW()       		 GPIO_ResetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)
#define CC2500_CS_HIGH()      		 GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)

void CC2500_Init(void);
/*	High level drivers	*/
void CC2500_Transmit_Packet(uint8_t data);
void CC2500_Receive_Packet(void);
/*	Lower level drivers	*/
void CC2500_Write_To_Register(uint8_t	data, uint8_t WriteAddr);
void CC2500_Write_Command_Strobe(uint8_t WriteAddr);
uint8_t CC2500_Read_From_Register(uint8_t ReadAddr);
uint32_t CC2500_TIMEOUT_UserCallback(void);
uint8_t CC2500_SendByte(uint8_t byte);
static void Reset();
static void Enable_RX_FIFO();
static void Enable_TX_FIFO();
static void CC250_Registers_Init(void);
static void CC2500_LowLevel_Init(void);
void EXTI0_IRQHandler(void);

#endif
