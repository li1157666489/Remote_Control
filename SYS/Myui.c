/**
 * @file           : myui.c
 * @brief          : 虚拟四轴无人机遥控器界面实现
 * @date           : 2026-03-28
 */

#include "myui.h"
#include <lvgl.h>

/* UI对象 */
static lv_obj_t *main_screen = NULL;
static lv_obj_t *joystick_left = NULL;
static lv_obj_t *joystick_right = NULL;
static lv_obj_t *joystick_knob_left = NULL;
static lv_obj_t *joystick_knob_right = NULL;
static lv_obj_t *speed_slider = NULL;
static lv_obj_t *battery_bar = NULL;
static lv_obj_t *battery_label = NULL;
static lv_obj_t *signal_label = NULL;
static lv_obj_t *quad_preview = NULL;
static lv_obj_t *rotor_anim[4] = {NULL};

/* 遥控器状态 */
typedef struct {
    int16_t throttle;     // 油门 0-1000
    int16_t yaw;          // 偏航 -500~500
    int16_t pitch;        // 俯仰 -500~500
    int16_t roll;         // 横滚 -500~500
    uint8_t speed_level;  // 速度等级 0-3
    uint8_t battery;      // 电池 0-100%
    int8_t signal;        // 信号强度 -100~0dBm
} QuadState_t;

static QuadState_t quad_state = {0};

/**
 * @brief 创建摇杆控件
 */
