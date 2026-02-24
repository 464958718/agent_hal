#include "application.h"
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#define TAG "application"

// Application version
#define APPLICATION_VERSION "1.0.0"

// Application event bits
#define APP_EVENT_INIT_DONE       (1 << 0)
#define APP_EVENT_START          (1 << 1)
#define APP_EVENT_STOP           (1 << 2)
#define APP_EVENT_CONFIG_UPDATE  (1 << 3)
#define APP_EVENT_ERROR          (1 << 4)

// Internal application structure
struct application {
    device_config_t config;
    app_state_t state;
    char* version;

    // FreeRTOS components
    EventGroupHandle_t event_group;
    TaskHandle_t task_handle;

    // Internal state
    bool is_running;
    char* error_message;
};

// Forward declarations of internal functions
static void application_task(void* param);
static void set_state(application_t* app, app_state_t new_state);

application_t* application_create(const device_config_t* config) {
    if (config == NULL) {
        ESP_LOGE(TAG, "Cannot create application with NULL config");
        return NULL;
    }

    application_t* app = (application_t*)malloc(sizeof(application_t));
    if (app == NULL) {
        ESP_LOGE(TAG, "Failed to allocate application");
        return NULL;
    }

    // Initialize with default values
    memset(app, 0, sizeof(application_t));

    // Copy configuration
    if (config->device_name != NULL) {
        app->config.device_name = strdup(config->device_name);
    }
    if (config->wifi_ssid != NULL) {
        app->config.wifi_ssid = strdup(config->wifi_ssid);
    }
    if (config->wifi_password != NULL) {
        app->config.wifi_password = strdup(config->wifi_password);
    }
    if (config->server_url != NULL) {
        app->config.server_url = strdup(config->server_url);
    }
    if (config->protocol_type != NULL) {
        app->config.protocol_type = strdup(config->protocol_type);
    }
    if (config->language != NULL) {
        app->config.language = strdup(config->language);
    }
    app->config.volume = config->volume;
    app->config.wake_word_enabled = config->wake_word_enabled;

    // Initialize version
    app->version = strdup(APPLICATION_VERSION);

    // Create event group
    app->event_group = xEventGroupCreate();
    if (app->event_group == NULL) {
        ESP_LOGE(TAG, "Failed to create event group");
        application_destroy(app);
        return NULL;
    }

    // Set initial state
    app->state = APP_STATE_INIT;
    app->is_running = false;

    ESP_LOGI(TAG, "Application created successfully");
    return app;
}

void application_destroy(application_t* app) {
    if (app == NULL) {
        return;
    }

    // Stop application if running
    if (app->is_running) {
        application_stop(app);
    }

    // Free configuration strings
    if (app->config.device_name != NULL) {
        free((void*)app->config.device_name);
    }
    if (app->config.wifi_ssid != NULL) {
        free((void*)app->config.wifi_ssid);
    }
    if (app->config.wifi_password != NULL) {
        free((void*)app->config.wifi_password);
    }
    if (app->config.server_url != NULL) {
        free((void*)app->config.server_url);
    }
    if (app->config.protocol_type != NULL) {
        free((void*)app->config.protocol_type);
    }
    if (app->config.language != NULL) {
        free((void*)app->config.language);
    }

    // Free version
    if (app->version != NULL) {
        free(app->version);
    }

    // Free error message
    if (app->error_message != NULL) {
        free(app->error_message);
    }

    // Delete event group
    if (app->event_group != NULL) {
        vEventGroupDelete(app->event_group);
    }

    // Free application structure
    free(app);
    ESP_LOGI(TAG, "Application destroyed");
}

static void set_state(application_t* app, app_state_t new_state) {
    if (app == NULL) {
        return;
    }

    app_state_t old_state = app->state;
    app->state = new_state;

    ESP_LOGI(TAG, "State changed: %d -> %d", old_state, new_state);
}

int application_init(application_t* app) {
    if (app == NULL) {
        return -1;
    }

    if (app->state != APP_STATE_INIT) {
        ESP_LOGE(TAG, "Application already initialized");
        return -1;
    }

    ESP_LOGI(TAG, "Initializing application...");

    // Perform initialization tasks here:
    // - Initialize display
    // - Initialize audio service
    // - Initialize network
    // - Set up callbacks

    ESP_LOGI(TAG, "Application initialized successfully");
    return 0;
}

