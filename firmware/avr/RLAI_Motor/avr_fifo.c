#include "include/avr_fifo.h"

void new_fifo(struct fifo fi, uint8_t *mem, uint8_t size) {

	fi.mem = fi.head = fi.tail = mem;
	fi.size = size;
	return;	
}

uint8_t queue(struct fifo fi, uint8_t data){
	
	uint8_t *tmp;

	*(fi.head) = data;
	tmp = fi.head + 1;	
	if((tmp - fi.mem) >= fi.size)
		tmp = fi.mem;
	if(tmp == fi.tail)
		return 1;
	fi.head = tmp;
	return 0;
}

uint8_t dequeue(struct fifo fi , uint8_t* data){

	if(fi.head == fi.tail)
		return 1;
	*data = *(fi.tail++);
	if((fi.tail - fi.mem) >= fi.size)
		fi.tail = fi.mem;
	return 0;
}
