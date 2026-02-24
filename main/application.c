/*
 * Agent HAL - Application Layer (Pure C Version)
 *
 * Core application module that manages all sub-modules.
 */

#include "application.h"
#include "display.h"
#include "audio_manager.h"
#include "audio_codec.h"
#include "wake_word.h"
#include "led_control.h"
#include "protocol.h"
#include "mqtt_protocol.h"
#include "websocket_protocol.h"
#include "device_state_machine.h"
#include "ota_update.h"
#include "log_utils.h"

#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <nvs_flash.h>

#define TAG "application"

// Application version
#define APPLICATION_VERSION "1.0.0"

// Application event bits
#define APP_EVENT_INIT_DONE       (1 << 0)
#define APP_EVENT_START          (1 << 1)
#define APP_EVENT_STOP           (1 << 2)
#define APP_EVENT_CONFIG_UPDATE  (1 << 3)
#define APP_EVENT_ERROR          (1 << 4)
#define APP_EVENT_WIFI_CONNECTED (1 << 5)
#define APP_EVENT_SERVER_CONNECTED (1 << 6)

// WiFi event group bits
#define WIFI_CONNECTED_BIT   (1 << 0)
#define WIFI_FAILED_BIT      (1 << 1)

// Internal application structure
struct application {
    // Configuration
    device_config_t config;

    // State
    app_state_t state;
    char* version;
    char* error_message;

    // FreeRTOS components
    EventGroupHandle_t event_group;
    TaskHandle_t task_handle;

    // Sub-modules
    display_t* display;
    audio_manager_t* audio_mgr;
    wake_word_t* wake_word;
    led_control_t* led;
    protocol_t* protocol;
    device_state_machine_t* state_machine;
    ota_update_t* ota;

    // Internal state
    bool is_running;
    bool display_initialized;
    bool audio_initialized;
    bool protocol_initialized;
};

// Forward declarations
static void application_task(void* param);
static void set_state(application_t* app, app_state_t new_state);
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
static void on_wake_word_detected(const char* wake_word, void* user_data);
static void on_protocol_connected(void* user_data);
static void on_protocol_error(const char* message, void* user_data);
static void on_protocol_text(const char* text, void* user_data);

// WiFi event group
static EventGroupHandle_t s_wifi_event_group = NULL;

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

    // Initialize with zeros
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

    // Create WiFi event group if not exists
    if (s_wifi_event_group == NULL) {
        s_wifi_event_group = xEventGroupCreate();
    }

    // Set initial state
    app->state = APP_STATE_INIT;
    app->is_running = false;

    ESP_LOGI(TAG, "Application created: version=%s", app->version);
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
    if (app->config.device_name != NULL) free((void*)app->config.device_name);
    if (app->config.wifi_ssid != NULL) free((void*)app->config.wifi_ssid);
    if (app->config.wifi_password != NULL) free((void*)app->config.wifi_password);
    if (app->config.server_url != NULL) free((void*)app->config.server_url);
    if (app->config.protocol_type != NULL) free((void*)app->config.protocol_type);
    if (app->config.language != NULL) free((void*)app->config.language);

    // Free version
    if (app->version != NULL) free(app->version);
    if (app->error_message != NULL) free(app->error_message);

    // Destroy sub-modules
    if (app->display != NULL) display_destroy(app->display);
    if (app->audio_mgr != NULL) audio_manager_destroy(app->audio_mgr);
    if (app->wake_word != NULL) wake_word_destroy(app->wake_word);
    if (app->led != NULL) led_control_destroy(app->led);
    if (app->protocol != NULL) protocol_destroy(app->protocol);
    if (app->state_machine != NULL) device_state_machine_destroy(app->state_machine);
    if (app->ota != NULL) ota_update_destroy(app->ota);

    // Delete event group
    if (app->event_group != NULL) {
        vEventGroupDelete(app->event_group);
    }

    free(app);
    ESP_LOGI(TAG, "Application destroyed");
}

static void set_state(application_t* app, app_state_t new_state) {
    if (app == NULL) return;

    app_state_t old_state = app->state;
    app->state = new_state;

    ESP_LOGI(TAG, "State changed: %d -> %d", old_state, new_state);
}

