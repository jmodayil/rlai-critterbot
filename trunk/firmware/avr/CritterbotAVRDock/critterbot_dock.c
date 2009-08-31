/*
 * Code for the AVR controller in the critterbot docking station.
 *
 * Created 13 August, 2009
 * Created by Mike Sokolsky
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>

// Voltage threshold to turn on voltage
#define VDETECT 40
// Voltage threshold that voltage is on
#define VTHRESH 200
// Minimum current to keep voltage on
#define ITHRESH 4

#define voltage_on() (PORTB = 0x01)
#define voltage_off() (PORTB = 0x00)
#define led_on() (PORTD |= 0x80)
#define led_off() (PORTD &= ~0x80)
#define led_blink() (PIND = 0x80)

ISR(TIMER0_COMPB_vect) {
  PIND = 0x1F;
}

ISR(TIMER0_OVF_vect) {
  PIND = 0x1F;
}

int main(void) {

  uint8_t blink_counter = 0;
  uint8_t state = 0;
  uint8_t voltage;
  uint8_t current;

  // Pins D0-D4 are outputs for LED's
  // Pin D7 is status LED
  DDRD = 0x9F;
  PORTD = 0x00;

  // Pin B0 controls charge power
  DDRB = 0x01;
  PORTB = 0x00;

  // Fast PWM
  TCCR0A = 0x03;
  // Count up to A, clkdiv 256 = ~3906 hertz
  TCCR0B = 0x0C;
  // Interrupt on OCB match and overflow
  TIMSK0 = 0x05;

  sei();

  // Run at 50 Hz
  OCR0A = 78;
  // 50% duty cycle
  OCR0B = 39;

  while(1) {
    // Left aligned channel 7
    ADMUX = 0x27;
    // Start conversion, MCK/8 (125kHz)
    ADCSRA = 0xC3;
    // Wait for conversion
    while((ADCSRA & 0x40));
    voltage = ADCH;
    // Left aligned channel 6
    ADMUX = 0x26;
    // Start conversion, MCK/8 (125kHz)
    ADCSRA = 0xC3;
    // Wait for conversion
    while((ADCSRA & 0x40));
    current = ADCH;

    // Turn on/off voltage to charger as required
    blink_counter++;
    switch(state) {
      case 0:
        if(voltage > VDETECT) {
          state = 1;
          voltage_on();
          led_on();
        }
        else {
          if(blink_counter >= 254)
            led_blink();
        }
        break;
      case 1:
        if(current < ITHRESH) {
          state = 0;
          voltage_off();
          led_off();
          blink_counter = 0;
         }
        break;
      default:
        break;
    }
    // Roughly 500uS delay at 1Mhz clock
    _delay_loop_2(1000);
  }

  return 0;
}
