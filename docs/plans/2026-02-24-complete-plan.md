# xiaozhi-esp32 C 语言移植完整实现计划

## 项目信息

- **项目名称**: agent_hal (xiaozhi-esp32 C 语言移植)
- **目标**: 将 xiaozhi-esp32 项目从 C++ 完全重写为纯 C 代码
- **目标平台**: ESP-IDF (C 语言模式)
- **编码风格**: snake_case (小写下划线)
- **仓库**: git@github.com:464958718/agent_hal.git

---

## 已完成阶段

### 阶段 1: 基础工具模块 ✅

| 模块 | 文件 | 功能 |
|------|------|------|
| string_utils | `main/c_utils/string_utils.h`<br>`main/c_utils/string_utils.c` | 动态字符串管理，自动容量增长，内存安全 |
| memory_pool | `main/c_utils/memory_pool.h`<br>`main/c_utils/memory_pool.c` | 固定块内存池，8字节对齐，空闲链表 |
| event_system | `main/c_utils/event_system.h`<br>`main/c_utils/event_system.c` | 事件发布/订阅系统，位掩码过滤 |
| log_utils | `main/c_utils/log_utils.h`<br>`main/c_utils/log_utils.c` | 日志工具，支持多级别和颜色输出 |

### 阶段 2: 协议层 ✅

| 模块 | 文件 | 功能 |
|------|------|------|
| protocol | `main/protocols/protocol.h`<br>`main/protocols/protocol.c` | 协议接口基类，函数指针表模拟虚函数 |
| mqtt_protocol | `main/protocols/mqtt_protocol.h`<br>`main/protocols/mqtt_protocol.c` | MQTT 协议实现 (ESP-IDF 占位) |
| websocket_protocol | `main/protocols/websocket_protocol.h`<br>`main/protocols/websocket_protocol.c` | WebSocket 协议实现 (ESP-IDF 占位) |

---

## 待实现阶段

### 阶段 3: 显示层

#### Task 3.1: display 显示接口定义

**Files:**
- Create: `main/display/display.h`
- Create: `main/display/display.c`

```c
// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct display display_t;

// 显示操作接口
typedef struct display_ops {
    int (*init)(display_t* disp);
    void (*deinit)(display_t* disp);
    int (*clear)(display_t* disp);
    int (*draw_string)(display_t* disp, int x, int y, const char* str);
    int (*draw_pixel)(display_t* disp, int x, int y, uint32_t color);
    int (*draw_rect)(display_t* disp, int x, int y, int w, int h, uint32_t color);
    int (*draw_circle)(display_t* disp, int x, int y, int radius, uint32_t color);
    int (*draw_image)(display_t* disp, int x, int y, const void* img_data, int w, int h);
    int (*set_brightness)(display_t* disp, int brightness);
    int (*flush)(display_t* disp);
} display_ops_t;

struct display {
    const display_ops_t* ops;
    void* impl;
    int width;
    int height;
    bool initialized;
};

// 显示类型
typedef enum {
    DISPLAY_TYPE_NONE,
    DISPLAY_TYPE_OLED,
    DISPLAY_TYPE_LCD,
    DISPLAY_TYPE_LVGL,
    DISPLAY_TYPE_EMOTE
} display_type_t;

// 公共 API
display_t* display_create(display_type_t type);
void display_destroy(display_t* disp);
int display_init(display_t* disp);
void display_deinit(display_t* disp);
int display_clear(display_t* disp);
int display_draw_string(display_t* disp, int x, int y, const char* str);
int display_set_brightness(display_t* disp, int brightness);
int display_flush(display_t* disp);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H
```

#### Task 3.2: oled_display OLED 显示驱动

**Files:**
- Create: `main/display/oled_display.h`
- Create: `main/display/oled_display.c`

#### Task 3.3: lcd_display LCD 显示驱动

**Files:**
- Create: `main/display/lcd_display.h`
- Create: `main/display/lcd_display.c`

#### Task 3.4: emote_display 表情显示

**Files:**
- Create: `main/display/emote_display.h`
- Create: `main/display/emote_display.c`

---

### 阶段 4: 音频层

#### Task 4.1: audio_manager 音频管理模块

**Files:**
- Create: `main/audio/audio_manager.h`
- Create: `main/audio/audio_manager.c`