static void create_joystick(lv_obj_t *parent, lv_obj_t **joystick, lv_obj_t **knob, lv_coord_t x, lv_coord_t y)
{
    /* 摇杆背景 */
    *joystick = lv_obj_create(parent);
    lv_obj_set_size(*joystick, 120, 120);
    lv_obj_set_pos(*joystick, x, y);
    lv_obj_set_style_bg_color(*joystick, lv_color_hex(0x2D3748), 0);
    lv_obj_set_style_border_width(*joystick, 2, 0);
    lv_obj_set_style_border_color(*joystick, lv_color_hex(0x4A5568), 0);
    lv_obj_set_style_radius(*joystick, 60, 0);
    lv_obj_set_style_shadow_width(*joystick, 5, 0);
    lv_obj_set_style_shadow_color(*joystick, lv_color_hex(0x000000), 0);
    
    /* 摇杆手柄 */
    *knob = lv_obj_create(*joystick);
    lv_obj_set_size(*knob, 50, 50);
    lv_obj_align(*knob, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(*knob, lv_color_hex(0x48BB78), 0);
    lv_obj_set_style_border_width(*knob, 0, 0);
    lv_obj_set_style_radius(*knob, 25, 0);
    lv_obj_set_style_shadow_width(*knob, 5, 0);
    lv_obj_set_style_shadow_color(*knob, lv_color_hex(0x276749), 0);
}

/**
 * @brief 创建旋翼动画对象
 */
static void create_rotor_anim(lv_obj_t *parent, int index, lv_coord_t x, lv_coord_t y)
{
    rotor_anim[index] = lv_arc_create(parent);
    lv_obj_set_size(rotor_anim[index], 60, 60);
    lv_obj_set_pos(rotor_anim[index], x, y);
    
    lv_arc_set_range(rotor_anim[index], 0, 360);
    lv_arc_set_value(rotor_anim[index], 0);
    lv_arc_set_bg_angles(rotor_anim[index], 0, 360);
    
    /* 设置旋翼样式 */
    lv_obj_set_style_arc_color(rotor_anim[index], lv_color_hex(0x4299E1), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(rotor_anim[index], 8, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(rotor_anim[index], true, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(rotor_anim[index], lv_color_hex(0x2D3748), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(rotor_anim[index], LV_OPA_50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(rotor_anim[index], 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}

/**
 * @brief 更新电池显示
 */
static void update_battery_display(void)
{
    /* 电池容器 */
    lv_obj_t *battery_container = lv_obj_create(main_screen);
    lv_obj_set_size(battery_container, 120, 30);
    lv_obj_align(battery_container, LV_ALIGN_TOP_RIGHT, -20, 20);
    lv_obj_set_style_bg_color(battery_container, lv_color_hex(0x2D3748), 0);
    lv_obj_set_style_border_width(battery_container, 2, 0);
    lv_obj_set_style_border_color(battery_container, lv_color_hex(0x4A5568), 0);
    lv_obj_set_style_radius(battery_container, 5, 0);
    
    /* 电池进度条 */
    battery_bar = lv_bar_create(battery_container);
    lv_obj_set_size(battery_bar, 100, 20);
    lv_obj_align(battery_bar, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_range(battery_bar, 0, 100);
    lv_bar_set_value(battery_bar, quad_state.battery, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(battery_bar, lv_color_hex(0x1F2937), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(battery_bar, lv_color_hex(0x48BB78), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(battery_bar, 3, 0);
    
    /* 电池标签 */
    battery_label = lv_label_create(battery_container);
    lv_label_set_text_fmt(battery_label, "%d%%", quad_state.battery);
    lv_obj_align(battery_label, LV_ALIGN_TOP_RIGHT, -10, 5);
    lv_obj_set_style_text_color(battery_label, lv_color_hex(0x48BB78), 0);
}

/**
 * @brief 更新信号显示
 */
static void update_signal_display(void)
{
    /* 信号强度图标 */
    signal_label = lv_label_create(main_screen);
    lv_obj_align(signal_label, LV_ALIGN_TOP_RIGHT, -20, 60);
    
    /* 绘制信号条 */
    int bars = (quad_state.signal + 100) / 20;  // -100~0dBm -> 0~5 bars
    if (bars < 0) bars = 0;
    if (bars > 5) bars = 5;
    
    char signal_str[20] = "";
    for (int i = 0; i < bars; i++) {
        strcat(signal_str, "▓");
    }
    for (int i = bars; i < 5; i++) {
        strcat(signal_str, "?");
    }
    
    lv_label_set_text_fmt(signal_label, "%s %ddBm", signal_str, quad_state.signal);
    lv_obj_set_style_text_color(signal_label, lv_color_hex(0x4299E1), 0);
}

/**
 * @brief 创建速度控制
 */
static void create_speed_control(void)
{
    /* 速度标签 */
    lv_obj_t *speed_title = lv_label_create(main_screen);
    lv_label_set_text(speed_title, "SPEED");
    lv_obj_align(speed_title, LV_ALIGN_TOP_LEFT, 20, 200);
    lv_obj_set_style_text_color(speed_title, lv_color_hex(0xA0AEC0), 0);
    
    /* 速度滑块 */
    speed_slider = lv_slider_create(main_screen);
    lv_obj_set_width(speed_slider, 150);
    lv_obj_align(speed_slider, LV_ALIGN_TOP_LEFT, 20, 220);
    lv_slider_set_range(speed_slider, 1, 4);
    lv_slider_set_value(speed_slider, quad_state.speed_level + 1, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(speed_slider, lv_color_hex(0x2D3748), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(speed_slider, lv_color_hex(0xF6AD55), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(speed_slider, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(speed_slider, 5, 0);
}

/**
 * @brief 创建四旋翼预览
 */
static void create_quad_preview(void)
{
    /* 四旋翼预览容器 */
    quad_preview = lv_obj_create(main_screen);
    lv_obj_set_size(quad_preview, 200, 200);
    lv_obj_align(quad_preview, LV_ALIGN_CENTER, 0, -50);
    lv_obj_set_style_bg_color(quad_preview, lv_color_hex(0x1A202C), 0);
    lv_obj_set_style_border_width(quad_preview, 2, 0);
    lv_obj_set_style_border_color(quad_preview, lv_color_hex(0x4A5568), 0);
    lv_obj_set_style_radius(quad_preview, 100, 0);
    
    /* 四个旋翼位置 */
    create_rotor_anim(quad_preview, 0, 20, 20);   // 左前
    create_rotor_anim(quad_preview, 1, 120, 20);  // 右前
    create_rotor_anim(quad_preview, 2, 20, 120);  // 左后
    create_rotor_anim(quad_preview, 3, 120, 120); // 右后
    
    /* 机身中心 */
    lv_obj_t *body = lv_obj_create(quad_preview);
    lv_obj_set_size(body, 60, 60);
    lv_obj_align(body, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(body, lv_color_hex(0x4299E1), 0);
    lv_obj_set_style_border_width(body, 0, 0);
    lv_obj_set_style_radius(body, 30, 0);
}

/**
 * @brief 初始化主界面
 */
void myui_init(void)
{
    /* 创建主屏幕 */
    main_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(main_screen, lv_color_hex(0x171923), 0);
    
    /* 标题 */
    lv_obj_t *title = lv_label_create(main_screen);
    lv_label_set_text(title, "TEXT  MODE");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    
    /* 左摇杆 - 控制油门和偏航 */
    create_joystick(main_screen, &joystick_left, &joystick_knob_left, 20, 80);
    
    /* 右摇杆 - 控制俯仰和横滚 */
    create_joystick(main_screen, &joystick_right, &joystick_knob_right, 160, 80);
    
    /* 四旋翼预览 */
  //  create_quad_preview();
    
    /* 速度控制 */
 //   create_speed_control();
    
    /* 电池和信号显示 */
  //  update_battery_display();
   // update_signal_display();
    
    /* 加载屏幕 */
    lv_scr_load(main_screen);
}

/**
 * @brief 更新遥控器控制数据
 */
void myui_update_control(int16_t throttle, int16_t yaw, int16_t pitch, int16_t roll)
{
    quad_state.throttle = throttle;
    quad_state.yaw = yaw;
    quad_state.pitch = pitch;
    quad_state.roll = roll;
    
    /* 更新左摇杆位置（油门垂直，偏航水平） */
    int16_t throttle_pos = -throttle * 35 / 1000;  // 上下
    int16_t yaw_pos = yaw * 35 / 500;              // 左右
    lv_obj_align(joystick_knob_left, LV_ALIGN_CENTER, yaw_pos, throttle_pos);
    
    /* 更新右摇杆位置（俯仰垂直，横滚水平） */
    int16_t pitch_pos = -pitch * 35 / 500;         // 上下
    int16_t roll_pos = roll * 35 / 500;           // 左右
    lv_obj_align(joystick_knob_right, LV_ALIGN_CENTER, roll_pos, pitch_pos);
    
    /* 更新旋翼动画速度 - 基于油门 */
    int16_t rotor_speed = throttle * 36 / 1000;    // 0-36度
    for (int i = 0; i < 4; i++) {
        int16_t current_value = lv_arc_get_value(rotor_anim[i]);
        lv_arc_set_value(rotor_anim[i], (current_value + rotor_speed) % 360);
    }
}

/**
 * @brief 更新状态信息
 */
void myui_update_status(uint8_t battery, int8_t signal, uint8_t speed)
{
    quad_state.battery = battery;
    quad_state.signal = signal;
    quad_state.speed_level = speed;
    
    /* 更新电池 */
    if (battery_bar) {
        lv_bar_set_value(battery_bar, battery, LV_ANIM_ON);
    }
    if (battery_label) {
        lv_label_set_text_fmt(battery_label, "%d%%", battery);
    }
    
    /* 更新信号 */
    if (signal_label) {
        int bars = (signal + 100) / 20;
        if (bars < 0) bars = 0;
        if (bars > 5) bars = 5;
        
        char signal_str[20] = "";
        for (int i = 0; i < bars; i++) {
            strcat(signal_str, "▓");
        }
        for (int i = bars; i < 5; i++) {
            strcat(signal_str, "?");
        }
        
        lv_label_set_text_fmt(signal_label, "%s %ddBm", signal_str, signal);
    }
    
    /* 更新速度等级 */
    if (speed_slider) {
        lv_slider_set_value(speed_slider, speed + 1, LV_ANIM_ON);
    }
}

/**
 * @brief 获取当前遥控器状态
 */
void myui_get_state(int16_t *throttle, int16_t *yaw, int16_t *pitch, int16_t *roll)
{
    if (throttle) *throttle = quad_state.throttle;
    if (yaw) *yaw = quad_state.yaw;
    if (pitch) *pitch = quad_state.pitch;
    if (roll) *roll = quad_state.roll;
}

/**
 * @brief 清理UI资源
 */
void myui_deinit(void)
{
    if (main_screen) {
        lv_obj_del(main_screen);
        main_screen = NULL;
    }
}
