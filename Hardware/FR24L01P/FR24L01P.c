#include "FR24L01P.h"

#define CE_SET()      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define CE_RESET()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)

#define CSN_LOW()     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET)
#define CSN_HIGH()    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET)

 	

#define W_REGISTER_ADDR  0x20u
#define R_REGISTER_ADDR  0x00u

const uint8_t TX_ADDRESS[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
const uint8_t RX_ADDRESS[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
uint8_t NOP = 0xFF;

static void WriteReg(uint8_t Regaddr, uint8_t SendData);
static uint8_t ReadReg(uint8_t Regaddr);
static void Rf24_WritePayload(uint8_t Regaddr, uint8_t* Txdata, uint8_t length);
static void Rf24_ReadPayload(uint8_t Regaddr, uint8_t* RxData, uint8_t length);
static bool Check_Nf24(void);
static void flush_fifo(void);
static uint8_t NRF24L01_SendTxBuf(uint8_t *Buf);
typedef struct
{
    // CONFIG寄存器位
    bool MASK_RX_DR : 1;
    bool MASK_TX_DS : 1;
    bool MASK_MAX_RT : 1;
    bool EN_CRC : 1;
    bool CRCO : 1;
    bool PWR_UP : 1;
    bool PRIM_RX : 1;
    // 其他寄存器位
} StrRegisterTab;

typedef union 
{
    StrRegisterTab bit;
    uint8_t byte[1]; 
} uniRegTab;

uniRegTab RegTab;


uint8_t RRX_PW_P0;
uint8_t REN_AA;
uint8_t REN_RXADDR;
uint8_t RSETUP_RETR;
uint8_t RCONFIG,RRF_CH,RRF_SETUP;
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
void FR24L019P_init(tstFr24State Mode) 
	{
		RegTab.byte[REG_CONFIG] = 0x0FU;
    while (Check_Nf24() != 1u) HAL_Delay(1);
    CE_RESET();
		
		WriteReg(RX_PW_P0, 32); // 设置接收通道0的有效载荷宽度
    flush_fifo(); // 清空TX FIFO
	  Rf24_WritePayload(TX_ADDR, (uint8_t*)TX_ADDRESS, 5);
    Rf24_WritePayload(RX_ADDR_P0, (uint8_t*)RX_ADDRESS, 5);	
		
		if(Mode == FR24_TX_Mode)
		{
		WriteReg(EN_AA,0x3F);
		}
		else
		{
		WriteReg(EN_AA,0x01);
		}
		
		WriteReg(EN_RXADDR,0x01);
		WriteReg(SETUP_RETR,0x1a);
		WriteReg(RF_CH,0x0);
		WriteReg(RF_SETUP,0x0f);
		WriteReg(CONFIG,RegTab.byte[REG_CONFIG]);

    CE_SET(); // 启动发送
		/*
	  RRX_PW_P0  = ReadReg(RX_PW_P0);
	  REN_AA = ReadReg(EN_AA);
		REN_RXADDR = ReadReg(EN_RXADDR);
		RSETUP_RETR  = ReadReg(SETUP_RETR);
		RRF_CH  = ReadReg(RF_CH);
	  RRF_SETUP	= ReadReg(RF_SETUP);
	  RCONFIG	= ReadReg(CONFIG);
		*/
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
void NRF24L01_SendBuf(uint8_t *Buf) 
{
    CE_RESET();
	  WriteReg(CONFIG, 0x0E);
	  CE_SET();
	  HAL_Delay(1);
	  NRF24L01_SendTxBuf(Buf);
	  CE_RESET();
	  WriteReg(CONFIG, RegTab.byte[0]);
		CE_SET();
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
bool NRF24L01_GetRxBuf(uint8_t *Buf)
{
	uint8_t State;
	State = ReadReg(STATUS);  			
	WriteReg(STATUS, State);
	if(State & RX_OK)								
	{                                                       
		CE_SET();											
		Rf24_ReadPayload(RD_RX_PLOAD, Buf,32);
		WriteReg(FLUSH_RX, NOP);					
		CE_SET();				
		HAL_Delay(1);
		return 0; 
	}	   
	return 1;
}


/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
static void WriteReg(uint8_t Regaddr, uint8_t SendData)
{
	  uint8_t REDSTATUS;
    CSN_LOW();
    uint8_t cmd = W_REGISTER_ADDR | Regaddr;  // 写寄存器命令
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_TransmitReceive(&hspi1, &SendData,&REDSTATUS, 1, HAL_MAX_DELAY);
    CSN_HIGH();
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
static uint8_t ReadReg(uint8_t Regaddr) 
{
    uint8_t ReadData = 0, ReadData0;

    CSN_LOW();
	  HAL_SPI_TransmitReceive(&hspi1,&Regaddr,&ReadData0,1,HAL_MAX_DELAY);
  	HAL_SPI_TransmitReceive(&hspi1,&NOP,&ReadData,1,HAL_MAX_DELAY);
    CSN_HIGH();

    return ReadData;
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
static void Rf24_WritePayload(uint8_t Regaddr, uint8_t* Txdata, uint8_t length) 
{
    CSN_LOW();
    uint8_t cmd = W_REGISTER_ADDR | Regaddr;
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, Txdata, length, HAL_MAX_DELAY);
    CSN_HIGH();
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
static void Rf24_ReadPayload(uint8_t Regaddr, uint8_t* RxData, uint8_t length)
{
    CSN_LOW();
    uint8_t cmd = R_REGISTER_ADDR | Regaddr;
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, RxData, length, HAL_MAX_DELAY);
    CSN_HIGH();
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
static bool Check_Nf24(void) 
{
    uint8_t check_in_buf[5] = {0x12, 0x34, 0x56, 0x78, 0x90};
    uint8_t check_out_buf[5] = {0};

    CE_RESET();
    Rf24_WritePayload(0x10, check_in_buf, sizeof(check_in_buf));
    HAL_Delay(1);
    Rf24_ReadPayload(0x10, check_out_buf, 5);

    return (check_out_buf[0] == 0x12 && check_out_buf[1] == 0x34 &&
            check_out_buf[2] == 0x56 && check_out_buf[3] == 0x78 &&
            check_out_buf[4] == 0x90);
}

/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
static void flush_fifo(void)
{
    WriteReg(FLUSH_TX, NOP); // 清空TX FIFO
	  WriteReg(FLUSH_RX, NOP);
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
static uint8_t NRF24L01_SendTxBuf(uint8_t *Buf)
{	
	uint8_t State;
	CE_RESET();
	
  Rf24_WritePayload(WR_TX_PLOAD, Buf, 32);
	CE_SET();
  while(IRQ() == GPIO_PIN_SET);
  State =  ReadReg(STATUS);
	WriteReg(STATUS, State); 
	
 	if(State&MAX_TX)				 						
	{
		WriteReg(FLUSH_TX, NOP);
		return MAX_TX; 
	}
	if(State & TX_OK)	
	{
		return TX_OK;
	}
	return NOP;	

}

