#ifndef WEBSOCKET_PROTOCOL_H
#define WEBSOCKET_PROTOCOL_H

#include "protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

// WebSocket 配置
typedef struct {
    const char* url;
    const char* path;
    const char* host;
    const char* protocol;
    uint16_t port;
    bool use_tls;
    const char** subprotocols;
    size_t subprotocol_count;
} websocket_config_t;

// WebSocket 协议创建函数
protocol_t* websocket_protocol_create(const websocket_config_t* config);

#ifdef __cplusplus
}
#endif

#endif // WEBSOCKET_PROTOCOL_H
