//
// Created by HZQ on 2026/9/21.
//

#include <windows.h>

#include "core_buffer.h"


typedef struct RingBuffer {
    int size;
    char *buffer;
    volatile int read_index;
    volatile int write_index;
} RingBuffer;


typedef struct Buffer {
    int type;
    union {
        RingBuffer data;
    };
    CRITICAL_SECTION cs;
} Buffer;

void create_buffer(Buffer *buffer, const int type, const int size) {
    if (buffer == NULL) {
        return;
    }

    int l;
    if (size <= MINIMAL_BUFFER_SIZE) {
        l = MINIMAL_BUFFER_SIZE;
    } else {
        l = size;
    }

    switch (type) {
        case CORE_BUFFER_RING: {
            // 环形缓冲区
            // 需要额外长度判断是否占满
            l ++;
            const RingBuffer data = {l, malloc(sizeof(char) * l), 0, 0};
            buffer -> type = CORE_BUFFER_RING;
            buffer -> data = data;
            InitializeCriticalSection(&buffer -> cs);
            break;
        }
        default: {
            // 未知缓冲区
            buffer -> type = CORE_BUFFER_UNKNOWN;
            break;
        }
    }
}

void release_buffer(Buffer *buffer) {
    if (buffer == NULL) {
        return;
    }

    switch (buffer -> type) {
        case CORE_BUFFER_RING: {
            // 环形缓冲区
            free(buffer -> data.buffer);
            buffer -> data.buffer = NULL;
            DeleteCriticalSection(&buffer -> cs);
            buffer -> type = CORE_BUFFER_UNKNOWN;
            break;
        }

        case CORE_BUFFER_UNKNOWN:
        default: {
            // 未知缓冲区
            break;
        }
    }
}

void clear_buffer(Buffer *buffer) {
    switch (buffer -> type) {
        case CORE_BUFFER_RING: {
            // 环形缓冲区
            buffer ->data.read_index = buffer -> data.write_index;
            break;
        }

        case CORE_BUFFER_UNKNOWN:
        default: {
            // 未知缓冲区
            break;
        }
    }
}

// 获得总大小
int get_size(Buffer *buffer) {
    switch (buffer -> type) {
        case CORE_BUFFER_RING: {
            // 环形缓冲区
            EnterCriticalSection(&buffer -> cs);
            const int size = buffer -> data.size - 1;
            LeaveCriticalSection(&buffer -> cs);
            return size;
        }

        case CORE_BUFFER_UNKNOWN:
        default: {
            // 未知缓冲区
            return -1;
        }
    }
}

// 获得剩余大小
int get_remaining_size(Buffer *buffer) {
    switch (buffer -> type) {
        case CORE_BUFFER_RING: {
            // 环形缓冲区
            EnterCriticalSection(&buffer -> cs);
            const int remaining_size = (buffer -> data.read_index - buffer -> data.write_index + buffer -> data.size - 1) / buffer -> data.size;
            LeaveCriticalSection(&buffer -> cs);
            return remaining_size;
        }

        case CORE_BUFFER_UNKNOWN:
        default: {
            // 未知缓冲区
            return -1;
        }
    }
}

// 获得已使用的大小
int get_used_size(Buffer *buffer) {
    switch (buffer -> type) {
        case CORE_BUFFER_RING: {
            // 环形缓冲区
            EnterCriticalSection(&buffer -> cs);
            const int used_size = (buffer -> data.write_index - buffer -> data.read_index + buffer -> data.size) / buffer -> data.size;
            LeaveCriticalSection(&buffer -> cs);
            return used_size;
        }

        case CORE_BUFFER_UNKNOWN:
        default: {
            // 未知缓冲区
            return -1;
        }
    }
}
