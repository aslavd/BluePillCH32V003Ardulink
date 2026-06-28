

/*
simple FIFO buffer
*/

#ifndef INC_VSL_BUFFER_H_
#define INC_VSL_BUFFER_H_

#include "inttypes.h"

typedef struct {
    uint8_t buffer[256];
    int16_t capacity;
    int16_t count;
    uint8_t error;
} RingBuffer;


void rb_init(RingBuffer *rb, int16_t capacity);
uint8_t rb_is_empty(RingBuffer *rb);
uint8_t rb_write(RingBuffer *rb, const void *src, int16_t size);
uint8_t rb_read_byte(RingBuffer *rb);
uint8_t rb_read(RingBuffer *rb, void *dest, int16_t size);


#endif /* INC_VSL_BUFFER_H_ */
