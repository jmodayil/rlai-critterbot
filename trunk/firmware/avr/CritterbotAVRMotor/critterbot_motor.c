#include <avr/io.h>
#include <avr/interrupt.h>
#include "include/avr_motor.h"
#include "include/avr_spi.h"

#define TEMP_THRESH 0
#define COMMAND_TIMEOUT 60

volatile uint16_t enc_count, enc_old_count;
volatile uint8_t rstate, event, event_count;
uint8_t adc_mux;
volatile uint8_t dat, dummy, current, temperature;
volatile uint8_t v_now;
volatile uint8_t motor_mode;

/*
 * SPI interrupt routine
 */
ISR(SPI_STC_vect) {

  switch(rstate) {
    case 0:
      dummy = SPDR;
      SPDR = (clicks >> 2);
      if(dummy == SPI_PACKET_HEADER) {
        motor_mode = 0;
        rstate = 1;
      }
      else if(dummy == SPI_PWM_HEADER) {
        motor_mode = 1;
        rstate = 1;
      }
      else
        rstate = 0;
      break;
    case 1:
      motor_setpoint = (int8_t)SPDR;
      event_count = 0;
      SPDR = current;
      rstate = 2;
      break;
    case 2:
      v_now = (uint8_t)SPDR;
      SPDR = temperature;
      rstate = 3;
      break;
    case 3:
      dummy = (uint8_t)SPDR;
      SPDR = SPI_PADDING;
      rstate = 4;
      break;
    case 4:
      dummy = (uint8_t)SPDR;
      SPDR = 0;
      rstate = 0;
      break;
    default:
      dummy = (uint8_t)SPDR;
      SPDR = 0;
      rstate = 0;
      break;

  }
}

ISR(INT0_vect) {
  uint8_t val;

  val = PIND;

  if(0x0C == (val & 0x0C) || 0x00 == (val & 0x0C))
    enc_count++;
  else
    enc_count--;

}


ISR(TIMER1_COMPA_vect) {

  if(0x26 == adc_mux) {
    adc_mux = 0x27;
    current = ADCH;
  }
  else {
    adc_mux = 0x26;
    temperature = ADCH;
  }
  ADMUX = adc_mux;
  ADCSRA = 0xC6;
}

// Quadrature caclulation interrupt
ISR(TIMER2_COMPA_vect) {

  event = 1;
  // Yes, this is still correct even when one value has wrapped around.
  clicks = (enc_count - enc_old_count);
  enc_old_count = enc_count;

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

  // Initialize with dummy conversion
  ADCSRA = 0xC6;
  while( ADCSRA & 0x40 );
  sei();
}

void quadrature_init( void ) {

  event = 0;
  enc_count = 0;
  enc_old_count = 0;
  clicks = 0;

  cli();
  // CTC Mode, count to OCR2A then reset
  TCCR2A = 0x02;
  // Clock divider = 1024 so step at 7812Hz.
  TCCR2B = 0x07;
  // Count to 78 to sample at approx 100 Hz.
  OCR2A = 78;
  // Enable interrupts on OCR2A match
  TIMSK2 = 0x02;

  EICRA = 0x01;
  EIMSK = 0x01;
  sei();
}

int main(void) {

  int8_t speed;
  // Clear everything for safety
  PORTB = 0x00;
  PORTC = 0x00;
  PORTD = 0x00;

  adc_init();
  quadrature_init();
  spi_init_slave();
  rstate = 0;
  motor_mode = 0;

  motor_init();

  event_count = 100;
  while(1){
    if(event) {

      if(event_count++ > COMMAND_TIMEOUT) {
        motor_setpoint = 0;
        event_count = 0;
      }
      speed = current_limit(motor_setpoint);
      if(motor_mode == 0) {
        speed = soft_pid_control(speed);
      }
      set_speed(speed);

      event = 0;
    }
  }

}
