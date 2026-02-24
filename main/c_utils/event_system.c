#include "event_system.h"
#include <stdlib.h>
#include <string.h>

static event_listener_t* g_listeners = NULL;
static bool g_initialized = false;
static uint64_t g_event_id = 0;

int event_system_init(void) {
    if (g_initialized) {
        return 0;
    }
    g_listeners = NULL;
    g_event_id = 0;
    g_initialized = true;
    return 0;
}

void event_system_deinit(void) {
    g_listeners = NULL;
    g_initialized = false;
}

int event_subscribe(event_listener_t* listener) {
    if (listener == NULL || listener->callback == NULL) {
        return -1;
    }

    listener->next = g_listeners;
    g_listeners = listener;
    return 0;
}

int event_unsubscribe(event_listener_t* listener) {
    if (listener == NULL) {
        return -1;
    }

    event_listener_t* prev = NULL;
    event_listener_t* current = g_listeners;

    while (current != NULL) {
        if (current == listener) {
            if (prev == NULL) {
                g_listeners = current->next;
            } else {
                prev->next = current->next;
            }
            return 0;
        }
        prev = current;
        current = current->next;
    }

    return -1;
}

void event_publish(const event_t* event) {
    if (event == NULL || !g_initialized) {
        return;
    }

    event_listener_t* current = g_listeners;
    while (current != NULL) {
        if (current->event_mask & (1U << event->type)) {
            event_t evt = *event;
            evt.timestamp = ++g_event_id;
            current->callback(&evt, current->user_data);
        }
        current = current->next;
    }
}

void event_publish_type(event_type_t type) {
    event_t event = {
        .type = type,
        .data = { .int_value = 0 },
        .timestamp = 0
    };
    event_publish(&event);
}
