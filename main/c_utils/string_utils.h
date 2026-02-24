#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 动态字符串结构
typedef struct {
    char* data;
    size_t len;
    size_t capacity;
} string_t;

// 字符串创建/销毁
string_t* string_create(void);
string_t* string_create_capacity(size_t capacity);
string_t* string_create_from(const char* str);
string_t* string_create_from_len(const char* str, size_t len);
void string_destroy(string_t* s);

// 字符串操作
int string_append(string_t* s, const char* str);
int string_append_len(string_t* s, const char* str, size_t len);
int string_printf(string_t* s, const char* fmt, ...);
void string_clear(string_t* s);

// 属性访问
const char* string_cstr(const string_t* s);
size_t string_len(const string_t* s);
size_t string_capacity(const string_t* s);
bool string_empty(const string_t* s);

// 便捷宏
#define string_free(s) do { string_destroy(s); s = NULL; } while(0)

#ifdef __cplusplus
}
#endif

#endif // STRING_UTILS_H
