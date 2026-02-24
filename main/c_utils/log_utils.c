#include "log_utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static log_config_t g_log_config = {
    .level = LOG_LEVEL_INFO,
    .output_fn = NULL,
    .use_colors = true,
    .print_timestamp = false
};

static const char* g_level_strings[] = {
    "NONE",
    "E",
    "W",
    "I",
    "D",
    "V"
};

static const char* g_level_colors[] = {
    "",
    "\033[31m",  // ERROR - red
    "\033[33m",  // WARN - yellow
    "\033[32m",  // INFO - green
    "\033[36m",  // DEBUG - cyan
    "\033[90m"   // VERBOSE - gray
};

static const char* g_level_color_reset = "\033[0m";

void log_init(const log_config_t* config) {
    if (config == NULL) {
        return;
    }
    memcpy(&g_log_config, config, sizeof(log_config_t));
}

void log_set_level(log_level_t level) {
    g_log_config.level = level;
}

log_level_t log_get_level(void) {
    return g_log_config.level;
}

static void log_print(log_level_t level, const char* tag, const char* fmt, va_list args) {
    if (level > g_log_config.level) {
        return;
    }

    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (g_log_config.output_fn != NULL) {
        g_log_config.output_fn(level, tag, buffer);
    } else {
        if (g_log_config.use_colors) {
            printf("%s[%s][%s] %s%s\n",
                   g_level_colors[level],
                   g_level_strings[level],
                   tag,
                   buffer,
                   g_level_color_reset);
        } else {
            printf("[%s][%s] %s\n",
                   g_level_strings[level],
                   tag,
                   buffer);
        }
    }
}

void log_error(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_print(LOG_LEVEL_ERROR, tag, fmt, args);
    va_end(args);
}

void log_warn(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_print(LOG_LEVEL_WARN, tag, fmt, args);
    va_end(args);
}

void log_info(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_print(LOG_LEVEL_INFO, tag, fmt, args);
    va_end(args);
}

void log_debug(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_print(LOG_LEVEL_DEBUG, tag, fmt, args);
    va_end(args);
}

void log_verbose(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_print(LOG_LEVEL_VERBOSE, tag, fmt, args);
    va_end(args);
}
