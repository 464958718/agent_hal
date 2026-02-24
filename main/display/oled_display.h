#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

// OLED 配置
typedef struct {
    int width;
    int height;
    int scl_pin;
    int sda_pin;
    int rst_pin;
    bool flip_xy;
} oled_config_t;

// 创建 OLED 显示实例
display_t* oled_display_create(const oled_config_t* config);

#ifdef __cplusplus
}
#endif

#endif // OLED_DISPLAY_H
