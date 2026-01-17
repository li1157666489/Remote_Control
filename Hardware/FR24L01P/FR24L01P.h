#ifndef _FR24L01P_H
#define _FR24L01P_H
#include <stdbool.h>
#include "stdint.h"
#include "spi.h"
#include "gpio.h"
typedef enum
{

    FR24_RX_Mode,
    FR24_TX_Mode,

}tstFr24State;

typedef enum
{
    REG_CONFIG = 0U,
	  REG_RF_CH,
	  REG_SETUP_RETR,
	  REG_STATUS,
	
	  REG_NUM
}TstReg;

#define ConfigReserved     0x0U
#define EN_RX_INTERRUPT    0x1U
#define DIS_RX_INTERRUPT   0x0U
#define EN_TX_INTERRUPT    0x1U
#define DIS_TX_INTERRUPT   0x0U
#define EN_MAX_INTERRUPT   0x0U
#define DIS_MAX_INTERRUPT  0x1U
#define EN_CRC_V           0x1U
#define DIS_CRC            0x0U
#define CRC_1BYTE          0x0U
#define CRC_2BYTE          0x1U
#define POWER_UP           0x1U  
#define POWER_DOWN         0x0U 
#define PRX                0x1U  
#define PTX                0x0U 

#define RF_CH_V            0x2U  

#define Retransmit_Delay_250US  0x0U 
#define Retransmit_Delay_500US  0x1U
#define Retransmit_Delay_750US  0x2U
/*SETUP_RETR*/
#define Retransmit_Count_0      0x0U 
#define Retransmit_Count_1      0x1U
#define Retransmit_Count_2      0x2U
#define Retransmit_Count_3      0x3U 
#define Retransmit_Count_4      0x4U
#define Retransmit_Count_5      0x5U
/* RF_SETUP*/
#define Enables_continuous_carrier 0x1U
#define RF_Reserved_V              0x0U
#define RF_Data_Rate               0x0U
#define Speed_Data_1M              0x0U
#define Speed_Data_2M              0x1U            
#define Speed_Data_250K            0x2U         
#define RF_Output_Power_18dB       0x0U
#define RF_Output_Power_12dB       0x1U
#define RF_Output_Power_6dB        0x2U
#define RF_Output_Power_0dB        0x3U

/**********  NRF24L01REGADDR   *************/
#define CONFIG          0x00                           
#define EN_AA           0x01  
#define EN_RXADDR       0x02  
#define SETUP_AW        0x03  
#define SETUP_RETR      0x04  
#define RF_CH           0x05  
#define RF_SETUP        0x06  
#define STATUS          0x07  
#define OBSERVE_TX      0x08  
#define CD              0x09  
#define RX_ADDR_P0      0x0A  
#define RX_ADDR_P1      0x0B  
#define RX_ADDR_P2      0x0C  
#define RX_ADDR_P3      0x0D  
#define RX_ADDR_P4      0x0E  
#define RX_ADDR_P5      0x0F  
#define TX_ADDR         0x10  
#define RX_PW_P0        0x11   
#define RX_PW_P1        0x12   
#define RX_PW_P2        0x13   
#define RX_PW_P3        0x14   
#define RX_PW_P4        0x15  
#define RX_PW_P5        0x16  
#define FIFO_STATUS     0x17
/**********  NRF24L01REGCMD  ***********/
#define nRF_READ_REG        0x00 
#define nRF_WRITE_REG       0x20 
#define RD_RX_PLOAD     0x61  
#define WR_TX_PLOAD     0xA0  
#define FLUSH_TX        0xE1  
#define FLUSH_RX        0xE2  
#define REUSE_TX_PL     0xE3  
/******   STATUS寄存器bit位定义      *******/
#define MAX_TX  	0x10  	  //达到最大发送次数中断
#define TX_OK   	0x20  	  //TX发送完成中断
#define RX_OK   	0x40  	  //接收到数据中断

void FR24L019P_init(tstFr24State Mode);
void NRF24L01_SendBuf(uint8_t *Buf) ;
#define IRQ()      HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) 
#endif
