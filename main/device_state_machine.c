#include "device_state_machine.h"

#include <stdlib.h>
#include <string.h>

// 最大监听器数量
#define MAX_LISTENERS 16

// 监听器结构
typedef struct {
    state_change_callback_t callback;
    void* user_data;
    bool valid;
} listener_t;

// 状态机结构
struct device_state_machine {
    device_state_t current_state;
    listener_t listeners[MAX_LISTENERS];
    int listener_count;
};

// 创建状态机
device_state_machine_t* device_state_machine_create(void) {
    device_state_machine_t* sm = (device_state_machine_t*)malloc(sizeof(device_state_machine_t));
    if (sm == NULL) {
        return NULL;
    }

    memset(sm, 0, sizeof(device_state_machine_t));
    sm->current_state = DEVICE_STATE_STARTING;

    return sm;
}

// 销毁状态机
void device_state_machine_destroy(device_state_machine_t* sm) {
    if (sm != NULL) {
        free(sm);
    }
}

// 检查状态转换是否有效
static bool is_valid_transition(device_state_t from, device_state_t to) {
    // 允许转换到相同状态（无操作）
    if (from == to) {
        return true;
    }

    // 定义有效的状态转换
    switch (from) {
        case DEVICE_STATE_STARTING:
            return to == DEVICE_STATE_WIFI_CONFIGURING;

        case DEVICE_STATE_WIFI_CONFIGURING:
            return to == DEVICE_STATE_IDLE;

        case DEVICE_STATE_IDLE:
            return to == DEVICE_STATE_LISTENING ||
                   to == DEVICE_STATE_SPEAKING ||
                   to == DEVICE_STATE_ERROR ||
                   to == DEVICE_STATE_WIFI_CONFIGURING;

        case DEVICE_STATE_LISTENING:
            return to == DEVICE_STATE_SPEAKING ||
                   to == DEVICE_STATE_IDLE;

        case DEVICE_STATE_SPEAKING:
            return to == DEVICE_STATE_LISTENING ||
                   to == DEVICE_STATE_IDLE;

        case DEVICE_STATE_ERROR:
            // 从错误状态只能转换到 WIFI_CONFIGURING 进行恢复
            return to == DEVICE_STATE_WIFI_CONFIGURING;

        default:
            return false;
    }
}

// 通知监听器状态变化
static void notify_listeners(device_state_machine_t* sm, device_state_t old_state, device_state_t new_state) {
    for (int i = 0; i < MAX_LISTENERS; i++) {
        if (sm->listeners[i].valid && sm->listeners[i].callback != NULL) {
            sm->listeners[i].callback(old_state, new_state, sm->listeners[i].user_data);
        }
    }
}

// 状态转换
int device_state_machine_transition(device_state_machine_t* sm, device_state_t new_state) {
    if (sm == NULL) {
        return -1;
    }

    device_state_t old_state = sm->current_state;

    // 如果已经是目标状态，返回成功
    if (old_state == new_state) {
        return 0;
    }

    // 验证转换
    if (!is_valid_transition(old_state, new_state)) {
        return -2;
    }

    // 执行转换
    sm->current_state = new_state;

    // 通知监听器
    notify_listeners(sm, old_state, new_state);

    return 0;
}

// 获取当前状态
device_state_t device_state_machine_get_state(const device_state_machine_t* sm) {
    if (sm == NULL) {
        return DEVICE_STATE_MAX;
    }
    return sm->current_state;
}

// 添加监听器
int device_state_machine_add_listener(device_state_machine_t* sm, state_change_callback_t callback, void* user_data) {
    if (sm == NULL || callback == NULL) {
        return -1;
    }

    // 查找空闲槽位
    for (int i = 0; i < MAX_LISTENERS; i++) {
        if (!sm->listeners[i].valid) {
            sm->listeners[i].callback = callback;
            sm->listeners[i].user_data = user_data;
            sm->listeners[i].valid = true;
            return i;
        }
    }

    // 监听器已满
    return -2;
}

// 判断是否处于空闲状态
bool device_state_machine_is_idle(const device_state_machine_t* sm) {
    if (sm == NULL) {
        return false;
    }
    return sm->current_state == DEVICE_STATE_IDLE;
}

// 判断是否处于活跃状态
bool device_state_machine_is_active(const device_state_machine_t* sm) {
    if (sm == NULL) {
        return false;
    }
    return sm->current_state == DEVICE_STATE_LISTENING ||
           sm->current_state == DEVICE_STATE_SPEAKING;
}
