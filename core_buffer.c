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
