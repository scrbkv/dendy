#include "w25qxx_qspi.h"
#include "quadspi.h"

static uint32_t QSPI_ResetDevice(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_EnterQPI(QSPI_HandleTypeDef *hqspi);
static uint32_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);
static uint32_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);

static uint8_t QSPI_Send_CMD(QSPI_HandleTypeDef *hqspi,uint32_t instruction, uint32_t address,uint32_t addressSize,uint32_t dummyCycles, 
                    uint32_t instructionMode,uint32_t addressMode, uint32_t dataMode, uint32_t dataSize);
										
w25qxx_StatusTypeDef w25qxx_Mode = w25qxx_SPIMode;
uint8_t w25qxx_StatusReg[3];
uint16_t w25qxx_ID;

void w25qxx_Init(void)
{
	HAL_Delay(5);
	MX_QUADSPI_Init();
	QSPI_ResetDevice(&hqspi);
	HAL_Delay(0); // 1ms wait device stable
	w25qxx_ID = w25qxx_GetID();
	w25qxx_ReadAllStatusReg();
}

uint16_t w25qxx_GetID(void)
{
	uint8_t ID[6];
	uint16_t deviceID;
	
	if(w25qxx_Mode == w25qxx_SPIMode)
		QSPI_Send_CMD(&hqspi,W25X_QUAD_ManufactDeviceID,0x00,QSPI_ADDRESS_24_BITS,6,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_4_LINES, QSPI_DATA_4_LINES, sizeof(ID));
	else
		QSPI_Send_CMD(&hqspi,W25X_ManufactDeviceID,0x00,QSPI_ADDRESS_24_BITS,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES, QSPI_DATA_4_LINES, sizeof(ID));

	/* Reception of the data */
  if (HAL_QSPI_Receive(&hqspi, ID, 100) != HAL_OK)
  {
    return w25qxx_ERROR;
  }
	deviceID = (ID[0] << 8) | ID[1];

	return deviceID;
}

uint8_t w25qxx_ReadSR(uint8_t SR)
{
	uint8_t byte=0;
	if(w25qxx_Mode == w25qxx_SPIMode)
		QSPI_Send_CMD(&hqspi,SR,0x00,QSPI_ADDRESS_8_BITS,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE, QSPI_DATA_1_LINE, 1);
	else
		QSPI_Send_CMD(&hqspi,SR,0x00,QSPI_ADDRESS_8_BITS,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE, QSPI_DATA_4_LINES, 1);
	
	if (HAL_QSPI_Receive(&hqspi,&byte,100) != HAL_OK)
	{
		
	}
  return byte;
}

uint8_t w25qxx_WriteSR(uint8_t SR,uint8_t data)
{
	if(w25qxx_Mode == w25qxx_SPIMode)
		QSPI_Send_CMD(&hqspi,SR,0x00,QSPI_ADDRESS_8_BITS,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE, QSPI_DATA_1_LINE, 1);
	else
		QSPI_Send_CMD(&hqspi,SR,0x00,QSPI_ADDRESS_8_BITS,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE, QSPI_DATA_4_LINES, 1);

  return HAL_QSPI_Transmit(&hqspi,&data,100);
}

uint8_t w25qxx_ReadAllStatusReg(void)
{
	
	w25qxx_StatusReg[0] = w25qxx_ReadSR(W25X_ReadStatusReg1);
	w25qxx_StatusReg[1] = w25qxx_ReadSR(W25X_ReadStatusReg2);
	w25qxx_StatusReg[2] = w25qxx_ReadSR(W25X_ReadStatusReg3);
	return w25qxx_OK;
}

//�ȴ�����
void W25QXX_Wait_Busy(void)
{
	while((w25qxx_ReadSR(W25X_ReadStatusReg1) & 0x01) == 0x01);
}

// Only use in QPI mode
uint8_t w25qxx_SetReadParameters(uint8_t DummyClock,uint8_t WrapLenth)
{
	uint8_t send;
	send = (DummyClock/2 -1)<<4 | ((WrapLenth/8 - 1)&0x03);
	
	W25qxx_WriteEnable();
	
	QSPI_Send_CMD(&hqspi,W25X_SetReadParam,0x00,QSPI_ADDRESS_8_BITS,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE, QSPI_DATA_4_LINES, 1);
	
	return HAL_QSPI_Transmit(&hqspi,&send,100);
}

