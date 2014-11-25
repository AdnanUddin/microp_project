#include "CC2500.h"

__IO uint32_t  CC2500DLTimeout = CC2500_FLAG_TIMEOUT;
uint8_t ctrl;
uint8_t data[(uint8_t)SMARTRF_SETTING_PKTLEN] = {0}; 
uint8_t Received_flag = 0;
uint8_t Transmitted_flag = 0;
uint8_t Transfer_mode = 0;

/*	RSSI constants	*/
float A = -45.0; // Reference RSSI value at 1 meter
float n = 2.7; // Path-loss exponent, (Inside a typical office building, no line of sight)

void CC2500_Init(void)
{	
	/*	TODO: Power-on condition	p.40*/
	// Need to wait (max) 5ms so that the voltage ramps up from 0 to 1.8V
	// For power off (not needed here) we need to wait a minimum of 1ms
	// Once connected to the power supply, we can reset using SRES command strobe
	/* Configure the low level interfaces */
	
	CC2500_LowLevel_Init();
	
	Reset();
	
	CC250_Registers_Init();
	
	
	/*	TODO: Enable the interrupt for the SPI */
  //LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG1_ADDR, 1);
}

double Distance(void)
{
	//	http://stupidembeddedblog.blogspot.ca/2014/05/estimating-distance-from-rssi-values.html
	uint8_t RSSI_hex = CC2500_Read_From_Register(CC2500REG_RSSI);
	double RSSI_dec = (double) RSSI_hex; // TODO 2:'s complement value to decimal
	double RSSI_dBm;
	double RSSI_offset	= 72.0; // ???
	if (RSSI_dec >= 128.0)
		RSSI_dBm = (RSSI_dec - 256.0) - RSSI_offset;
	else
		RSSI_dBm = (RSSI_dec/2.0) - RSSI_offset;
	
	/*	Calculate the Distance according to the RSSI_dBm */
	double Distance = pow(10.0,((A + RSSI_dBm)/(10.0*n)));
	return Distance;
}

/*	Load TX FIFO and transmit via wireless communication 	*/
void CC2500_Transmit_Packet(uint8_t data)
{	
	Transfer_mode = 0;
	
	uint8_t NumBytesinFIFO = CC2500_Read_From_Register(CC2500REG_TXBYTES);
	
	CC2500_CS_LOW();
		
	//	Send Header byte (Burst access included) then send the Address Byte
	//	Since the length is fixed, we do not need to include it here
	//	Note here that the Preamble bytes and the Sync words are inserted automatically in TX
	//	Preamble is configured to 8 bytes and 16/16 Sync word bits detected
	//	Address byte is optional so we did not include it
	
	//	Header byte
	uint8_t status_header = CC2500_SendByte(CC2500REG_TX_FIFO);
	//	Address byte
	uint8_t status_address = CC2500_SendByte(CC2500REG_NODE1);
	
	/* Send the data that will be written into the device (MSB First) */
	uint8_t status_data = CC2500_SendByte(data);
	
	printf("%d\n %d\n %d\n", status_header, status_address, status_data);
	CC2500_CS_HIGH();
	
	if (NumBytesinFIFO > 0)
	{
		Transmitted_flag = 0;
		/*	Enable 64 bytes TX FIFO	*/
		//	Goes to IDLE mode after automatically
		Enable_TX_FIFO();
	}
}

void CC2500_Receive_Packet(void)
{	
	
	/*	Enable 64 bytes RX FIFO	*/
	//	Goes to IDLE mode after automatically	
	Enable_RX_FIFO();
	
	CC2500_CS_LOW();
	uint8_t NumBytesinFIFO = CC2500_Read_From_Register(CC2500REG_RXBYTES);
	CC2500_CS_HIGH();
	
	delay(1);
	
	if (NumBytesinFIFO > 0)
	{
		Received_flag = 0;
		
		/* Set chip select Low at the start of the transmission */
		CC2500_CS_LOW();
		
		/*	Send Header byte (Single access)	*/

		for (int i = 0; i < NumBytesinFIFO; i++)
		{
			uint8_t address = CC2500_SendByte(CC2500REG_RX_FIFO);
			uint8_t data = CC2500_SendByte(DUMMY_BYTE);
			printf("%d\n %d\n", address, data);
		}
		/* Send dummy byte (0x00) to generate the SPI clock to CC2500 (Slave device) */
		//uint8_t address = CC2500_SendByte(DUMMY_BYTE);
		
		  /* Set chip select High at the end of the transmission */ 
		CC2500_CS_HIGH();
	
		CC2500_Write_Command_Strobe(SIDLE);
		CC2500_Write_Command_Strobe(SFRX);
		/*	TODO: For packet lengths less than 64 bytes it is recommended to wait until the complete packet has been received before reading it out of the RX FIFO	
				Store the data of *pBuffer in an array at every iteration and retrieve it after	*/
		//printf("%d\n", address);
		//printf("%d\n %d\n %d\n \n", NumBytesinFIFO, data);
	}
}