int application_init(application_t* app) {
    if (app == NULL) return -1;
    if (app->state != APP_STATE_INIT) {
        ESP_LOGE(TAG, "Application already initialized");
        return -1;
    }

    ESP_LOGI(TAG, "Initializing application...");

    // Initialize log system
    log_config_t log_cfg = {
        .level = LOG_LEVEL_INFO,
        .output_fn = NULL,
        .use_colors = true,
        .print_timestamp = false
    };
    log_init(&log_cfg);

    // Initialize event system
    event_system_init();

    // Initialize state machine
    app->state_machine = device_state_machine_create();
    if (app->state_machine == NULL) {
        ESP_LOGE(TAG, "Failed to create state machine");
        return -1;
    }

    // Initialize display (use LCD as default)
    app->display = display_create(DISPLAY_TYPE_LCD);
    if (app->display != NULL) {
        if (display_init(app->display) == 0) {
            app->display_initialized = true;
            ESP_LOGI(TAG, "Display initialized");
        } else {
            ESP_LOGW(TAG, "Display init failed, continuing...");
        }
    }

    // Initialize LED
    app->led = led_control_create(LED_TYPE_GPIO);
    if (app->led != NULL) {
        if (led_control_init(app->led) == 0) {
            ESP_LOGI(TAG, "LED initialized");
            // Set LED to idle state
            led_color_t color = {255, 255, 255};
            led_control_set_color(app->led, &color);
        }
    }

    // Initialize audio manager
    app->audio_mgr = audio_manager_create();
    if (app->audio_mgr != NULL) {
        if (audio_manager_init(app->audio_mgr) == 0) {
            app->audio_initialized = true;
            // Set volume
            audio_manager_set_volume(app->audio_mgr, app->config.volume);
            ESP_LOGI(TAG, "Audio manager initialized, volume=%d", app->config.volume);
        } else {
            ESP_LOGW(TAG, "Audio init failed, continuing...");
        }
    }

    // Initialize wake word (if enabled)
    if (app->config.wake_word_enabled) {
        wake_word_config_t ww_config = {
            .model_path = NULL,
            .sensitivity = 0.7,
            .debug_mode = false
        };
        app->wake_word = wake_word_create(&ww_config);
        if (app->wake_word != NULL) {
            if (wake_word_init(app->wake_word) == 0) {
                wake_word_set_callback(app->wake_word, on_wake_word_detected, app);
                ESP_LOGI(TAG, "Wake word initialized");
            }
        }
    }

    // Initialize protocol (MQTT or WebSocket)
    if (strcmp(app->config.protocol_type, "mqtt") == 0) {
        mqtt_config_t mqtt_cfg = {
            .broker_url = app->config.server_url,
            .client_id = app->config.device_name,
            .username = NULL,
            .password = NULL,
            .publish_topic = "xiaozhi/audio",
            .subscribe_topic = "xiaozhi/response",
            .keepalive = 60,
            .port = 1883,
            .use_tls = false
        };
        app->protocol = mqtt_protocol_create(&mqtt_cfg);
    } else {
        websocket_config_t ws_cfg = {
            .url = app->config.server_url,
            .path = "/ws",
            .host = NULL,
            .protocol = "ws",
            .port = 443,
            .use_tls = true,
            .subprotocols = NULL,
            .subprotocol_count = 0
        };
        app->protocol = websocket_protocol_create(&ws_cfg);
    }

    if (app->protocol != NULL) {
        // Set protocol callbacks
        protocol_set_connected_callback(app->protocol, on_protocol_connected, app);
        protocol_set_network_error_callback(app->protocol, on_protocol_error, app);
        protocol_set_incoming_text_callback(app->protocol, on_protocol_text, app);
        ESP_LOGI(TAG, "Protocol created: %s", app->config.protocol_type);
    }

    // Initialize OTA
    app->ota = ota_update_create();
    if (app->ota != NULL) {
        ESP_LOGI(TAG, "OTA initialized");
    }

    ESP_LOGI(TAG, "Application initialized successfully");
    return 0;
}

