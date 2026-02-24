/*
 * Agent HAL - Main Entry Point (Pure C Version)
 *
 * This is the pure C version of the main entry point.
 * Use this instead of main.cc for pure C builds.
 */

#include <esp_log.h>
#include <esp_err.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_event.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "application.h"

#define TAG "main"

// 默认设备配置
static const device_config_t default_config = {
    .device_name = "agent_hal",
    .wifi_ssid = "",
    .wifi_password = "",
    .server_url = "wss://api.xiaozhi.ai/",
    .protocol_type = "websocket",
    .language = "zh-CN",
    .volume = 80,
    .wake_word_enabled = true
};

void app_main(void)
{
    // Initialize NVS flash for WiFi configuration
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash to fix corruption");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Agent HAL starting (Pure C Version)...");

    // Create application instance
    application_t* app = application_create(&default_config);
    if (app == NULL) {
        ESP_LOGE(TAG, "Failed to create application");
        return;
    }

    // Initialize application
    ret = application_init(app);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to initialize application: %d", ret);
        application_destroy(app);
        return;
    }

    // Start application
    ret = application_start(app);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to start application: %d", ret);
        application_destroy(app);
        return;
    }

    ESP_LOGI(TAG, "Agent HAL started successfully");

    // Main event loop - process events
    while (1) {
        application_process(app);
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms tick
    }

    // Cleanup (never reached in normal operation)
    application_stop(app);
    application_destroy(app);
}
