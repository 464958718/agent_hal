#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct display display_t;

// 显示操作接口
typedef struct display_ops {
    int (*init)(display_t* disp);
    void (*deinit)(display_t* disp);
    int (*clear)(display_t* disp);
    int (*draw_string)(display_t* disp, int x, int y, const char* str);
    int (*draw_pixel)(display_t* disp, int x, int y, uint32_t color);
    int (*draw_rect)(display_t* disp, int x, int y, int w, int h, uint32_t color);
    int (*draw_circle)(display_t* disp, int x, int y, int radius, uint32_t color);
    int (*draw_image)(display_t* disp, int x, int y, const void* img_data, int w, int h);
    int (*set_brightness)(display_t* disp, int brightness);
    int (*flush)(display_t* disp);
} display_ops_t;

struct display {
    const display_ops_t* ops;
    void* impl;
    int width;
    int height;
    bool initialized;
};

// 显示类型
typedef enum {
    DISPLAY_TYPE_NONE,
    DISPLAY_TYPE_OLED,
    DISPLAY_TYPE_LCD,
    DISPLAY_TYPE_LVGL,
    DISPLAY_TYPE_EMOTE
} display_type_t;

// 公共 API
display_t* display_create(display_type_t type);
void display_destroy(display_t* disp);
int display_init(display_t* disp);
void display_deinit(display_t* disp);
int display_clear(display_t* disp);
int display_draw_string(display_t* disp, int x, int y, const char* str);
int display_draw_pixel(display_t* disp, int x, int y, uint32_t color);
int display_draw_rect(display_t* disp, int x, int y, int w, int h, uint32_t color);
int display_draw_circle(display_t* disp, int x, int y, int radius, uint32_t color);
int display_draw_image(display_t* disp, int x, int y, const void* img_data, int w, int h);
int display_set_brightness(display_t* disp, int brightness);
int display_flush(display_t* disp);

// 辅助函数
display_type_t display_get_type(const display_t* disp);
bool display_is_initialized(const display_t* disp);
int display_get_width(const display_t* disp);
int display_get_height(const display_t* disp);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H
