#include "display.h"
#include "log_utils.h"
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "display"

static int default_init(display_t* disp) {
    (void)disp;
    return -1;
}

static void default_deinit(display_t* disp) {
    (void)disp;
}

static int default_clear(display_t* disp) {
    (void)disp;
    return -1;
}

static int default_draw_string(display_t* disp, int x, int y, const char* str) {
    (void)disp;
    (void)x;
    (void)y;
    (void)str;
    return -1;
}

static int default_draw_pixel(display_t* disp, int x, int y, uint32_t color) {
    (void)disp;
    (void)x;
    (void)y;
    (void)color;
    return -1;
}

static int default_draw_rect(display_t* disp, int x, int y, int w, int h, uint32_t color) {
    (void)disp;
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)color;
    return -1;
}

static int default_draw_circle(display_t* disp, int x, int y, int radius, uint32_t color) {
    (void)disp;
    (void)x;
    (void)y;
    (void)radius;
    (void)color;
    return -1;
}

static int default_draw_image(display_t* disp, int x, int y, const void* img_data, int w, int h) {
    (void)disp;
    (void)x;
    (void)y;
    (void)img_data;
    (void)w;
    (void)h;
    return -1;
}

static int default_set_brightness(display_t* disp, int brightness) {
    (void)disp;
    (void)brightness;
    return -1;
}

static int default_flush(display_t* disp) {
    (void)disp;
    return -1;
}

static const display_ops_t default_ops = {
    .init = default_init,
    .deinit = default_deinit,
    .clear = default_clear,
    .draw_string = default_draw_string,
    .draw_pixel = default_draw_pixel,
    .draw_rect = default_draw_rect,
    .draw_circle = default_draw_circle,
    .draw_image = default_draw_image,
    .set_brightness = default_set_brightness,
    .flush = default_flush
};

display_t* display_create(display_type_t type) {
    display_t* disp = malloc(sizeof(display_t));
    if (disp == NULL) {
        LOGE(LOG_TAG, "Failed to allocate display");
        return NULL;
    }

    memset(disp, 0, sizeof(display_t));
    disp->ops = &default_ops;
    disp->impl = NULL;
    disp->width = 0;
    disp->height = 0;
    disp->initialized = false;

    (void)type;
    return disp;
}

void display_destroy(display_t* disp) {
    if (disp == NULL) {
        return;
    }
    if (disp->initialized && disp->ops && disp->ops->deinit) {
        disp->ops->deinit(disp);
    }
    free(disp);
}

int display_init(display_t* disp) {
    if (disp == NULL || disp->ops == NULL || disp->ops->init == NULL) {
        return -1;
    }
    int ret = disp->ops->init(disp);
    if (ret == 0) {
        disp->initialized = true;
    }
    return ret;
}

void display_deinit(display_t* disp) {
    if (disp == NULL || disp->ops == NULL || disp->ops->deinit == NULL) {
        return;
    }
    disp->ops->deinit(disp);
    disp->initialized = false;
}

int display_clear(display_t* disp) {
    if (disp == NULL || disp->ops == NULL || disp->ops->clear == NULL) {
        return -1;
    }
    return disp->ops->clear(disp);
}

int display_draw_string(display_t* disp, int x, int y, const char* str) {
    if (disp == NULL || disp->ops == NULL || disp->ops->draw_string == NULL) {
        return -1;
    }
    return disp->ops->draw_string(disp, x, y, str);
}

int display_draw_pixel(display_t* disp, int x, int y, uint32_t color) {
    if (disp == NULL || disp->ops == NULL || disp->ops->draw_pixel == NULL) {
        return -1;
    }
    return disp->ops->draw_pixel(disp, x, y, color);
}

int display_draw_rect(display_t* disp, int x, int y, int w, int h, uint32_t color) {
    if (disp == NULL || disp->ops == NULL || disp->ops->draw_rect == NULL) {
        return -1;
    }
    return disp->ops->draw_rect(disp, x, y, w, h, color);
}

int display_draw_circle(display_t* disp, int x, int y, int radius, uint32_t color) {
    if (disp == NULL || disp->ops == NULL || disp->ops->draw_circle == NULL) {
        return -1;
    }
    return disp->ops->draw_circle(disp, x, y, radius, color);
}

int display_draw_image(display_t* disp, int x, int y, const void* img_data, int w, int h) {
    if (disp == NULL || disp->ops == NULL || disp->ops->draw_image == NULL) {
        return -1;
    }
    return disp->ops->draw_image(disp, x, y, img_data, w, h);
}

int display_set_brightness(display_t* disp, int brightness) {
    if (disp == NULL || disp->ops == NULL || disp->ops->set_brightness == NULL) {
        return -1;
    }
    return disp->ops->set_brightness(disp, brightness);
}

int display_flush(display_t* disp) {
    if (disp == NULL || disp->ops == NULL || disp->ops->flush == NULL) {
        return -1;
    }
    return disp->ops->flush(disp);
}

display_type_t display_get_type(const display_t* disp) {
    (void)disp;
    return DISPLAY_TYPE_NONE;
}

bool display_is_initialized(const display_t* disp) {
    return disp != NULL && disp->initialized;
}

int display_get_width(const display_t* disp) {
    return disp ? disp->width : 0;
}

int display_get_height(const display_t* disp) {
    return disp ? disp->height : 0;
}
