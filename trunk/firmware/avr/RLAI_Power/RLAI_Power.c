#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t sys_voltage;

ISR(SPI_STC_vect) {

	SPDR = sys_voltage;
}

/*
 *  This reports system Voltage as:
 *  System Voltage = (val + 148) * 7 / 108
 *  or Approx:
 *  System Voltage = (val + 144) / 15
 *  This is close to linear, above 17V it starts to roll off
 *  because of the protection diode.
 */
uint8_t get_sys_volt( void ) {
	
	uint16_t raw;
	uint8_t val;
	raw = ADCL;
	raw += (((uint16_t)ADCH) << 8);
	ADMUX = 0x02;
	ADCSRA = 0xC3;
	val = (raw - 300) >> 1;
	return val;
}


void adc_init( void ) {
	
	ADMUX = 0x02;
	ADCSRA = 0xC3; // C3 for 1Mhz, C6 for 8Mhz
	while( ADCSRA & 0x40 );
}


void fan_init(void) {
	
	DDRD |= 0x60;
	OCR0A = 0x20;
	OCR0B = 0x01;
	TCCR0A = 0x21;
	TCCR0B = 0x09;

}


void spi_init_slave( void ) {
	
	cli();
	DDRB = 0x10;
	SPCR = _BV(SPE)|_BV(CPHA)|_BV(SPIE);
	SPDR = 0x00;
	sei();
}

int main(void) {

	volatile uint16_t i;
	unsigned char dat;
	char dir;
	// Clear everything for safety sake
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;

	// Pin Directions
	//DDRB = (1<<4);
	//DDRD = 0x0C;
	//PORTD = 0x00;
	DDRC |= 0x03;
	DDRD |= 0xAC;

	//PORTD &= ~(_BV(4)|_BV(5)|_BV(6));

	PORTC |= 0x01;
	PORTD |= 0x80;
	//PORTD |= 0x0C;
	//for(i = 0; i < 10000; i++);
	//PORTD &= ~0x0C;

	spi_init_slave();
	fan_init();

	while(1) {
		for(i = 0; i < 10000; i++);
		sys_voltage = get_sys_volt();
		dat = 10 - (sys_voltage / 11);
		OCR0B = dat;

	}

}