```c
// audio_manager.h
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// 音频流数据包
typedef struct {
    uint8_t* data;
    size_t len;
    uint32_t timestamp;
    bool is_text;
} audio_stream_packet_t;

// 音频管理器
typedef struct audio_manager audio_manager_t;

// 音频回调类型
typedef void (*audio_data_callback_t)(const uint8_t* data, size_t len, void* user_data);
typedef void (*wake_word_callback_t)(const char* wake_word, void* user_data);
typedef void (*vad_callback_t)(bool speaking, void* user_data);

// 音频管理器 API
audio_manager_t* audio_manager_create(void);
void audio_manager_destroy(audio_manager_t* mgr);
int audio_manager_init(audio_manager_t* mgr);
void audio_manager_deinit(audio_manager_t* mgr);

// 录音控制
int audio_manager_start_recording(audio_manager_t* mgr);
int audio_manager_stop_recording(audio_manager_t* mgr);
bool audio_manager_is_recording(const audio_manager_t* mgr);

// 播放控制
int audio_manager_play_audio(audio_manager_t* mgr, const uint8_t* data, size_t len);
int audio_manager_stop_playback(audio_manager_t* mgr);
bool audio_manager_is_playing(const audio_manager_t* mgr);

// 音量控制
int audio_manager_set_volume(audio_manager_t* mgr, int volume);
int audio_manager_get_volume(const audio_manager_t* mgr);

// 回调注册
void audio_manager_set_wake_word_callback(audio_manager_t* mgr,
    wake_word_callback_t cb, void* user_data);
void audio_manager_set_vad_callback(audio_manager_t* mgr,
    vad_callback_t cb, void* user_data);
void audio_manager_set_audio_data_callback(audio_manager_t* mgr,
    audio_data_callback_t cb, void* user_data);

// 状态查询
bool audio_manager_is_initialized(const audio_manager_t* mgr);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_MANAGER_H
```

#### Task 4.2: audio_codec 音频编解码接口

**Files:**
- Create: `main/audio/audio_codec.h`
- Create: `main/audio/audio_codec.c`

#### Task 4.3: wake_word 离线语音唤醒

**Files:**
- Create: `main/audio/wake_word.h`
- Create: `main/audio/wake_word.c`

---

### 阶段 5: 设备状态机

#### Task 5.1: device_state_machine 设备状态机

**Files:**
- Create: `main/device_state_machine.h`
- Create: `main/device_state_machine.c`

```c
// device_state_machine.h
#ifndef DEVICE_STATE_MACHINE_H
#define DEVICE_STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 设备状态枚举
typedef enum {
    DEVICE_STATE_STARTING = 0,
    DEVICE_STATE_WIFI_CONFIGURING,
    DEVICE_STATE_IDLE,
    DEVICE_STATE_LISTENING,
    DEVICE_STATE_SPEAKING,
    DEVICE_STATE_ERROR,
    DEVICE_STATE_MAX
} device_state_t;

// 状态变更回调
typedef void (*state_change_callback_t)(device_state_t old_state,
    device_state_t new_state, void* user_data);

// 状态机结构
typedef struct device_state_machine device_state_machine_t;

// 状态机 API
device_state_machine_t* device_state_machine_create(void);
void device_state_machine_destroy(device_state_machine_t* sm);

// 状态转换
int device_state_machine_transition(device_state_machine_t* sm,
    device_state_t new_state);
device_state_t device_state_machine_get_state(const device_state_machine_t* sm);

// 状态监听
int device_state_machine_add_listener(device_state_machine_t* sm,
    state_change_callback_t callback, void* user_data);

// 便捷函数
bool device_state_machine_is_idle(const device_state_machine_t* sm);
bool device_state_machine_is_active(const device_state_machine_t* sm);

#ifdef __cplusplus
}
#endif

#endif // DEVICE_STATE_MACHINE_H
```

---

### 阶段 6: LED 控制

#### Task 6.1: led_control LED 控制模块

**Files:**
- Create: `main/led/led_control.h`
- Create: `main/led/led_control.c`

```c
// led_control.h
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// LED 类型
typedef enum {
    LED_TYPE_NONE,
    LED_TYPE_GPIO,
    LED_TYPE_RGB,
    LED_TYPE_WS2812,
    LED_TYPE_CIRCULAR
} led_type_t;

// LED 颜色
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_color_t;

// LED 效果
typedef enum {
    LED_EFFECT_OFF,
    LED_EFFECT_ON,
    LED_EFFECT_BREATHE,
    LED_EFFECT_BLINK,
    LED_EFFECT_RAINBOW,
    LED_EFFECT_CUSTOM
} led_effect_t;

// LED 控制结构
typedef struct led_control led_control_t;

// LED 控制 API
led_control_t* led_control_create(led_type_t type);
void led_control_destroy(led_control_t* led);
int led_control_init(led_control_t* led);
void led_control_deinit(led_control_t* led);

// LED 操作
int led_control_set_color(led_control_t* led, const led_color_t* color);
int led_control_set_brightness(led_control_t* led, uint8_t brightness);
int led_control_set_effect(led_control_t* led, led_effect_t effect);
int led_control_clear(led_control_t* led);

// 动画控制
int led_control_start_animation(led_control_t* led, uint32_t duration_ms);
int led_control_stop_animation(led_control_t* led);

#ifdef __cplusplus
}
#endif

#endif // LED_CONTROL_H
```