uint8_t w25qxx_EnterQPI(void)
{
	/* Enter QSPI memory in QSPI mode */
  if(QSPI_EnterQPI(&hqspi) != w25qxx_OK)
  {
    return w25qxx_ERROR;
  }
	
	return w25qxx_SetReadParameters(8,8);
}


uint8_t W25qxx_WriteEnable(void)
{
	return QSPI_WriteEnable(&hqspi);
}


/**
  * @brief  This function reset the QSPI memory.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static uint32_t QSPI_ResetDevice(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reset enable command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = W25X_EnableReset;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(hqspi, &s_command, 100) != HAL_OK)
  {
    return w25qxx_ERROR;
  }

  /* Send the reset device command */
  s_command.Instruction = W25X_ResetDevice;
  if (HAL_QSPI_Command(hqspi, &s_command, 100) != HAL_OK)
  {
    return w25qxx_ERROR;
  }

  s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  s_command.Instruction       = W25X_EnableReset;
  /* Send the command */
  if (HAL_QSPI_Command(hqspi, &s_command, 100) != HAL_OK)
  {
    return w25qxx_ERROR;
  }

  /* Send the reset memory command */
  s_command.Instruction = W25X_ResetDevice;
  if (HAL_QSPI_Command(hqspi, &s_command, 100) != HAL_OK)
  {
    return w25qxx_ERROR;
  }
	
  return w25qxx_OK;
}

/**
 * @brief	QSPI��������
 *
 * @param   instruction		Ҫ���͵�ָ��
 * @param   address			���͵���Ŀ�ĵ�ַ
 * @param   addressSize	���͵���Ŀ�ĵ�ַ��С
 * @param   dummyCycles		��ָ��������
 * @param   instructionMode		ָ��ģʽ;
 * @param   addressMode		��ַģʽ; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINES,QSPI_ADDRESS_4_LINES
 * @param   dataMode		����ģʽ; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINES,QSPI_DATA_4_LINES
 * @param   dataSize        ����������ݳ���
 *
 * @return  uint8_t			w25qxx_OK:����
 *                      w25qxx_ERROR:����
 */
static uint8_t QSPI_Send_CMD(QSPI_HandleTypeDef *hqspi,uint32_t instruction, uint32_t address,uint32_t addressSize,uint32_t dummyCycles, 
                    uint32_t instructionMode,uint32_t addressMode, uint32_t dataMode, uint32_t dataSize)
{
    QSPI_CommandTypeDef Cmdhandler;

    Cmdhandler.Instruction        = instruction;   
	  Cmdhandler.InstructionMode    = instructionMode;  
	
    Cmdhandler.Address            = address;
    Cmdhandler.AddressSize        = addressSize;
	  Cmdhandler.AddressMode        = addressMode;
	  
	  Cmdhandler.AlternateBytes     = 0x00;
    Cmdhandler.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
	  Cmdhandler.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;                              
    Cmdhandler.DummyCycles        = dummyCycles;                   
       					      				
    Cmdhandler.DataMode           = dataMode;
    Cmdhandler.NbData             = dataSize; 
	
    Cmdhandler.DdrMode            = QSPI_DDR_MODE_DISABLE;           	
    Cmdhandler.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
    Cmdhandler.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

    if(HAL_QSPI_Command(hqspi, &Cmdhandler, 100) != HAL_OK)
      return w25qxx_ERROR;

    return w25qxx_OK;
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static uint32_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;

  /* Enable write operations */
	if(w25qxx_Mode == w25qxx_QPIMode)
		s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	else 
		s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;

  s_command.Instruction       = W25X_WriteEnable;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(hqspi, &s_command, 100) != HAL_OK)
  {
    return w25qxx_ERROR;
  }

  /* Configure automatic polling mode to wait for write enabling */
  s_config.Match           = W25X_SR_WREN;
  s_config.Mask            = W25X_SR_WREN;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  s_command.Instruction    = W25X_ReadStatusReg1;
	
	if(w25qxx_Mode == w25qxx_QPIMode)
		s_command.DataMode     = QSPI_DATA_4_LINES;
  else 
		s_command.DataMode     = QSPI_DATA_1_LINE;
	
  if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, 100) != HAL_OK)
  {
    return w25qxx_ERROR;
  }

  return w25qxx_OK;
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hqspi: QSPI handle
  * @param  Timeout
  * @retval None
  */
