#include <avr/io.h>
#include <avr/interrupt.h>
#include "include/avr_fifo.h"
#include "include/avr_motor.h"

#define SPI_PACKET_HEADER 0x7F
#define TEMP_THRESH 0

struct fifo rx_fifo;
struct fifo tx_fifo;

volatile uint16_t count, old_count;
uint8_t rstate, event, event_count;
uint8_t adc_mux;
volatile uint8_t dat, dummy, current, temperture;
volatile uint8_t v_now;
/*
 * SPI interrupt routine
 */
ISR(SPI_STC_vect) {

	switch(rstate) {
		case 0:
			dummy = SPDR;
			SPDR = (clicks >> 2);
			if(dummy == SPI_PACKET_HEADER)
				rstate = 1;
			else
				rstate = 0;
			break;
		case 1:
			motor_setpoint = (int8_t)SPDR;
			SPDR = current;
			rstate = 2;
			break;
		case 2:
			v_now = (uint8_t)SPDR;
			SPDR = temperture;
			rstate = 3;
			break;
		case 3:
			dummy = (uint8_t)SPDR;
			SPDR = SPI_PACKET_HEADER;			
			rstate = 4;
			break;
		case 4:
			dummy = (uint8_t)SPDR;
			SPDR = SPI_PACKET_HEADER;
			rstate = 0;
			break;
		default:
			dummy = (uint8_t)SPDR;
			SPDR = SPI_PACKET_HEADER;
			rstate = 0;
			break;

	}
}

ISR(INT0_vect) {
	uint8_t val;
	
	val = PIND;

	if(0x0C == (val & 0x0C) || 0x00 == (val & 0x0C))
		count++;
	else
		count--;
	
}


ISR(TIMER1_COMPA_vect) {
	
	if(0x26 == adc_mux) {
		adc_mux = 0x27;
		current = ADCH;
	}
	else {
		adc_mux = 0x26;
		temperture = ADCH;
	}
	ADMUX = adc_mux;
	ADCSRA = 0xC6;
}

// Quadrature caclulation interrupt
ISR(TIMER2_COMPA_vect) {
	
	event = 1;
	event_count = 0;
	clicks = (count - old_count);
	old_count = count;

}

void adc_init( void ) {
	
	cli();
	// set up ADC trigger counter
	OCR1AH = 0x00;
	OCR1AL = 0x80;
	TCCR1A = 0x00;
	TCCR1B = 0x0C;
	TIMSK1 = 0x02;

	adc_mux = 0x26;
	ADMUX = adc_mux;
	ADCSRA = 0xC6;
	while( ADCSRA & 0x40 );
	sei();
}

void quadrature_init( void ) {
	
	event = 0;
	count = 0;
	old_count = 0;
	clicks = 0;

	cli();
	TCCR2A = 0x02;
	TCCR2B = 0x07;
	OCR2A = 78;
	TIMSK2 = 0x02;

	EICRA = 0x01;
	EIMSK = 0x01;
	sei();
}

void spi_init_slave( void ) {
	
	cli();
	DDRB = 0x10;
	SPCR = _BV(SPE)|_BV(CPHA)|_BV(SPIE);
	SPDR = 0x13;
	//new_fifo(rx_fifo, rx_fifo_mem, RX_FIFO_SIZE);
	//new_fifo(tx_fifo, tx_fifo_mem, TX_FIFO_SIZE);
	rstate = 0;
	sei();
}

int main(void) {

	int8_t speed;
	// Clear everything for safety sake
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
	
	adc_init();
	quadrature_init();
	spi_init_slave();

	motor_init();

	event_count = 0;
	while(1){
		if(event) {
			event_count++;
			//slew_control();
			speed = pid_control(
				current_limit(motor_setpoint));
			
			//if(event_count > 2)
			//	set_speed(0);
			//else
				
			set_speed(speed);
			event = 0;
		}
	}

}
