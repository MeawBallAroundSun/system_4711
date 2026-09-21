//
// Created by HZQ on 2026/9/21.
//

#ifndef SYSTEM_4711_CORE_BUFFER_H
#define SYSTEM_4711_CORE_BUFFER_H

#define CORE_BUFFER_UNKNOWN          0
#define CORE_BUFFER_RING            1

#define MINIMAL_BUFFER_SIZE         16

typedef struct Buffer Buffer;

typedef struct RingBuffer RingBuffer;

void create_buffer(Buffer *buffer, int type, int size);

void release_buffer(Buffer *buffer);

void clear_buffer(Buffer *buffer);

int get_size(Buffer *buffer);

int get_remaining_size(Buffer *buffer);

int get_used_size(Buffer *buffer);

int write_buffer(Buffer *buffer, const char *string, int length);





#endif //SYSTEM_4711_CORE_BUFFER_H
