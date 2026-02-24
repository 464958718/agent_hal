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
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "application.h"

#define TAG "main"

// 默认设备配置 - 请根据实际情况修改 WiFi 配置
static const device_config_t default_config = {
    .device_name = "agent_hal",
    .wifi_ssid = "YOUR_WIFI_SSID",      // 修改为你的 WiFi 名称
    .wifi_password = "YOUR_WIFI_PASSWORD", // 修改为你的 WiFi 密码
    .server_url = "wss://api.xiaozhi.ai/",
    .protocol_type = "websocket",
    .language = "zh-CN",
    .volume = 80,
    .wake_word_enabled = true
};

void app_main(void)
{
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Agent HAL v1.0.0 Starting...");
    ESP_LOGI(TAG, "========================================");

    // 1. Initialize NVS flash for WiFi configuration
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash to fix corruption");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS flash initialized");

    // 2. Initialize WiFi driver
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, "WiFi driver initialized");

    // 3. Create application instance
    application_t* app = application_create(&default_config);
    if (app == NULL) {
        ESP_LOGE(TAG, "Failed to create application");
        return;
    }

    // 4. Initialize application (creates all sub-modules)
    ret = application_init(app);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to initialize application: %d", ret);
        application_destroy(app);
        return;
    }

    // 5. Start application (starts WiFi, connects to server)
    ret = application_start(app);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to start application: %d", ret);
        application_destroy(app);
        return;
    }

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Agent HAL started successfully!");
    ESP_LOGI(TAG, "========================================");

    // 6. Main event loop - process events periodically
    while (1) {
        application_process(app);
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms tick
    }

    // Cleanup (never reached in normal operation)
    application_stop(app);
    application_destroy(app);
}
