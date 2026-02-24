#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 事件类型
typedef enum {
    EVENT_WAKE_WORD_DETECTED = 0,
    EVENT_START_LISTENING,
    EVENT_STOP_LISTENING,
    EVENT_START_SPEAKING,
    EVENT_STOP_SPEAKING,
    EVENT_VAD_CHANGE,
    EVENT_NETWORK_CONNECTED,
    EVENT_NETWORK_DISCONNECTED,
    EVENT_NETWORK_ERROR,
    EVENT_STATE_CHANGE,
    EVENT_MCP_MESSAGE,
    EVENT_OTA_START,
    EVENT_OTA_PROGRESS,
    EVENT_OTA_COMPLETE,
    EVENT_OTA_ERROR,
    EVENT_BUTTON_PRESS,
    EVENT_MAX
} event_type_t;

// 事件数据联合体
typedef union {
    int int_value;
    bool bool_value;
    float float_value;
    const char* string_value;
    void* ptr_value;
    struct {
        int old_state;
        int new_state;
    } state_change;
    struct {
        const char* wake_word;
    } wake_word_data;
    struct {
        const char* message;
    } error_data;
    struct {
        int progress;
    } ota_progress;
} event_data_t;

// 事件结构
typedef struct {
    event_type_t type;
    event_data_t data;
    uint64_t timestamp;
} event_t;

// 事件回调函数类型
typedef void (*event_callback_t)(const event_t* event, void* user_data);

// 事件监听器
typedef struct event_listener {
    event_callback_t callback;
    void* user_data;
    uint32_t event_mask;  // 位掩码，监听哪些事件
    struct event_listener* next;
} event_listener_t;

// 事件系统 API
int event_system_init(void);
void event_system_deinit(void);
int event_subscribe(event_listener_t* listener);
int event_unsubscribe(event_listener_t* listener);
void event_publish(const event_t* event);
void event_publish_type(event_type_t type);

// 便捷订阅宏
#define EVENT_MASK(event_type) (1U << (event_type))

#ifdef __cplusplus
}
#endif

#endif // EVENT_SYSTEM_H
