#ifndef __ADCCALCULSTE_H
#define __ADCCALCULSTE_H
#include "myproject.h"
typedef enum
{
  RollChannel     =0,
	PitchChannel,
	ThrottleChannel,
	YawChannel,
	NumChannel
	
}Enu_AdcCha;

uint16_t ADC_Read(uint32_t Channel);
void Vidadc_init(void);
uint16_t u16Adc_gatavg(Enu_AdcCha Cha);

#endif
