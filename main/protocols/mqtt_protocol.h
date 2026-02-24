#ifndef MQTT_PROTOCOL_H
#define MQTT_PROTOCOL_H

#include "protocol.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// MQTT 配置
typedef struct {
    const char* broker_url;
    const char* client_id;
    const char* username;
    const char* password;
    const char* publish_topic;
    const char* subscribe_topic;
    uint16_t keepalive;
    uint16_t port;
    bool use_tls;
} mqtt_config_t;

// MQTT 协议创建函数
protocol_t* mqtt_protocol_create(const mqtt_config_t* config);

#ifdef __cplusplus
}
#endif

#endif // MQTT_PROTOCOL_H
