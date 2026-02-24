#ifndef EMOTE_DISPLAY_H
#define EMOTE_DISPLAY_H

#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

// 表情类型
typedef enum {
    EMOTE_DEFAULT = 0,
    EMOTE_LISTENING,
    EMOTE_SPEAKING,
    EMOTE_HAPPY,
    EMOTE_SAD,
    EMOTE_ANGRY,
    EMOTE_SURPRISED,
    EMOTE_SLEEPING,
    EMOTE_CONFUSED,
    EMOTE_WINK,
    EMOTE_LOVE,
    EMOTE_MAX
} emote_type_t;

// 表情配置
typedef struct {
    display_t* display;
    int x;
    int y;
    int size;
} emote_config_t;

// 创建表情显示实例
display_t* emote_display_create(const emote_config_t* config);

// 显示指定表情
int emote_display_show(display_t* disp, emote_type_t emote);

// 显示自定义表情
int emote_display_show_custom(display_t* disp, const char* emoji);

#ifdef __cplusplus
}
#endif

#endif // EMOTE_DISPLAY_H
