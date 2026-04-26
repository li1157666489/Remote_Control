#include "myproject.h"
#include "myui.h"
#define TASKNUM_MAX 4U

typedef  struct
{
    uint32_t Current_Timer;
    uint32_t Expected_Timer;
    void (*task) (void);
    /* data */
}typtask;
static void TaskScheduling_1ms(void);
static void TaskScheduling_2ms(void);
static void TaskScheduling_10ms(void);
static void TaskScheduling_50ms(void);
static typtask Air_tackList[TASKNUM_MAX]=
{
	  {
        0,
        1,
        TaskScheduling_1ms,
    },
    {
        0,
        2,
        TaskScheduling_2ms,
    },
    {
        0,
        10,
        TaskScheduling_10ms,
    },
		{
        0,
        50,
        TaskScheduling_50ms,
    }
};
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
void AirTack_Init(void)
{


}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
void AirTack_Tick(void)
{
    uint8_t u8Tack_index = 0u;
    for(u8Tack_index =0u;u8Tack_index < TASKNUM_MAX;u8Tack_index++)
    {
        Air_tackList[u8Tack_index].Current_Timer++;
    }
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
void AirTack_Scheduling(void)
{
    uint8_t u8Tack_index = 0u;
    for(u8Tack_index =0u;u8Tack_index < TASKNUM_MAX;u8Tack_index++)
    {
        if(Air_tackList[u8Tack_index].Current_Timer >= Air_tackList[u8Tack_index].Expected_Timer)
        {
            Air_tackList[u8Tack_index].Current_Timer = 0U;
            Air_tackList[u8Tack_index].task();
        }
    }
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
static void TaskScheduling_1ms(void)
{
HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_8);
//HAL_ADC_Start(&hadc1);
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
uint16_t ADCval[2] = {0,0};
static void TaskScheduling_2ms(void)
{
    /* LVGL时钟更新,每2ms调用一次 */
    lv_tick_inc(2);
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
static void TaskScheduling_10ms(void)
{
uint8_t Buf[5] = {0x11,0x22,0x55};
static uint32_t con = 0;
con++;

	/* LVGL任务处理，添加防重入保护和时间限制 */
	static volatile bool lvgl_handler_running = false;
	if (!lvgl_handler_running) {
		uint32_t start_tick = HAL_GetTick();
		lvgl_handler_running = true;
		lv_timer_handler();
		lvgl_handler_running = false;

		/* 监控执行时间，超过5ms则可能影响其他任务 */
		if (HAL_GetTick() - start_tick > 5) {
			/* LVGL处理时间过长，可以考虑优化 */
			HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_2);  /* 可选：LED指示性能问题 */
		}
	}

	if(con > 100)
	{
		HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_2);
		//NRF24L01_SendBuf(Buf);
		con = 0;

	}
	//ADCval[0]= 	ADC_Read(0);
	//ADCval[1]= 	ADC_Read(1);
}
/**************************************************************************** 
- Function :        
- Input:            
- Output:           
- Return:           
- Others :          
******************************************************************************/ 
uint8_t Buft[50] ={11,22,33,44,55,66,77,88,99,};
static void TaskScheduling_50ms(void)
{
	//myui_update_status(12, 1, 10);
	myui_update_control(ADCval[0], ADCval[1], ADCval[0], ADCval[1]);
	Vidcom_txdata();
}
