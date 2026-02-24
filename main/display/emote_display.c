/**
 * @file emote_display.c
 * @brief Emote Display Implementation
 *
 * This module provides C implementation for displaying emotes/emojis
 * on displays. It implements the display_ops_t interface.
 */

#include "emote_display.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAG "emote_display"

// 表情数据占位符 - 实际实现需要替换为真实的表情图像数据
// 这里使用简单的字符串映射到表情名称
static const char* emote_names[EMOTE_MAX] = {
    "default",     // EMOTE_DEFAULT
    "listening",  // EMOTE_LISTENING
    "speaking",   // EMOTE_SPEAKING
    "happy",      // EMOTE_HAPPY
    "sad",        // EMOTE_SAD
    "angry",      // EMOTE_ANGRY
    "surprised",  // EMOTE_SURPRISED
    "sleeping",   // EMOTE_SLEEPING
    "confused",   // EMOTE_CONFUSED
    "wink",       // EMOTE_WINK
    "love",       // EMOTE_LOVE
};

// 私有数据结构
typedef struct {
    emote_config_t config;
    emote_type_t current_emote;
    char current_emoji[64];
    bool is_custom;
} emote_private_t;

// 函数原型声明
static int emote_display_init(display_t* disp);
static void emote_display_deinit(display_t* disp);
static int emote_display_clear(display_t* disp);
static int emote_display_draw_string(display_t* disp, int x, int y, const char* str);
static int emote_display_draw_pixel(display_t* disp, int x, int y, uint32_t color);
static int emote_display_draw_rect(display_t* disp, int x, int y, int w, int h, uint32_t color);
static int emote_display_draw_circle(display_t* disp, int x, int y, int radius, uint32_t color);
static int emote_display_draw_image(display_t* disp, int x, int y, const void* img_data, int w, int h);
static int emote_display_set_brightness(display_t* disp, int brightness);
static int emote_display_flush(display_t* disp);

// 显示操作接口
static const display_ops_t emote_ops = {
    .init = emote_display_init,
    .deinit = emote_display_deinit,
    .clear = emote_display_clear,
    .draw_string = emote_display_draw_string,
    .draw_pixel = emote_display_draw_pixel,
    .draw_rect = emote_display_draw_rect,
    .draw_circle = emote_display_draw_circle,
    .draw_image = emote_display_draw_image,
    .set_brightness = emote_display_set_brightness,
    .flush = emote_display_flush,
};

/**
 * @brief Create an emote display instance
 */
display_t* emote_display_create(const emote_config_t* config) {
    if (config == NULL) {
        return NULL;
    }

    display_t* disp = (display_t*)malloc(sizeof(display_t));
    if (disp == NULL) {
        return NULL;
    }

    emote_private_t* priv = (emote_private_t*)malloc(sizeof(emote_private_t));
    if (priv == NULL) {
        free(disp);
        return NULL;
    }

    // 复制配置
    priv->config.display = config->display;
    priv->config.x = config->x;
    priv->config.y = config->y;
    priv->config.size = config->size;
    priv->current_emote = EMOTE_DEFAULT;
    priv->is_custom = false;
    memset(priv->current_emoji, 0, sizeof(priv->current_emoji));

    // 初始化 display 结构体
    disp->ops = &emote_ops;
    disp->impl = priv;
    disp->width = config->display ? config->display->width : 0;
    disp->height = config->display ? config->display->height : 0;
    disp->initialized = false;

    return disp;
}

/**
 * @brief Initialize emote display
 */
static int emote_display_init(display_t* disp) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 初始化底层显示设备
    if (priv->config.display != NULL) {
        if (display_init(priv->config.display) != 0) {
            return -1;
        }
    }

    disp->initialized = true;
    return 0;
}

/**
 * @brief Deinitialize emote display
 */
