#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Device configuration
typedef struct {
    const char* device_name;
    const char* wifi_ssid;
    const char* wifi_password;
    const char* server_url;
    const char* protocol_type;  // "mqtt" or "websocket"
    const char* language;
    int volume;
    bool wake_word_enabled;
} device_config_t;

// Application state
typedef enum {
    APP_STATE_INIT,
    APP_STATE_WIFI_CONNECTING,
    APP_STATE_WIFI_CONNECTED,
    APP_STATE_SERVER_CONNECTING,
    APP_STATE_RUNNING,
    APP_STATE_ERROR
} app_state_t;

// Application structure
typedef struct application application_t;

// Application API
application_t* application_create(const device_config_t* config);
void application_destroy(application_t* app);

// Lifecycle
int application_init(application_t* app);
int application_start(application_t* app);
void application_stop(application_t* app);

// Event processing
void application_process(application_t* app);

// Configuration update
int application_update_config(application_t* app, const device_config_t* config);

// State query
app_state_t application_get_state(const application_t* app);
const char* application_get_version(const application_t* app);

#ifdef __cplusplus
}
#endif

#endif // APPLICATION_H
