#include "string_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define STRING_DEFAULT_CAPACITY 64
#define STRING_GROWTH_FACTOR 2

static int string_ensure_capacity(string_t* s, size_t needed) {
    if (s == NULL || needed <= s->capacity) {
        return 0;
    }

    size_t new_capacity = s->capacity > 0 ? s->capacity : STRING_DEFAULT_CAPACITY;
    while (new_capacity < needed) {
        // 溢出检查：确保乘以增长因子不会导致溢出
        if (new_capacity > SIZE_MAX / STRING_GROWTH_FACTOR) {
            return -1;
        }
        new_capacity *= STRING_GROWTH_FACTOR;
    }

    char* new_data = realloc(s->data, new_capacity);
    if (new_data == NULL) {
        return -1;
    }

    s->data = new_data;
    s->capacity = new_capacity;
    return 0;
}

string_t* string_create(void) {
    return string_create_capacity(STRING_DEFAULT_CAPACITY);
}

string_t* string_create_capacity(size_t capacity) {
    string_t* s = malloc(sizeof(string_t));
    if (s == NULL) {
        return NULL;
    }

    s->data = malloc(capacity);
    if (s->data == NULL) {
        free(s);
        return NULL;
    }

    s->data[0] = '\0';
    s->len = 0;
    s->capacity = capacity;
    return s;
}

string_t* string_create_from(const char* str) {
    if (str == NULL) {
        return string_create();
    }
    return string_create_from_len(str, strlen(str));
}

string_t* string_create_from_len(const char* str, size_t len) {
    if (str == NULL) {
        return string_create();
    }

    // 溢出检查：确保 len + 1 不会溢出
    if (len > SIZE_MAX - 1) {
        return NULL;
    }

    string_t* s = string_create_capacity(len + 1);
    if (s == NULL) {
        return NULL;
    }

    memcpy(s->data, str, len);
    s->data[len] = '\0';
    s->len = len;
    return s;
}

void string_destroy(string_t* s) {
    if (s == NULL) {
        return;
    }
    free(s->data);
    free(s);
}

int string_append(string_t* s, const char* str) {
    if (s == NULL || str == NULL) {
        return -1;
    }
    return string_append_len(s, str, strlen(str));
}

int string_append_len(string_t* s, const char* str, size_t len) {
    if (s == NULL || str == NULL) {
        return -1;
    }
    if (len == 0) {
        return 0;  // 空字符串追加成功
    }
    // 添加溢出检查
    if (len > SIZE_MAX - s->len - 1) {
        return -1;
    }

    if (string_ensure_capacity(s, s->len + len + 1) != 0) {
        return -1;
    }

    memcpy(s->data + s->len, str, len);
    s->len += len;
    s->data[s->len] = '\0';
    return 0;
}

int string_printf(string_t* s, const char* fmt, ...) {
    if (s == NULL || fmt == NULL) {
        return -1;
    }

    va_list args;
    va_start(args, fmt);

    // 首先尝试获取需要的缓冲区大小
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (needed < 0) {
        va_end(args);
        return -1;
    }

    // 溢出检查：确保 needed + 1 不会溢出
    if (needed > (int)(SIZE_MAX - 1)) {
        va_end(args);
        return -1;
    }

    // 确保有足够的空间
    if (string_ensure_capacity(s, needed + 1) != 0) {
        va_end(args);
        return -1;
    }

    // 实际写入
    int written = vsnprintf(s->data, s->capacity, fmt, args);
    va_end(args);

    if (written >= 0) {
        s->len = written;
    }

    return written;
}

void string_clear(string_t* s) {
    if (s == NULL) {
        return;
    }
    s->data[0] = '\0';
    s->len = 0;
}

const char* string_cstr(const string_t* s) {
    return s && s->data ? s->data : "";
}

size_t string_len(const string_t* s) {
    return s ? s->len : 0;
}

size_t string_capacity(const string_t* s) {
    return s ? s->capacity : 0;
}

bool string_empty(const string_t* s) {
    return s == NULL || s->len == 0;
}
