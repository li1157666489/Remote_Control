#include "ADCcalculate.h"

#define ADC_DMA_BUF_SIZE (NumChannel*16 ) // 通道×16次
uint16_t adcDmaBuf[ADC_DMA_BUF_SIZE];
uint16_t adcChannelAvg[NumChannel];

void Vidadc_init(void)
{

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcDmaBuf, ADC_DMA_BUF_SIZE);
}

uint16_t ADC_Read(uint32_t Channel)
{
        ADC_ChannelConfTypeDef sConfig = {0};
        sConfig.Channel = Channel;                                         /* 通道 */
        sConfig.Rank = 1;                              
        sConfig.SamplingTime = 5;                  /* 采样时间 */
        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)            
        {
                Error_Handler();
        }
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        return (uint16_t)HAL_ADC_GetValue(&hadc1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    uint32_t sum[NumChannel] = {0};
    // 通道，每通道16个数据
    for(uint8_t i=0; i<ADC_DMA_BUF_SIZE; i++)
    {
        sum[i%NumChannel] += adcDmaBuf[i];
    }
    for(uint8_t ch=0; ch<NumChannel; ch++)
    {
        adcChannelAvg[ch] = (uint16_t)(sum[ch]/16);
    }
}


uint16_t u16Adc_gatavg(Enu_AdcCha Cha)
{
   
  return adcChannelAvg[Cha];
}