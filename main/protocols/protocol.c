#include "protocol.h"
#include "log_utils.h"
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "protocol"

static int default_connect(protocol_t* proto) {
    (void)proto;
    return -1;
}

static void default_disconnect(protocol_t* proto) {
    (void)proto;
}

static int default_send_text(protocol_t* proto, const char* text, size_t len) {
    (void)proto;
    (void)text;
    (void)len;
    return -1;
}

static int default_send_binary(protocol_t* proto, const uint8_t* data, size_t len) {
    (void)proto;
    (void)data;
    (void)len;
    return -1;
}

static void default_poll(protocol_t* proto) {
    (void)proto;
}

static void default_destroy(protocol_t* proto) {
    free(proto->impl);
    proto->impl = NULL;
}

static const protocol_ops_t default_ops = {
    .connect = default_connect,
    .disconnect = default_disconnect,
    .send_text = default_send_text,
    .send_binary = default_send_binary,
    .poll = default_poll,
    .destroy = default_destroy
};

protocol_t* protocol_create(protocol_type_t type) {
    protocol_t* proto = malloc(sizeof(protocol_t));
    if (proto == NULL) {
        LOGE(LOG_TAG, "Failed to allocate protocol");
        return NULL;
    }

    memset(proto, 0, sizeof(protocol_t));
    proto->type = type;
    proto->ops = &default_ops;
    proto->impl = NULL;

    return proto;
}

void protocol_destroy(protocol_t* proto) {
    if (proto == NULL) {
        return;
    }

    if (proto->ops && proto->ops->destroy) {
        proto->ops->destroy(proto);
    }

    free(proto);
}

int protocol_connect(protocol_t* proto) {
    if (proto == NULL || proto->ops == NULL || proto->ops->connect == NULL) {
        return -1;
    }
    return proto->ops->connect(proto);
}

void protocol_disconnect(protocol_t* proto) {
    if (proto == NULL || proto->ops == NULL || proto->ops->disconnect == NULL) {
        return;
    }
    proto->ops->disconnect(proto);
}

int protocol_send_text(protocol_t* proto, const char* text, size_t len) {
    if (proto == NULL || proto->ops == NULL || proto->ops->send_text == NULL) {
        return -1;
    }
    return proto->ops->send_text(proto, text, len);
}

int protocol_send_binary(protocol_t* proto, const uint8_t* data, size_t len) {
    if (proto == NULL || proto->ops == NULL || proto->ops->send_binary == NULL) {
        return -1;
    }
    return proto->ops->send_binary(proto, data, len);
}

void protocol_poll(protocol_t* proto) {
    if (proto == NULL || proto->ops == NULL || proto->ops->poll == NULL) {
        return;
    }
    proto->ops->poll(proto);
}

// 回调注册函数实现
void protocol_set_connected_callback(protocol_t* proto,
    protocol_connected_callback_t cb, void* user_data) {
    if (proto == NULL) return;
    proto->on_connected = cb;
    proto->callback_user_data = user_data;
}

void protocol_set_disconnected_callback(protocol_t* proto,
    protocol_disconnected_callback_t cb, void* user_data) {
    if (proto == NULL) return;
    proto->on_disconnected = cb;
    proto->callback_user_data = user_data;
}

void protocol_set_network_error_callback(protocol_t* proto,
    protocol_network_error_callback_t cb, void* user_data) {
    if (proto == NULL) return;
    proto->on_network_error = cb;
    proto->callback_user_data = user_data;
}

void protocol_set_incoming_audio_callback(protocol_t* proto,
    protocol_incoming_audio_callback_t cb, void* user_data) {
    if (proto == NULL) return;
    proto->on_incoming_audio = cb;
    proto->callback_user_data = user_data;
}

void protocol_set_incoming_text_callback(protocol_t* proto,
    protocol_incoming_text_callback_t cb, void* user_data) {
    if (proto == NULL) return;
    proto->on_incoming_text = cb;
    proto->callback_user_data = user_data;
}

void protocol_set_incoming_json_callback(protocol_t* proto,
    protocol_incoming_json_callback_t cb, void* user_data) {
    if (proto == NULL) return;
    proto->on_incoming_json = cb;
    proto->callback_user_data = user_data;
}

protocol_type_t protocol_get_type(const protocol_t* proto) {
    return proto ? proto->type : PROTOCOL_TYPE_MQTT;
}

bool protocol_is_connected(const protocol_t* proto) {
    (void)proto;
    // 由具体实现类实现
    return false;
}
