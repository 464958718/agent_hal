// lcd_display.h
#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

// LCD 类型
typedef enum {
    LCD_TYPE_SPI,
    LCD_TYPE_RGB,
    LCD_TYPE_MIPI
} lcd_type_t;

// LCD 配置
typedef struct {
    lcd_type_t type;
    int width;
    int height;
    int clk_pin;
    int data_pin;
    int cs_pin;
    int dc_pin;
    int rst_pin;
    int backlight_pin;
    bool flip_xy;
    bool color_swap;
} lcd_config_t;

// 创建 LCD 显示实例
display_t* lcd_display_create(const lcd_config_t* config);

#ifdef __cplusplus
}
#endif

#endif // LCD_DISPLAY_H
