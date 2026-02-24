#ifndef DEVICE_STATE_MACHINE_H
#define DEVICE_STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 设备状态枚举
typedef enum {
    DEVICE_STATE_STARTING = 0,
    DEVICE_STATE_WIFI_CONFIGURING,
    DEVICE_STATE_IDLE,
    DEVICE_STATE_LISTENING,
    DEVICE_STATE_SPEAKING,
    DEVICE_STATE_ERROR,
    DEVICE_STATE_MAX
} device_state_t;

// 状态变更回调
typedef void (*state_change_callback_t)(device_state_t old_state,
    device_state_t new_state, void* user_data);

// 状态机结构
typedef struct device_state_machine device_state_machine_t;

// 状态机 API
device_state_machine_t* device_state_machine_create(void);
void device_state_machine_destroy(device_state_machine_t* sm);

// 状态转换
int device_state_machine_transition(device_state_machine_t* sm,
    device_state_t new_state);
device_state_t device_state_machine_get_state(const device_state_machine_t* sm);

// 状态监听
int device_state_machine_add_listener(device_state_machine_t* sm,
    state_change_callback_t callback, void* user_data);

// 便捷函数
bool device_state_machine_is_idle(const device_state_machine_t* sm);
bool device_state_machine_is_active(const device_state_machine_t* sm);

#ifdef __cplusplus
}
#endif

#endif // DEVICE_STATE_MACHINE_H