int application_start(application_t* app) {
    if (app == NULL) return -1;
    if (app->is_running) {
        ESP_LOGW(TAG, "Application already running");
        return 0;
    }

    ESP_LOGI(TAG, "Starting application...");

    // Set state to connecting
    set_state(app, APP_STATE_WIFI_CONNECTING);

    // Start WiFi connection
    if (app->config.wifi_ssid != NULL && strlen(app->config.wifi_ssid) > 0) {
        ESP_LOGI(TAG, "Connecting to WiFi: %s", app->config.wifi_ssid);

        // Register WiFi event handler
        esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, app);
        esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, app);

        // Configure and start WiFi
        wifi_config_t wifi_config = {0};
        strncpy((char*)wifi_config.sta.ssid, app->config.wifi_ssid, sizeof(wifi_config.sta.ssid));
        strncpy((char*)wifi_config.sta.password, app->config.wifi_password, sizeof(wifi_config.sta.password));
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        // Wait for WiFi connection (with timeout)
        EventBits_t bits = xEventGroupWaitBits(
            s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAILED_BIT,
            pdFALSE,
            pdFALSE,
            pdMS_TO_TICKS(30000)  // 30 second timeout
        );

        if (bits & WIFI_CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi connected");
            set_state(app, APP_STATE_WIFI_CONNECTED);
        } else {
            ESP_LOGE(TAG, "WiFi connection failed");
            // Continue anyway for testing
            set_state(app, APP_STATE_WIFI_CONNECTED);  // Continue for demo
        }
    } else {
        ESP_LOGW(TAG, "No WiFi credentials, skipping WiFi connection");
        set_state(app, APP_STATE_WIFI_CONNECTED);
    }

    // Start wake word detection
    if (app->wake_word != NULL) {
        wake_word_start(app->wake_word);
        ESP_LOGI(TAG, "Wake word started");
    }

    // Create main application task
    BaseType_t ret = xTaskCreate(
        application_task,
        "app_main_task",
        8192,
        app,
        5,
        &app->task_handle
    );

    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create application task");
        return -1;
    }

    // Set display brightness
    if (app->display_initialized) {
        display_set_brightness(app->display, 100);
        display_clear(app->display);
        display_draw_string(app->display, 0, 0, "Agent HAL v1.0");
        display_flush(app->display);
    }

    app->is_running = true;
    ESP_LOGI(TAG, "Application started successfully");
    return 0;
}

void application_stop(application_t* app) {
    if (app == NULL) return;
    if (!app->is_running) {
        ESP_LOGW(TAG, "Application not running");
        return;
    }

    ESP_LOGI(TAG, "Stopping application...");

    // Stop wake word
    if (app->wake_word != NULL) {
        wake_word_stop(app->wake_word);
    }

    // Stop audio
    if (app->audio_mgr != NULL) {
        audio_manager_stop_recording(app->audio_mgr);
        audio_manager_stop_playback(app->audio_mgr);
    }

    // Disconnect protocol
    if (app->protocol != NULL) {
        protocol_disconnect(app->protocol);
    }

    // Stop WiFi
    esp_wifi_stop();

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
    if (app == NULL) return;

    // Poll protocol for incoming data
    if (app->protocol != NULL) {
        protocol_poll(app->protocol);
    }

    // Process OTA if updating
    if (app->ota != NULL) {
        ota_state_t ota_state = ota_update_get_state(app->ota);
        if (ota_state == OTA_STATE_DOWNLOADING || ota_state == OTA_STATE_FLASHING) {
            // OTA in progress - handled in task
        }
    }
}

int application_update_config(application_t* app, const device_config_t* config) {
    if (app == NULL || config == NULL) return -1;

    // Free old config
    if (app->config.device_name != NULL) free((void*)app->config.device_name);
    if (app->config.wifi_ssid != NULL) free((void*)app->config.wifi_ssid);
    if (app->config.wifi_password != NULL) free((void*)app->config.wifi_password);
    if (app->config.server_url != NULL) free((void*)app->config.server_url);
    if (app->config.protocol_type != NULL) free((void*)app->config.protocol_type);
    if (app->config.language != NULL) free((void*)app->config.language);

    // Copy new config
    app->config.device_name = strdup(config->device_name ?: "");
    app->config.wifi_ssid = strdup(config->wifi_ssid ?: "");
    app->config.wifi_password = strdup(config->wifi_password ?: "");
    app->config.server_url = strdup(config->server_url ?: "");
    app->config.protocol_type = strdup(config->protocol_type ?: "websocket");
    app->config.language = strdup(config->language ?: "zh-CN");
    app->config.volume = config->volume;
    app->config.wake_word_enabled = config->wake_word_enabled;

    // Apply volume change
    if (app->audio_mgr != NULL) {
        audio_manager_set_volume(app->audio_mgr, app->config.volume);
    }

    // Signal configuration update if running
    if (app->is_running) {
        xEventGroupSetBits(app->event_group, APP_EVENT_CONFIG_UPDATE);
    }

    ESP_LOGI(TAG, "Configuration updated");
    return 0;
}

