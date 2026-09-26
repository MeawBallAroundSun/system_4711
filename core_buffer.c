//
// Created by HZQ on 2026/9/21.
//

#include <windows.h>

#include "core_buffer.h"

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
            EnterCriticalSection(&buffer -> cs);
            buffer ->ring.read_index = buffer -> ring.write_index;
            LeaveCriticalSection(&buffer -> cs);
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
            const int remaining_size = (buffer -> ring.read_index - buffer -> ring.write_index + buffer -> ring.size - 1) % buffer -> ring.size;
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
            const int used_size = (buffer -> ring.write_index - buffer -> ring.read_index + buffer -> ring.size) % buffer -> ring.size;
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



// 向环形缓冲区中写入一个char
static void write_char_to_ring_buffer(RingBuffer *buffer, const char c) {
    const int remaining_size = (buffer -> read_index - buffer -> write_index + buffer -> size - 1) % buffer -> size;
    if (remaining_size == 0) {
        int utf_8_length = 1;
        const char first_char = buffer -> buffer[buffer -> read_index];
        if ((first_char & 0x80) == 0x00) {
            utf_8_length = 1;
        } else if ((first_char & 0xE0) == 0xC0) {
            utf_8_length = 2;
        } else if ((first_char & 0xF0) == 0xE0) {
            utf_8_length = 3;
        } else if ((first_char & 0xF8) == 0xF0) {
            utf_8_length = 4;
        } else if ((first_char & 0xFC) == 0xF8) {
            utf_8_length = 5;
        } else if ((first_char & 0xFE) == 0xFC) {
            utf_8_length = 6;
        }


        for (int i = 0; i < utf_8_length; i ++) {
            buffer -> read_index = (buffer -> read_index + 1) % buffer -> size;
        }
    }
    buffer -> buffer[buffer -> write_index] = c;
    buffer -> write_index = (buffer -> write_index + 1) % buffer -> size;
}

// 向缓冲区中写入内容，返回实际写入的char数（不加入'\0'）
int write_buffer(Buffer *buffer, const char *string, const int length) {
    switch (buffer -> type) {
        case CORE_BUFFER_RING: {
            // 环形缓冲区
            EnterCriticalSection(&buffer -> cs);
            int write_count = 0;
            for (int i = 0; i < length; i ++, write_count ++) {
                if (string[i] == '\0') {
                    break;
                }
                write_char_to_ring_buffer(&buffer -> ring, string[i]);
            }
            LeaveCriticalSection(&buffer -> cs);
            return write_count;
        }

        case CORE_BUFFER_UNKNOWN:
        default: {
            // 未知缓冲区
            return -1;
        }
    }
}

// 从环形缓冲区中读取一个char
static void read_char_from_ring_buffer(RingBuffer *buffer, char *c) {
    const int used_size = (buffer -> write_index - buffer -> read_index + buffer -> size) % buffer -> size;
    if (used_size == 0) {
        *c = '\0';
    } else {
        *c = buffer -> buffer[buffer -> read_index];
        buffer -> read_index = (buffer -> read_index + 1) % buffer -> size;
    }
}

// 从缓冲区中读取内容，返回读取的长度（会自动补上'\0'，不计入长度）
int read_buffer(Buffer *buffer, char *string, const int length, const char consume) {
    if (length <= 0) {
        return -1;
    }
    switch (buffer -> type) {
        case CORE_BUFFER_RING: {
            // 环形缓冲区
            EnterCriticalSection(&buffer -> cs);
            int write_count = 0;
            if (consume) {
                // 消费模式
                while (write_count < length - 1) {
                    char c;
                    read_char_from_ring_buffer(&buffer -> ring, &c);
                    if (c == '\0') {
                        break;
                    }
                    *string = c;
                    string ++, write_count ++;
                }
            } else {
                // 阅览模式
                const int used_size = (buffer -> ring.write_index - buffer -> ring.read_index + buffer -> ring.size) % buffer -> ring.size;
                int l;
                if (used_size < length - 1) {
                    l = used_size;
                } else {
                    l = length - 1;
                }

                int i = buffer -> ring.read_index;
                while (write_count < l) {
                    *string = buffer -> ring.buffer[i];
                    string ++, i = (i + 1) % buffer ->ring.size, write_count ++;
                }
            }

            *string = '\0';
            LeaveCriticalSection(&buffer -> cs);
            return write_count;
        }

        case CORE_BUFFER_UNKNOWN:
        default: {
            // 未知缓冲区
            return -1;
        }
    }
}