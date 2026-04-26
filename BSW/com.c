#include "com.h"
#include "myproject.h"
typedef struct
{
	  uint8_t  Frame_header   :8;
		bool Motosw             :1;
	  bool Lamp               :1;
		bool reserved1          :1;
		bool reserved2          :1;
		bool reserved3          :1;
		bool reserved4          :1;
		bool reserved5          :1;
		bool reserved6          :1;
	/*********L*****/
	  uint16_t Roll           :16;
		uint16_t Pitch          :16;
	/*********R*****/
	  uint16_t Throttle       :16;
	  uint16_t Yaw            :16;
	
	 

}Tx_Data;

typedef union
{
  Tx_Data u16txdata;
  uint8_t byte[sizeof(Tx_Data)];
	
}txdatatab;
static uint16_t ADC_ToRC(uint16_t adc_val);
txdatatab txtab;

void Vidcom_init(void)
{
txtab.u16txdata.Frame_header = 0x5a;
}
void Vidcom_txdata(void)
{
	txtab.u16txdata.Roll = ADC_ToRC(u16Adc_gatavg(RollChannel));
  txtab.u16txdata.Pitch = ADC_ToRC(u16Adc_gatavg(PitchChannel));
	txtab.u16txdata.Throttle = ADC_ToRC(u16Adc_gatavg(ThrottleChannel));
  txtab.u16txdata.Yaw = ADC_ToRC(u16Adc_gatavg(YawChannel));
	
	NRF24L01_SendBuf(txtab.byte);
}

static uint16_t ADC_ToRC(uint16_t adc_val)
{
    // 安全限幅（防止ADC超出范围）
    if(adc_val > 4095) adc_val = 4095;
    if(adc_val < 0)    adc_val = 0;

    // 核心公式：1000 + (adc_val * 1000) / 4095
    return 1000 + (uint16_t)((uint32_t)adc_val * 1000 / 4095);
}