app_state_t application_get_state(const application_t* app) {
    if (app == NULL) return APP_STATE_ERROR;
    return app->state;
}

const char* application_get_version(const application_t* app) {
    if (app == NULL) return NULL;
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
        APP_EVENT_ERROR |
        APP_EVENT_WIFI_CONNECTED |
        APP_EVENT_SERVER_CONNECTED;

    ESP_LOGI(TAG, "Application task started");

    // Signal init done
    xEventGroupSetBits(app->event_group, APP_EVENT_INIT_DONE);

    // Connect to server
    if (app->protocol != NULL) {
        set_state(app, APP_STATE_SERVER_CONNECTING);
        int ret = protocol_connect(app->protocol);
        if (ret == 0) {
            ESP_LOGI(TAG, "Protocol connected");
            set_state(app, APP_STATE_RUNNING);
        } else {
            ESP_LOGW(TAG, "Protocol connection failed, will retry");
        }
    }

    while (true) {
        EventBits_t bits = xEventGroupWaitBits(
            app->event_group,
            all_events,
            pdTRUE,
            pdFALSE,
            pdMS_TO_TICKS(100)  // Short timeout for responsive handling
        );

        if (bits & APP_EVENT_STOP) {
            ESP_LOGI(TAG, "Stop event received");
            break;
        }

        if (bits & APP_EVENT_CONFIG_UPDATE) {
            ESP_LOGI(TAG, "Config update event");
            // Handle config update if needed
        }

        // Also process protocol in the loop
        if (app->protocol != NULL && app->is_running) {
            protocol_poll(app->protocol);
        }
    }

    // Notify exit
    xTaskNotifyGive(xTaskGetCurrentTaskHandle());
    vTaskDelete(NULL);
}

// WiFi event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    application_t* app = (application_t*)arg;

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "WiFi connected to AP");
                xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "WiFi disconnected");
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAILED_BIT);
                break;
            default:
                break;
        }
    } else if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP:
                ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
                ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
                xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                break;
            default:
                break;
        }
    }
}

// Wake word callback
static void on_wake_word_detected(const char* wake_word, void* user_data) {
    application_t* app = (application_t*)user_data;
    if (app == NULL) return;

    ESP_LOGI(TAG, "Wake word detected: %s", wake_word);

    // Change LED to listening state
    if (app->led != NULL) {
        led_color_t color = {0, 255, 0};  // Green
        led_control_set_color(app->led, &color);
    }

    // Update state machine
    if (app->state_machine != NULL) {
        device_state_machine_transition(app->state_machine, DEVICE_STATE_LISTENING);
    }
}

// Protocol callbacks
static void on_protocol_connected(void* user_data) {
    application_t* app = (application_t*)user_data;
    if (app == NULL) return;

    ESP_LOGI(TAG, "Protocol connected to server");
    xEventGroupSetBits(app->event_group, APP_EVENT_SERVER_CONNECTED);
    set_state(app, APP_STATE_RUNNING);

    // Update LED
    if (app->led != NULL) {
        led_color_t color = {0, 0, 255};  // Blue
        led_control_set_color(app->led, &color);
    }
}

static void on_protocol_error(const char* message, void* user_data) {
    application_t* app = (application_t*)user_data;
    if (app == NULL) return;

    ESP_LOGE(TAG, "Protocol error: %s", message);

    // Set error state
    if (app->error_message != NULL) free(app->error_message);
    app->error_message = strdup(message);
    xEventGroupSetBits(app->event_group, APP_EVENT_ERROR);

    // Update LED to error
    if (app->led != NULL) {
        led_color_t color = {255, 0, 0};  // Red
        led_control_set_color(app->led, &color);
    }
}

static void on_protocol_text(const char* text, void* user_data) {
    application_t* app = (application_t*)user_data;
    if (app == NULL || text == NULL) return;

    ESP_LOGI(TAG, "Received text: %s", text);

    // Display on screen
    if (app->display_initialized) {
        display_clear(app->display);
        display_draw_string(app->display, 0, 0, text);
        display_flush(app->display);
    }

    // Change LED to speaking state
    if (app->led != NULL) {
        led_color_t color = {255, 255, 0};  // Yellow
        led_control_set_color(app->led, &color);
    }

    // Update state
    if (app->state_machine != NULL) {
        device_state_machine_transition(app->state_machine, DEVICE_STATE_SPEAKING);
    }
}
