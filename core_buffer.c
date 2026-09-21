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
        RingBuffer ring;
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
            buffer -> ring = data;
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
            free(buffer -> ring.buffer);
            buffer -> ring.buffer = NULL;
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
            buffer ->ring.read_index = buffer -> ring.write_index;
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
            const int size = buffer -> ring.size - 1;
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
            const int remaining_size = (buffer -> ring.read_index - buffer -> ring.write_index + buffer -> ring.size - 1) / buffer -> ring.size;
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
            const int used_size = (buffer -> ring.write_index - buffer -> ring.read_index + buffer -> ring.size) / buffer -> ring.size;
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




static void write_char_to_ring_buffer(RingBuffer *buffer, const char c) {
    const int remaining_size = (buffer -> read_index - buffer -> write_index + buffer -> size - 1) % buffer -> size;
    if (remaining_size == 0) {
        buffer -> read_index = (buffer -> read_index + 1) % buffer -> size;
    }
    buffer -> buffer[buffer -> write_index] = c;
    buffer -> write_index = (buffer -> write_index + 1) % buffer -> size;
}

// 向数组中写入内容，返回实际写入的char数
int write_buffer(Buffer *buffer, const char *string, const int length) {
    switch (buffer -> type) {
        case CORE_BUFFER_RING: {
            // 环形缓冲区
            EnterCriticalSection(&buffer -> cs);

            int write_count = 0;
            for (int i = buffer -> ring.read_index; i < length; i++, write_count++) {
                if (string[i] == '\0') {
                    break;
                }


            }
            LeaveCriticalSection(&buffer -> cs);
        }

        case CORE_BUFFER_UNKNOWN:
        default: {
            // 未知缓冲区
            return -1;
        }
    }
}
