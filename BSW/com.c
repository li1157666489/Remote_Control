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
  uint8_t byte[4U];
	
}txdatatab;
txdatatab txtab;

void Vidcom_init(void)
{
txtab.u16txdata.Frame_header = 0x5a;

}
void Vidcom_txdata(void)
{
	txtab.u16txdata.Roll = u16Adc_gatavg(RollChannel);
  txtab.u16txdata.Pitch = u16Adc_gatavg(PitchChannel);
	//NRF24L01_SendBuf(txtab.byte);
}