#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// OTA 状态
typedef enum {
    OTA_STATE_IDLE,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_FLASHING,
    OTA_STATE_REBOOTING,
    OTA_STATE_ERROR
} ota_state_t;

// OTA 进度回调
typedef void (*ota_progress_callback_t)(int progress, void* user_data);

// OTA 错误回调
typedef void (*ota_error_callback_t)(int error_code, const char* message, void* user_data);

// OTA 控制结构
typedef struct ota_update ota_update_t;

// OTA API
ota_update_t* ota_update_create(void);
void ota_update_destroy(ota_update_t* ota);

// OTA 操作
int ota_update_check_version(ota_update_t* ota, const char* version_url);
int ota_update_start(ota_update_t* ota, const char* url);
int ota_update_cancel(ota_update_t* ota);

// 状态查询
ota_state_t ota_update_get_state(const ota_update_t* ota);
const char* ota_update_get_current_version(const ota_update_t* ota);
const char* ota_update_get_latest_version(const ota_update_t* ota);

// 回调注册
void ota_update_set_progress_callback(ota_update_t* ota,
    ota_progress_callback_t cb, void* user_data);
void ota_update_set_error_callback(ota_update_t* ota,
    ota_error_callback_t cb, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // OTA_UPDATE_H
