#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// LED 类型
typedef enum {
    LED_TYPE_NONE,
    LED_TYPE_GPIO,
    LED_TYPE_RGB,
    LED_TYPE_WS2812,
    LED_TYPE_CIRCULAR
} led_type_t;

// LED 颜色
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_color_t;

// LED 效果
typedef enum {
    LED_EFFECT_OFF,
    LED_EFFECT_ON,
    LED_EFFECT_BREATHE,
    LED_EFFECT_BLINK,
    LED_EFFECT_RAINBOW,
    LED_EFFECT_CUSTOM
} led_effect_t;

// LED 控制结构
typedef struct led_control led_control_t;

// LED 控制 API
led_control_t* led_control_create(led_type_t type);
void led_control_destroy(led_control_t* led);
int led_control_init(led_control_t* led);
void led_control_deinit(led_control_t* led);

// LED 操作
int led_control_set_color(led_control_t* led, const led_color_t* color);
int led_control_set_brightness(led_control_t* led, uint8_t brightness);
int led_control_set_effect(led_control_t* led, led_effect_t effect);
int led_control_clear(led_control_t* led);

// 动画控制
int led_control_start_animation(led_control_t* led, uint32_t duration_ms);
int led_control_stop_animation(led_control_t* led);

#ifdef __cplusplus
}
#endif

#endif // LED_CONTROL_H