static uint32_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;

  /* Configure automatic polling mode to wait for memory ready */
	
	if(w25qxx_Mode == w25qxx_SPIMode)
		s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	else
		s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	
  s_command.Instruction       = W25X_ReadStatusReg1;
	
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.Address           = 0x00;
	s_command.AddressSize       = QSPI_ADDRESS_8_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	
	if(w25qxx_Mode == w25qxx_SPIMode)
		s_command.DataMode        = QSPI_DATA_1_LINE;
  else
		s_command.DataMode        = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  s_config.Match           = 0;
	s_config.Mask            = W25X_SR_WIP;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
  s_config.StatusBytesSize = 1;
  
  return HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, Timeout);

}

/**
  * @brief  This function enter the QSPI memory in QPI mode
  * @param  hqspi QSPI handle 
  * @retval QSPI status
  */
static uint8_t QSPI_EnterQPI(QSPI_HandleTypeDef *hqspi)
{
	uint8_t stareg2;
	stareg2 = w25qxx_ReadSR(W25X_ReadStatusReg2);
	if((stareg2 & 0X02) == 0) //QEλδʹ��
	{
		W25qxx_WriteEnable();
		stareg2 |= 1<<1; //ʹ��QEλ
		w25qxx_WriteSR(W25X_WriteStatusReg2,stareg2);
	}
	QSPI_Send_CMD(hqspi,W25X_EnterQSPIMode,0x00,QSPI_ADDRESS_8_BITS,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_DATA_NONE,0);
  
	/* Configure automatic polling mode to wait the memory is ready */
  if (QSPI_AutoPollingMemReady(hqspi, 100) != w25qxx_OK)
  {
    return w25qxx_ERROR;
  }
	
  w25qxx_Mode = w25qxx_QPIMode;
	
  return w25qxx_OK;
}

uint8_t Flash_4k_Read(uint32_t addr, uint8_t* buf) {
	uint8_t result;

	QSPI_CommandTypeDef      s_command;

	/* Configure the command for the read instruction */

	if(w25qxx_Mode == w25qxx_QPIMode)
	{
		s_command.Instruction     = W25X_QUAD_INOUT_FAST_READ_CMD;
		s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;
		s_command.DummyCycles     = W25X_DUMMY_CYCLES_READ_QUAD;
	}
	else
	{
		s_command.Instruction     = W25X_QUAD_INOUT_FAST_READ_CMD;
		s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
		s_command.DummyCycles     = W25X_DUMMY_CYCLES_READ_QUAD-2;
	}

	s_command.Address           = addr;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;

	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
	s_command.AlternateBytes    = 0xFF;
	s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;

	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.NbData            = 4096;

	s_command.DdrMode         = QSPI_DDR_MODE_DISABLE;

	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	result = HAL_QSPI_Command(&hqspi, &s_command, 100);

	if(result == w25qxx_OK)
		result = HAL_QSPI_Receive(&hqspi, buf, 100);

	return result;
}

uint8_t Flash_4k_Write(uint32_t addr, const uint8_t* buf) {
	uint8_t result;

	W25qxx_WriteEnable();
	W25QXX_Wait_Busy();

	result = QSPI_Send_CMD(&hqspi,W25X_SectorErase,addr,QSPI_ADDRESS_24_BITS,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_DATA_NONE,0);

	if (result != w25qxx_OK)
		return result;

	W25QXX_Wait_Busy();

	for (int i = 0; i < 16; ++i) {
		W25qxx_WriteEnable();
		W25QXX_Wait_Busy();

		result = QSPI_Send_CMD(&hqspi,W25X_PageProgram,addr,QSPI_ADDRESS_24_BITS,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_DATA_4_LINES,256);
		if (result != w25qxx_OK)
			return result;

		result = HAL_QSPI_Transmit(&hqspi, buf, 100);
		if (result != w25qxx_OK)
			return result;

		W25QXX_Wait_Busy();
		addr += 256;
		buf += 256;
	}

	return result;
}
