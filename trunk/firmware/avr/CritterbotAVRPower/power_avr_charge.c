/*
 * power_avr_charge.c
 *
 *  Created on: Jun 22, 2009
 *      Author: sokolsky
 */

#include "include/pin_names.h"
#include "include/critterbot_power.h"
#include "include/power_avr_charge.h"
#include "include/power_avr_utils.h"

uint8_t charge_state;


// If batteries are highly charged allready, we should not charge.
// Also, if
void charge( void ) {

  static uint8_t count;
  static uint8_t delay;
  uint8_t stat;

  // Only charge if the system bus has sufficient voltage
  if(system_voltage > 175) {
    if(delay < CHARGE_VOLTAGE_DELAY)
      delay++;
  }
  else {
    if(delay > 0)
      delay--;
  }
  if(delay < CHARGE_VOLTAGE_DELAY)
    return;
  if(delay == 0 && charge_state < 10)
    charge_state = 200;
  if(delay == 0 && charge_state == 10)
    charge_state = 0;

  switch(charge_state) {
    case 0:
      if(delay == CHARGE_VOLTAGE_DELAY) {
        charge_state = 1;
      }
      else
        charge_state = 0;
      break;
    // Start charger 40
    case 1:
      charger40_enable();
      charge_state = 2;
      break;
    // Charger 40 on normal
    case 2:
      stat = charger40_status();
      switch(stat) {
        case 0:
          charge_state = 2;
          break;
        case 2:
          charge_state = 3;
          break;
        default:
          charge_state = 201;
          break;
      }
      break;
    // Charger 40 top off, start charger 160
    case 3:
        charger160_enable();
        charge_state = 4;
      break;
    // Charger 40 top off, charger 160 normal or top off
    case 4:
      stat = charger40_status();
      switch(stat) {
        case 2:
          charge_state = 4;
          break;
        case 3:
          charge_state = 5;
          break;
        default:
          charge_state = 202;
          break;
      }
      stat = charger160_status();
      switch(stat) {
        case 0:
          break;
        case 2:
          break;
        default:
          charge_state = 203;
          break;
      }
      break;
    // Charger 160 normal
    case 5:
      //charger40_disable();
      stat = charger160_status();
      switch(stat) {
        case 0:
          charge_state = 5;
          break;
        case 2:
          count = 0;
          charge_state = 6;
          break;
        default:
          charge_state = 204;
          break;
      }
      break;
    // Charger 160 top off, start charger 280 or top off
    case 6:
      charger280_enable();
      charge_state = 7;
      break;
    // Charger 160 top off, charger 280 normal
    case 7:
      stat = charger160_status();
      switch(stat) {
        case 2:
          charge_state = 7;
          break;
        case 3:
          charge_state = 8;
          break;
        default:
          charge_state = 205;
          break;
      }
      stat = charger280_status();
      switch(stat) {
        case 0:
          break;
        case 2:
          break;
        default:
          charge_state = 206;
          break;
      }
      break;
    // Charger 280 normal
    case 8:
      //charger160_disable();
      stat = charger280_status();
      switch(stat) {
        case 0:
          charge_state = 8;
          break;
        case 2:
          charge_state = 9;
          break;
        default:
          charge_state = 207;
          break;
      }
      break;
    // Charger 280 top off
    case 9:
      stat = charger280_status();
      switch(stat) {
        case 2:
          charge_state = 9;
          break;
        case 3:
          charge_state = 10;
          break;
        default:
          charge_state = 208;
          break;
      }
      break;
    // Charge complete
    case 10:
      charger40_disable();
      charger160_disable();
      charger280_disable();
      LED1_PORT |= LED1;
      delay = 0;
      break;
    // Error!
    default:
      charger40_disable();
      charger160_disable();
      charger280_disable();
      delay = 0;
      break;
  }
}
