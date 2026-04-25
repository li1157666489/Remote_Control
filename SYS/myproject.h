#include "stdio.h"
#include "main.h"
#include "adc.h"
#include <stdint.h>
#include "FR24L01P.h"
#include "lvgl.h"
#include "demos/lv_demos.h"
#include "task.h"
#include "st7789v.h"
#include "lv_port_disp_template.h"
#include "myui.h"
#include "stm32f4xx_hal_def.h"
#include "com.h"
#include "ADCcalculate.h"

#define unsigned int uint16_t 