void CC2500_Write_To_Register(uint8_t data, uint8_t WriteAddr)
{
	CC2500_CS_LOW();
	/*	Send Header Byte	*/
	CC2500_SendByte(WriteAddr);
	/*	Send 1 byte of Data	*/
	CC2500_SendByte(data);
	CC2500_CS_HIGH();
}

/*	Send Command Strobe (not SRES)	*/
void CC2500_Write_Command_Strobe(uint8_t WriteAddr)
{
	CC2500_CS_LOW();
  /* Send the Address of the indexed register */
  CC2500_SendByte(WriteAddr);
	CC2500_CS_HIGH();
}

uint8_t CC2500_Read_From_Register(uint8_t ReadAddr)

{
	ReadAddr |= (uint8_t)READWRITE_CMD;
  CC2500_SendByte(ReadAddr);
	uint8_t Status_byte = CC2500_SendByte(DUMMY_BYTE);
	return Status_byte;
}

uint32_t CC2500_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {   
  }
}
	
/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  */
uint8_t CC2500_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  CC2500DLTimeout = CC2500_FLAG_TIMEOUT;
	
	/* Checks whether the Transmit Buffer Empty Flag is set	*/
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
    if((CC2500DLTimeout--) == 0) return CC2500_TIMEOUT_UserCallback();
  }
  
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(CC2500_SPI, byte);
  
  /* Wait to receive a Byte */
  CC2500DLTimeout = CC2500_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if((CC2500DLTimeout--) == 0) return CC2500_TIMEOUT_UserCallback();
  }
  
  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}

static void Reset()
{

	CC2500_CS_LOW();
	delay(1);	
	CC2500_CS_HIGH();
	
	delay(4);
	
	CC2500_CS_LOW();
	
	//	Wait for device
	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
    if((CC2500DLTimeout--) == 0) 
			CC2500_TIMEOUT_UserCallback();
  }
	
	CC2500_SendByte(SRES);
	
	CC2500_CS_HIGH();
}

/*	Receiving Buffer	*/
static void Enable_RX_FIFO()
{
	/* Read and Burst mode	*/
	//ctrl = READWRITE_CMD | MULTIPLEBYTE_CMD | SRX;`
	CC2500_Write_Command_Strobe(SRX);
}

/*	Transmitting Buffer	*/
static void Enable_TX_FIFO()
{
	/* Write and Burst mode	*/
	CC2500_Write_Command_Strobe(STX);
}

static void CC250_Registers_Init(void)
{
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FSCTRL1, CC2500REG_FSCTRL1);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FSCTRL0, CC2500REG_FSCTRL0);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FREQ2, CC2500REG_FREQ2);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FREQ1, CC2500REG_FREQ1);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FREQ0, CC2500REG_FREQ0);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_MDMCFG4, CC2500REG_MDMCFG4);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_MDMCFG3, CC2500REG_MDMCFG3);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_MDMCFG2, CC2500REG_MDMCFG2);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_MDMCFG1, CC2500REG_MDMCFG1);

	CC2500_Write_To_Register(SMARTRF_SETTING_MDMCFG0, CC2500REG_MDMCFG0);
		
	CC2500_Write_To_Register(SMARTRF_SETTING_CHANNR, CC2500REG_CHANNR);
		
	CC2500_Write_To_Register(SMARTRF_SETTING_DEVIATN, CC2500REG_DEVIATN);
		
	CC2500_Write_To_Register(SMARTRF_SETTING_FREND1, CC2500REG_FREND1);
		
	CC2500_Write_To_Register(SMARTRF_SETTING_FREND0, CC2500REG_FREND0);

	CC2500_Write_To_Register(SMARTRF_SETTING_MCSM0, CC2500REG_MCSM0);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FOCCFG, CC2500REG_FOCCFG);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_BSCFG, CC2500REG_BSCFG);
			
	CC2500_Write_To_Register(SMARTRF_SETTING_AGCCTRL2, CC2500REG_AGCCTRL2);
		
	CC2500_Write_To_Register(SMARTRF_SETTING_AGCCTRL1, CC2500REG_AGCCTRL1);
			
	CC2500_Write_To_Register(SMARTRF_SETTING_AGCCTRL0, CC2500REG_AGCCTRL0);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FSCAL3, CC2500REG_FSCAL3);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FSCAL2, CC2500REG_FSCAL2);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FSCAL1, CC2500REG_FSCAL1);

	CC2500_Write_To_Register(SMARTRF_SETTING_FSCAL0, CC2500REG_FSCAL0);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FSTEST, CC2500REG_FSTEST);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_TEST2, CC2500REG_TEST2);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_TEST1, CC2500REG_TEST1);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_TEST0, CC2500REG_TEST0);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_FIFOTHR, CC2500REG_FIFOTHR);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_IOCFG2, CC2500REG_IOCFG2);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_PKTCTRL1, CC2500REG_PKTCTRL1);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_PKTCTRL0, CC2500REG_PKTCTRL0);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_ADDR, CC2500REG_ADDR);
	
	CC2500_Write_To_Register(SMARTRF_SETTING_PKTLEN, CC2500REG_PKTLEN);

	CC2500_Write_To_Register(SMARTRF_SETTING_IOCFG0D, CC2500REG_IOCFG0);
	//???
	CC2500_Write_To_Register(PREAMBLE_BYTE, CC2500REG_SYNC1);
}


