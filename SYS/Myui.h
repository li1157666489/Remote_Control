/**
 * @file           : myui.h
 * @brief          : 虚拟四轴无人机遥控器界面头文件
 * @date           : 2026-03-28
 */

#ifndef __MYUI_H
#define __MYUI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化UI界面 */
void myui_init(void);

/* 更新遥控器控制数据（油门、偏航、俯仰、横滚） */
void myui_update_control(int16_t throttle, int16_t yaw, int16_t pitch, int16_t roll);

/* 更新状态信息（电池、信号、速度） */
void myui_update_status(uint8_t battery, int8_t signal, uint8_t speed);

/* 获取当前遥控器状态 */
void myui_get_state(int16_t *throttle, int16_t *yaw, int16_t *pitch, int16_t *roll);

/* 清理UI资源 */
void myui_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __MYUI_H */
