// wake_word.h
#ifndef WAKE_WORD_H
#define WAKE_WORD_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 唤醒词检测器
typedef struct wake_word wake_word_t;

// 唤醒词检测回调
typedef void (*wake_word_detected_callback_t)(const char* wake_word, void* user_data);

// 唤醒词配置
typedef struct {
    const char* model_path;
    int sensitivity;
    bool debug_mode;
} wake_word_config_t;

// 唤醒词 API
wake_word_t* wake_word_create(const wake_word_config_t* config);
void wake_word_destroy(wake_word_t* ww);
int wake_word_init(wake_word_t* ww);
void wake_word_deinit(wake_word_t* ww);
int wake_word_start(wake_word_t* ww);
int wake_word_stop(wake_word_t* ww);
bool wake_word_is_running(const wake_word_t* ww);

// 回调注册
void wake_word_set_callback(wake_word_t* ww,
    wake_word_detected_callback_t callback, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // WAKE_WORD_H
