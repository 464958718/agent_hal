#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct memory_pool memory_pool_t;

// 内存池配置
typedef struct {
    size_t block_size;
    size_t block_count;
} memory_pool_config_t;

// 内存池 API
memory_pool_t* memory_pool_create(const memory_pool_config_t* config);
void memory_pool_destroy(memory_pool_t* pool);
void* memory_pool_alloc(memory_pool_t* pool);
void memory_pool_free(memory_pool_t* pool, void* ptr);
size_t memory_pool_available(memory_pool_t* pool);
size_t memory_pool_block_size(memory_pool_t* pool);
bool memory_pool_is_valid(memory_pool_t* pool);

#ifdef __cplusplus
}
#endif

#endif // MEMORY_POOL_H