int application_start(application_t* app) {
    if (app == NULL) {
        return -1;
    }

    if (app->is_running) {
        ESP_LOGW(TAG, "Application already running");
        return 0;
    }

    ESP_LOGI(TAG, "Starting application...");

    // Create main application task
    BaseType_t ret = xTaskCreate(
        application_task,
        "application_task",
        4096,
        app,
        5,
        &app->task_handle
    );

    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create application task");
        return -1;
    }

    app->is_running = true;
    ESP_LOGI(TAG, "Application started successfully");
    return 0;
}

void application_stop(application_t* app) {
    if (app == NULL) {
        return;
    }

    if (!app->is_running) {
        ESP_LOGW(TAG, "Application not running");
        return;
    }

    ESP_LOGI(TAG, "Stopping application...");

    // Signal stop event
    xEventGroupSetBits(app->event_group, APP_EVENT_STOP);

    // Wait for task to finish
    if (app->task_handle != NULL) {
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000));
        app->task_handle = NULL;
    }

    app->is_running = false;
    ESP_LOGI(TAG, "Application stopped");
}

void application_process(application_t* app) {
    if (app == NULL) {
        return;
    }

    // This function can be called periodically to process events
    // The actual event processing happens in the application_task

    // Process any pending events here if needed
}

int application_update_config(application_t* app, const device_config_t* config) {
    if (app == NULL || config == NULL) {
        return -1;
    }

    ESP_LOGI(TAG, "Updating application configuration...");

    // Free existing configuration
    if (app->config.device_name != NULL) {
        free((void*)app->config.device_name);
    }
    if (app->config.wifi_ssid != NULL) {
        free((void*)app->config.wifi_ssid);
    }
    if (app->config.wifi_password != NULL) {
        free((void*)app->config.wifi_password);
    }
    if (app->config.server_url != NULL) {
        free((void*)app->config.server_url);
    }
    if (app->config.protocol_type != NULL) {
        free((void*)app->config.protocol_type);
    }
    if (app->config.language != NULL) {
        free((void*)app->config.language);
    }

    // Copy new configuration
    if (config->device_name != NULL) {
        app->config.device_name = strdup(config->device_name);
    }
    if (config->wifi_ssid != NULL) {
        app->config.wifi_ssid = strdup(config->wifi_ssid);
    }
    if (config->wifi_password != NULL) {
        app->config.wifi_password = strdup(config->wifi_password);
    }
    if (config->server_url != NULL) {
        app->config.server_url = strdup(config->server_url);
    }
    if (config->protocol_type != NULL) {
        app->config.protocol_type = strdup(config->protocol_type);
    }
    if (config->language != NULL) {
        app->config.language = strdup(config->language);
    }
    app->config.volume = config->volume;
    app->config.wake_word_enabled = config->wake_word_enabled;

    // Signal configuration update event if running
    if (app->is_running) {
        xEventGroupSetBits(app->event_group, APP_EVENT_CONFIG_UPDATE);
    }

    ESP_LOGI(TAG, "Configuration updated successfully");
    return 0;
}

app_state_t application_get_state(const application_t* app) {
    if (app == NULL) {
        return APP_STATE_ERROR;
    }
    return app->state;
}

const char* application_get_version(const application_t* app) {
    if (app == NULL) {
        return NULL;
    }
    return app->version;
}

// Internal task function
static void application_task(void* param) {
    application_t* app = (application_t*)param;

    const EventBits_t all_events =
        APP_EVENT_INIT_DONE |
        APP_EVENT_START |
        APP_EVENT_STOP |
        APP_EVENT_CONFIG_UPDATE |
        APP_EVENT_ERROR;

    ESP_LOGI(TAG, "Application task started");

    while (true) {
        EventBits_t bits = xEventGroupWaitBits(
            app->event_group,
            all_events,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY
        );

        if (bits & APP_EVENT_STOP) {
            ESP_LOGI(TAG, "Stop event received, exiting application task");
            break;
        }

        if (bits & APP_EVENT_INIT_DONE) {
            ESP_LOGI(TAG, "Init done event processed");
        }

        if (bits & APP_EVENT_START) {
            ESP_LOGI(TAG, "Start event processed");
            set_state(app, APP_STATE_WIFI_CONNECTING);
        }

        if (bits & APP_EVENT_CONFIG_UPDATE) {
            ESP_LOGI(TAG, "Configuration update event processed");
            // Handle configuration update
        }

        if (bits & APP_EVENT_ERROR) {
            ESP_LOGE(TAG, "Error event processed: %s",
                     app->error_message ? app->error_message : "unknown");
            set_state(app, APP_STATE_ERROR);
        }
    }

    // Notify that task is about to exit
    xTaskNotifyGive(xTaskGetCurrentTaskHandle());
    vTaskDelete(NULL);
}