static void emote_display_deinit(display_t* disp) {
    if (disp == NULL || disp->impl == NULL) {
        return;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 反初始化底层显示设备
    if (priv->config.display != NULL) {
        display_deinit(priv->config.display);
    }

    // 释放私有数据
    free(priv);
    disp->impl = NULL;
    disp->initialized = false;
}

/**
 * @brief Clear the emote display
 */
static int emote_display_clear(display_t* disp) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 清除底层显示
    if (priv->config.display != NULL) {
        return display_clear(priv->config.display);
    }

    return 0;
}

/**
 * @brief Draw a string on the emote display
 */
static int emote_display_draw_string(display_t* disp, int x, int y, const char* str) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 委托给底层显示
    if (priv->config.display != NULL) {
        return display_draw_string(priv->config.display, x, y, str);
    }

    return -1;
}

/**
 * @brief Draw a pixel on the emote display
 */
static int emote_display_draw_pixel(display_t* disp, int x, int y, uint32_t color) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 委托给底层显示
    if (priv->config.display != NULL) {
        return display_draw_pixel(priv->config.display, x, y, color);
    }

    return -1;
}

/**
 * @brief Draw a rectangle on the emote display
 */
static int emote_display_draw_rect(display_t* disp, int x, int y, int w, int h, uint32_t color) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 委托给底层显示
    if (priv->config.display != NULL) {
        return display_draw_rect(priv->config.display, x, y, w, h, color);
    }

    return -1;
}

/**
 * @brief Draw a circle on the emote display
 */
static int emote_display_draw_circle(display_t* disp, int x, int y, int radius, uint32_t color) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 委托给底层显示
    if (priv->config.display != NULL) {
        return display_draw_circle(priv->config.display, x, y, radius, color);
    }

    return -1;
}

/**
 * @brief Draw an image on the emote display
 */
static int emote_display_draw_image(display_t* disp, int x, int y, const void* img_data, int w, int h) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 委托给底层显示
    if (priv->config.display != NULL) {
        return display_draw_image(priv->config.display, x, y, img_data, w, h);
    }

    return -1;
}

/**
 * @brief Set display brightness
 */
static int emote_display_set_brightness(display_t* disp, int brightness) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 委托给底层显示
    if (priv->config.display != NULL) {
        return display_set_brightness(priv->config.display, brightness);
    }

    return -1;
}

/**
 * @brief Flush changes to display
 */
static int emote_display_flush(display_t* disp) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 委托给底层显示
    if (priv->config.display != NULL) {
        return display_flush(priv->config.display);
    }

    return -1;
}

/**
 * @brief Show specified emote on display
 *
 * @param disp Display instance
 * @param emote Emote type to display
 * @return 0 on success, -1 on failure
 */
int emote_display_show(display_t* disp, emote_type_t emote) {
    if (disp == NULL || disp->impl == NULL) {
        return -1;
    }

    if (emote >= EMOTE_MAX) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 更新当前表情
    priv->current_emote = emote;
    priv->is_custom = false;

    // TODO: 实现实际的表情显示逻辑
    // 这里可以使用表情名称来加载和显示对应的图像
    // 例如：emote_names[emote] 可以用来查找对应的表情图像数据

    return 0;
}

/**
 * @brief Show custom emoji on display
 *
 * @param disp Display instance
 * @param emoji Emoji string to display
 * @return 0 on success, -1 on failure
 */
int emote_display_show_custom(display_t* disp, const char* emoji) {
    if (disp == NULL || disp->impl == NULL || emoji == NULL) {
        return -1;
    }

    emote_private_t* priv = (emote_private_t*)disp->impl;

    // 更新当前自定义表情
    priv->is_custom = true;
    strncpy(priv->current_emoji, emoji, sizeof(priv->current_emoji) - 1);
    priv->current_emoji[sizeof(priv->current_emoji) - 1] = '\0';

    // TODO: 实现实际的自定义表情显示逻辑
    // 这里可以使用 emoji 字符串来查找和显示对应的图像

    return 0;
}
