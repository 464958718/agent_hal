#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 前向声明
typedef struct protocol protocol_t;
typedef struct audio_stream_packet audio_stream_packet_t;

// 协议类型
typedef enum {
    PROTOCOL_TYPE_MQTT,
    PROTOCOL_TYPE_WEBSOCKET,
    PROTOCOL_TYPE_MQTT_UDP
} protocol_type_t;

// 协议操作接口
typedef struct protocol_ops {
    int (*connect)(protocol_t* proto);
    void (*disconnect)(protocol_t* proto);
    int (*send_text)(protocol_t* proto, const char* text, size_t len);
    int (*send_binary)(protocol_t* proto, const uint8_t* data, size_t len);
    void (*poll)(protocol_t* proto);
    void (*destroy)(protocol_t* proto);
} protocol_ops_t;

// 回调函数类型
typedef void (*protocol_connected_callback_t)(void* user_data);
typedef void (*protocol_disconnected_callback_t)(void* user_data);
typedef void (*protocol_network_error_callback_t)(const char* message, void* user_data);
typedef void (*protocol_incoming_audio_callback_t)(audio_stream_packet_t* packet, void* user_data);
typedef void (*protocol_incoming_text_callback_t)(const char* text, void* user_data);
typedef void (*protocol_incoming_json_callback_t)(const char* json, void* user_data);

// 协议实例结构
struct protocol {
    const protocol_ops_t* ops;
    void* impl;
    void* user_data;
    protocol_type_t type;

    // 回调函数
    protocol_connected_callback_t on_connected;
    protocol_disconnected_callback_t on_disconnected;
    protocol_network_error_callback_t on_network_error;
    protocol_incoming_audio_callback_t on_incoming_audio;
    protocol_incoming_text_callback_t on_incoming_text;
    protocol_incoming_json_callback_t on_incoming_json;

    void* callback_user_data;
};

// 协议工厂函数
protocol_t* protocol_create(protocol_type_t type);
void protocol_destroy(protocol_t* proto);

// 协议操作
int protocol_connect(protocol_t* proto);
void protocol_disconnect(protocol_t* proto);
int protocol_send_text(protocol_t* proto, const char* text, size_t len);
int protocol_send_binary(protocol_t* proto, const uint8_t* data, size_t len);
void protocol_poll(protocol_t* proto);

// 回调注册
void protocol_set_connected_callback(protocol_t* proto,
    protocol_connected_callback_t cb, void* user_data);
void protocol_set_disconnected_callback(protocol_t* proto,
    protocol_disconnected_callback_t cb, void* user_data);
void protocol_set_network_error_callback(protocol_t* proto,
    protocol_network_error_callback_t cb, void* user_data);
void protocol_set_incoming_audio_callback(protocol_t* proto,
    protocol_incoming_audio_callback_t cb, void* user_data);
void protocol_set_incoming_text_callback(protocol_t* proto,
    protocol_incoming_text_callback_t cb, void* user_data);
void protocol_set_incoming_json_callback(protocol_t* proto,
    protocol_incoming_json_callback_t cb, void* user_data);

// 辅助函数
protocol_type_t protocol_get_type(const protocol_t* proto);
bool protocol_is_connected(const protocol_t* proto);

#ifdef __cplusplus
}
#endif

#endif // PROTOCOL_H