static void CC2500_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(CC2500_SPI_CLK, ENABLE);

  /* Enable SCK, MOSI and MISO GPIO clocks */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_SCK_GPIO_CLK | CC2500_SPI_MISO_GPIO_CLK |	CC2500_SPI_MOSI_GPIO_CLK, ENABLE);

  /* Enable CS  GPIO clock */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_CS_GPIO_CLK, ENABLE);
  
  /* Enable INT1 GPIO clock */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_INT1_GPIO_CLK, ENABLE);
  
  /* Enable INT2 GPIO clock */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_INT2_GPIO_CLK, ENABLE);

  GPIO_PinAFConfig(CC2500_SPI_SCK_GPIO_PORT, CC2500_SPI_SCK_SOURCE, CC2500_SPI_SCK_AF);
  GPIO_PinAFConfig(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_SOURCE, CC2500_SPI_MISO_AF);
  GPIO_PinAFConfig(CC2500_SPI_MOSI_GPIO_PORT, CC2500_SPI_MOSI_SOURCE, CC2500_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_SCK_PIN;
  GPIO_Init(CC2500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  CC2500_SPI_MOSI_PIN;
  GPIO_Init(CC2500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MISO_PIN;
  GPIO_Init(CC2500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(CC2500_SPI);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(CC2500_SPI, &SPI_InitStructure);

  /* Enable SPI2  */
  SPI_Cmd(CC2500_SPI, ENABLE);

  /* Configure GPIO PIN for CC2500 Chip select */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CC2500_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN);
  
  /* Configure GPIO PINs to detect Interrupts */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_INT1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CC2500_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_INT2_PIN;
  GPIO_Init(CC2500_SPI_INT2_GPIO_PORT, &GPIO_InitStructure);
}

void CC2500_interrupt_Enable(void)
{	
	EXTI_InitTypeDef exti_init_s;
	NVIC_InitTypeDef nvic_init_s;
	GPIO_InitTypeDef gpio_init_s;
	
	//	Enable SYSCFG clock for external interrupts
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//	Connect PE1 pin to EXTI Line 0 (INT1)
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
	
	// Configure EXTI Line 0 
	exti_init_s.EXTI_Line = EXTI_Line0;
	exti_init_s.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init_s.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
	exti_init_s.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init_s);
	
	//	Enable and set EXTI Line1 Interrupt to the lowest priority
	nvic_init_s.NVIC_IRQChannel = EXTI0_IRQn;
	nvic_init_s.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_s.NVIC_IRQChannelSubPriority = 0;
	nvic_init_s.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_s);	
}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET && Transfer_mode == 1)
  {
		//	Clear the flags in the sensor's end
		EXTI_ClearITPendingBit(EXTI_Line1);
		Transmitted_flag = 1;
	}
	if(EXTI_GetITStatus(EXTI_Line1) == RESET && Transfer_mode == 0)
  {
		//	Clear the flags in the sensor's end
		EXTI_ClearITPendingBit(EXTI_Line1);
		Received_flag = 1;
	}
}
