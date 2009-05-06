#include <avr/io.h>

#ifndef AVR_FIFO_H
#define AVR_FIFO_H

#define MAX_FIFO_SIZE 128;

struct fifo {
	
	uint8_t *head;
	uint8_t *tail;
	uint8_t *mem;
	uint8_t size;
};

void new_fifo(struct fifo, uint8_t*, uint8_t);

uint8_t queue(struct fifo, uint8_t);

uint8_t dequeue(struct fifo, uint8_t*);

#endif
