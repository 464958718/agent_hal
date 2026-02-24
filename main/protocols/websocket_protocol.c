#include "websocket_protocol.h"
#include "log_utils.h"
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "websocket"

// WebSocket 实现结构
typedef struct {
    websocket_config_t config;
    bool connected;
    bool connecting;
    // WebSocket 客户端
    void* ws_client;
} ws_impl_t;

static int ws_connect(protocol_t* proto) {
    if (proto == NULL) return -1;

    ws_impl_t* impl = (ws_impl_t*)proto->impl;
    if (impl == NULL) return -1;

    LOGI(LOG_TAG, "Connecting to WebSocket: %s", impl->config.url);

    // TODO: 使用 ESP-IDF websocket API 实现连接
    impl->connecting = true;

    return 0;
}

static void ws_disconnect(protocol_t* proto) {
    if (proto == NULL) return;

    ws_impl_t* impl = (ws_impl_t*)proto->impl;
    if (impl == NULL) return;

    impl->connected = false;
    impl->connecting = false;

    LOGI(LOG_TAG, "Disconnected from WebSocket");
}

static int ws_send_text(protocol_t* proto, const char* text, size_t len) {
    if (proto == NULL || text == NULL) return -1;

    ws_impl_t* impl = (ws_impl_t*)proto->impl;
    if (impl == NULL || !impl->connected) return -1;

    // TODO: 使用 ESP-IDF websocket API 发送文本
    LOGD(LOG_TAG, "Sending text: %.*s", (int)len, text);

    return (int)len;
}

static int ws_send_binary(protocol_t* proto, const uint8_t* data, size_t len) {
    if (proto == NULL || data == NULL) return -1;

    ws_impl_t* impl = (ws_impl_t*)proto->impl;
    if (impl == NULL || !impl->connected) return -1;

    // TODO: 使用 ESP-IDF websocket API 发送二进制
    LOGD(LOG_TAG, "Sending binary: %zu bytes", len);

    return (int)len;
}

static void ws_poll(protocol_t* proto) {
    if (proto == NULL) return;

    // TODO: 处理 WebSocket 事件
}

static void ws_destroy(protocol_t* proto) {
    if (proto == NULL) return;

    ws_impl_t* impl = (ws_impl_t*)proto->impl;
    if (impl != NULL) {
        if (impl->ws_client != NULL) {
            // TODO: destroy websocket client
        }
        free((void*)impl->config.url);
        free((void*)impl->config.path);
        free((void*)impl->config.host);
        free((void*)impl->config.protocol);
        free(impl);
    }
    proto->impl = NULL;
}

static const protocol_ops_t ws_ops = {
    .connect = ws_connect,
    .disconnect = ws_disconnect,
    .send_text = ws_send_text,
    .send_binary = ws_send_binary,
    .poll = ws_poll,
    .destroy = ws_destroy
};

protocol_t* websocket_protocol_create(const websocket_config_t* config) {
    if (config == NULL || config->url == NULL) {
        return NULL;
    }

    protocol_t* proto = protocol_create(PROTOCOL_TYPE_WEBSOCKET);
    if (proto == NULL) {
        return NULL;
    }

    ws_impl_t* impl = malloc(sizeof(ws_impl_t));
    if (impl == NULL) {
        protocol_destroy(proto);
        return NULL;
    }

    memset(impl, 0, sizeof(ws_impl_t));

    impl->config.url = strdup(config->url);
    impl->config.path = config->path ? strdup(config->path) : NULL;
    impl->config.host = config->host ? strdup(config->host) : NULL;
    impl->config.protocol = config->protocol ? strdup(config->protocol) : NULL;
    impl->config.port = config->port;
    impl->config.use_tls = config->use_tls;

    proto->impl = impl;
    proto->ops = &ws_ops;

    return proto;
}
