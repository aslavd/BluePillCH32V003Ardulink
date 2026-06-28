
/*
simple FIFO buffer
*/

#include <string.h>
#include "VSL_Buffer.h"

//======================================================================
void rb_init(RingBuffer *rb, int16_t capacity)
{
	//rb->capacity = capacity;
	rb->capacity = 256;
	rb->count = 0;
	rb->error = 0;
}

//======================================================================
uint8_t rb_is_empty(RingBuffer *rb)
{
	return rb->count == 0;
}

//======================================================================
uint8_t rb_write(RingBuffer *rb, const void *src, int16_t size)
{
    if (size == 0 || size > rb->capacity) return 0;

    const uint8_t *src_ptr = (const uint8_t *)src;

    // Limit write to available space
    int16_t available_space = rb->capacity - rb->count;

    // shift left, unread bytes at left will be lost
    if (size > available_space)
    {
    	rb->error = 1;
    	int16_t shift_amount =  size - available_space;
    	// shift left
    	memcpy(rb->buffer, rb->buffer + shift_amount, rb->capacity - shift_amount);
    	rb->count -= shift_amount;
    	if (rb->count < 0) rb->count = 0;
    }

    // write bytes to buffer
    memcpy(rb->buffer + rb->count, src_ptr, size);

    rb->count += size;

    return size;
}

//======================================================================
uint8_t rb_read_byte(RingBuffer *rb)
{
   uint8_t rez = 0;
   if (rb->count == 0) return rez;

   rez = rb->buffer[0];
   memcpy(rb->buffer, rb->buffer + 1, rb->capacity - 1);
   rb->count --;

   // debug
   //rb->buffer[rb->count] = 0;

   return rez;
}

//======================================================================
uint8_t rb_read(RingBuffer *rb, void *dest, int16_t size)
{
    if (size == 0 || rb_is_empty(rb)) return 0;

    // Limit read to available data
    if (size > rb->count) {
        size = rb->count;
    }

    uint8_t *dest_ptr = (uint8_t *)dest;

    memcpy(dest_ptr, rb->buffer, size);
    // shift left
    int16_t shift_amount = rb->capacity - size;
    if (shift_amount > 0)
      memcpy(rb->buffer, rb->buffer + size, shift_amount);

    rb->count -= size;

    // debug
    //rb->buffer[rb->count] = 0;

    return size;
}
