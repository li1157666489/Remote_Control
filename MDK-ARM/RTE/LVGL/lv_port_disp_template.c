/**
 * @file lv_port_disp_template.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include <stdbool.h>
#include "lvgl.h"
#include "st7789v.h"
/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    280
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_VER_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    240
#endif

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
disp_init(); lv_display_t *disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES); lv_display_set_flush_cb(disp, disp_flush); static lv_color_t buf1[MY_DISP_HOR_RES * 20]; lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

//    /* Example 2
//     * Two buffers for partial rendering
//     * In flush_cb DMA or similar hardware should be used to update the display in the background.*/
//    LV_ATTRIBUTE_MEM_ALIGN
//    static uint8_t buf_2_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];

//    LV_ATTRIBUTE_MEM_ALIGN
//    static uint8_t buf_2_2[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];
//    lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

//    /* Example 3
//     * Two buffers screen sized buffer for double buffering.
//     * Both LV_DISPLAY_RENDER_MODE_DIRECT and LV_DISPLAY_RENDER_MODE_FULL works, see their comments*/
//    LV_ATTRIBUTE_MEM_ALIGN
//    static uint8_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];

//    LV_ATTRIBUTE_MEM_ALIGN
//    static uint8_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];
//    lv_display_set_buffers(disp, buf_3_1, buf_3_2, sizeof(buf_3_1), LV_DISPLAY_RENDER_MODE_DIRECT);

lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
 uint16_t   TEXT[1];
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    /* 参数检查 */
    if (area->x2 < area->x1 || area->y2 < area->y1 || px_map == NULL) {
        lv_display_flush_ready(disp_drv);
        return;
    }

	uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

    ST7789V_SetAddressWindow(area->x1, area->y1,
                             area->x2, area->y2);

    uint32_t pixel_count = w * h;

    ST7789V_Select();    // CS = 0
    ST7789V_DC_Set();    // DC = 1

    /* 使用批量传输,提高速度,设置合理超时时间 */
    uint32_t total_bytes = pixel_count * 2;  /* RGB565: 2字节/像素 */
    HAL_SPI_Transmit(&hspi2, px_map, total_bytes, 100);  /* 超时100ms */

    ST7789V_UnSelect();  // CS = 1

    lv_display_flush_ready(disp_drv);

//uint16_t val = (uint16_t)px_map[1] |
//               ((uint16_t)px_map[0] << 8);
//    if(disp_flush_enabled) {
//        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

//        int32_t x;
//        int32_t y;
//        for(y = area->y1; y <= area->y2; y++) {
//            for(x = area->x1; x <= area->x2; x++) {
//                /*Put a pixel to the display. For example:*/
//                /*put_px(x, y, *px_map)*/
//							  LCD_DrawPoint(x, y, val);
//                px_map++;
//							
//            }
//        }
//    }

//    /*IMPORTANT!!!
//     *Inform the graphics library that you are ready with the flushing*/
//    lv_display_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
