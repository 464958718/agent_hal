#include "memory_pool.h"
#include <stdlib.h>
#include <string.h>

// 内存块头部
typedef struct memory_block_header {
    memory_pool_t* pool;
    bool in_use;
} memory_block_header_t;

struct memory_pool {
    void* buffer;
    size_t block_size;
    size_t block_count;
    size_t free_count;
    void* free_list;
};

// 对齐到 8 字节
#define ALIGN_8(x) (((x) + 7) & ~7)

memory_pool_t* memory_pool_create(const memory_pool_config_t* config) {
    if (config == NULL || config->block_size == 0 || config->block_count == 0) {
        return NULL;
    }

    memory_pool_t* pool = malloc(sizeof(memory_pool_t));
    if (pool == NULL) {
        return NULL;
    }

    size_t aligned_block_size = ALIGN_8(config->block_size + sizeof(memory_block_header_t));
    size_t total_size = aligned_block_size * config->block_count;

    pool->buffer = malloc(total_size);
    if (pool->buffer == NULL) {
        free(pool);
        return NULL;
    }

    pool->block_size = aligned_block_size;
    pool->block_count = config->block_count;
    pool->free_count = config->block_count;
    pool->free_list = pool->buffer;

    // 初始化空闲链表
    char* block = (char*)pool->buffer;
    for (size_t i = 0; i < config->block_count; i++) {
        memory_block_header_t* header = (memory_block_header_t*)block;
        header->pool = pool;
        header->in_use = false;

        // 将当前块指向下一个块
        char* next = block + aligned_block_size;
        if (i < config->block_count - 1) {
            *(char**)block = next;
        } else {
            *(char**)block = NULL;
        }

        block = next;
    }

    return pool;
}

void memory_pool_destroy(memory_pool_t* pool) {
    if (pool == NULL) {
        return;
    }
    free(pool->buffer);
    free(pool);
}

void* memory_pool_alloc(memory_pool_t* pool) {
    if (pool == NULL || pool->free_list == NULL) {
        return NULL;
    }

    char* block = (char*)pool->free_list;
    memory_block_header_t* header = (memory_block_header_t*)block;

    pool->free_list = *(char**)block;
    header->in_use = true;
    pool->free_count--;

    return block + sizeof(memory_block_header_t);
}

void memory_pool_free(memory_pool_t* pool, void* ptr) {
    if (pool == NULL || ptr == NULL) {
        return;
    }

    char* block = (char*)ptr - sizeof(memory_block_header_t);
    memory_block_header_t* header = (memory_block_header_t*)block;

    if (!header->in_use || header->pool != pool) {
        return;
    }

    header->in_use = false;
    *(char**)block = pool->free_list;
    pool->free_list = block;
    pool->free_count++;
}

size_t memory_pool_available(memory_pool_t* pool) {
    return pool ? pool->free_count : 0;
}

size_t memory_pool_block_size(memory_pool_t* pool) {
    return pool ? (pool->block_size - sizeof(memory_block_header_t)) : 0;
}

bool memory_pool_is_valid(memory_pool_t* pool) {
    return pool != NULL && pool->buffer != NULL;
}
