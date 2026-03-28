#include "ADC.h"



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