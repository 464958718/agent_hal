#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 日志级别
typedef enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE
} log_level_t;

// 日志输出函数类型
typedef void (*log_output_fn)(log_level_t level, const char* tag, const char* msg);

// 日志配置
typedef struct {
    log_level_t level;
    log_output_fn output_fn;
    bool use_colors;
    bool print_timestamp;
} log_config_t;

// 日志 API
void log_init(const log_config_t* config);
void log_set_level(log_level_t level);
log_level_t log_get_level(void);

void log_error(const char* tag, const char* fmt, ...);
void log_warn(const char* tag, const char* fmt, ...);
void log_info(const char* tag, const char* fmt, ...);
void log_debug(const char* tag, const char* fmt, ...);
void log_verbose(const char* tag, const char* fmt, ...);

// 简写宏
#define LOGE(tag, ...) log_error(tag, __VA_ARGS__)
#define LOGW(tag, ...) log_warn(tag, __VA_ARGS__)
#define LOGI(tag, ...) log_info(tag, __VA_ARGS__)
#define LOGD(tag, ...) log_debug(tag, __VA_ARGS__)
#define LOGV(tag, ...) log_verbose(tag, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // LOG_UTILS_H