---

### 阶段 7: OTA 升级

#### Task 7.1: ota_update OTA 升级模块

**Files:**
- Create: `main/ota_update.h`
- Create: `main/ota_update.c`

```c
// ota_update.h
#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// OTA 状态
typedef enum {
    OTA_STATE_IDLE,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_FLASHING,
    OTA_STATE_REBOOTING,
    OTA_STATE_ERROR
} ota_state_t;

// OTA 进度回调
typedef void (*ota_progress_callback_t)(int progress, void* user_data);

// OTA 错误回调
typedef void (*ota_error_callback_t)(int error_code, const char* message, void* user_data);

// OTA 控制结构
typedef struct ota_update ota_update_t;

// OTA API
ota_update_t* ota_update_create(void);
void ota_update_destroy(ota_update_t* ota);

// OTA 操作
int ota_update_check_version(ota_update_t* ota, const char* version_url);
int ota_update_start(ota_update_t* ota, const char* url);
int ota_update_cancel(ota_update_t* ota);

// 状态查询
ota_state_t ota_update_get_state(const ota_update_t* ota);
const char* ota_update_get_current_version(const ota_update_t* ota);
const char* ota_update_get_latest_version(const ota_update_t* ota);

// 回调注册
void ota_update_set_progress_callback(ota_update_t* ota,
    ota_progress_callback_t cb, void* user_data);
void ota_update_set_error_callback(ota_update_t* ota,
    ota_error_callback_t cb, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // OTA_UPDATE_H
```

---

### 阶段 8: 应用层

#### Task 8.1: application 应用主程序

**Files:**
- Create: `main/application.h`
- Create: `main/application.c`

```c
// application.h
#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 设备配置
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

// 应用状态
typedef enum {
    APP_STATE_INIT,
    APP_STATE_WIFI_CONNECTING,
    APP_STATE_WIFI_CONNECTED,
    APP_STATE_SERVER_CONNECTING,
    APP_STATE_RUNNING,
    APP_STATE_ERROR
} app_state_t;

// 应用结构
typedef struct application application_t;

// 应用 API
application_t* application_create(const device_config_t* config);
void application_destroy(application_t* app);

// 生命周期
int application_init(application_t* app);
int application_start(application_t* app);
void application_stop(application_t* app);

// 事件处理
void application_process(application_t* app);

// 配置更新
int application_update_config(application_t* app, const device_config_t* config);

// 状态查询
app_state_t application_get_state(const application_t* app);
const char* application_get_version(const application_t* app);

#ifdef __cplusplus
}
#endif

#endif // APPLICATION_H
```

---

## 文件结构总览

```
agent_hal/
├── CMakeLists.txt
├── README.md
├── main/
│   ├── application.h / application.c      # 应用层
│   ├── device_state_machine.h / .c         # 设备状态机
│   ├── ota_update.h / ota_update.c         # OTA 升级
│   ├── c_utils/
│   │   ├── string_utils.h / .c            # ✅ 字符串工具
│   │   ├── memory_pool.h / .c              # ✅ 内存池
│   │   ├── event_system.h / .c            # ✅ 事件系统
│   │   └── log_utils.h / .c               # ✅ 日志工具
│   ├── protocols/
│   │   ├── protocol.h / .c                # ✅ 协议接口
│   │   ├── mqtt_protocol.h / .c           # ✅ MQTT 协议
│   │   └── websocket_protocol.h / .c      # ✅ WebSocket 协议
│   ├── display/
│   │   ├── display.h / .c                 # 显示接口
│   │   ├── oled_display.h / .c            # OLED 驱动
│   │   ├── lcd_display.h / .c            # LCD 驱动
│   │   └── emote_display.h / .c          # 表情显示
│   ├── audio/
│   │   ├── audio_manager.h / .c           # 音频管理
│   │   ├── audio_codec.h / .c             # 音频编解码
│   │   └── wake_word.h / .c               # 语音唤醒
│   └── led/
│       └── led_control.h / .c             # LED 控制
└── docs/
    └── plans/
        └── 2026-02-23-xiaozhi-c-porting-design.md
        └── 2026-02-24-complete-plan.md
```

---

## 开发建议

1. **按阶段实施**: 建议按照上述阶段顺序实施
2. **测试驱动**: 每个模块先写测试，再实现
3. **渐进式**: 可以先实现核心功能，再完善边界情况
4. **保持同步**: 定期与原 C++ 项目对比功能完整性
5. **ESP-IDF 集成**: 协议层和部分驱动需要集成 ESP-IDF API

---

## 版本历史

| 日期 | 版本 | 说明 |
|------|------|------|
| 2026-02-24 | 0.1.0 | 初始版本，完成基础工具模块和协议层 |
