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
#include <avr/eeprom.h>

uint8_t charge_state;
uint8_t saved_charge_state;

void set_charge_state(uint8_t new_state) {
  charge_state = new_state;
  eeprom_write_byte(CHARGE_STAT_EEPROM, new_state);
  return;
}

void read_charge_state(void) {
  charge_state = eeprom_read_byte(CHARGE_STAT_EEPROM);
}

void charge( void ) {

  uint8_t stat;
  // Stop things and reset if the charger gets unplugged.
  // Signal an error if we were in the middle of charging.
  if(!(system_state & CHARGE_OK)) {
    // Error if we were charging and the charger was unplugged.
    if(charge_state > 0 && charge_state < 10) {
      // We explicitly don't want to save this change to eeprom.
      // Charging was interrupted and we want to preserve the previous
      // state to resume from.
      charge_state = 200;
      charger40_disable();
      charger160_disable();
      charger280_disable();
    }
    // All is okay if we finished charging when it was unplugged.
    if(charge_state == 10) {
      charge_state = 0;
      // LED1_PORT &= ~LED1;
    }
    return;
  }
  else if (charge_state == 200) // and system_state & CHARGE_OK implicitly
    // If we are plugged back in after being disconnected, resume charge
    read_charge_state();

  // Due to the above test, we should never get this far unless we are in
  // system_state 3 or 7.

  // Some state transitions set charge_state explicitly, this bypasses
  // writing to the EEPROM to minimize wear since the next state will
  // Immediately transition to a new one.
  switch(charge_state) {
    case 0:
      // Don't charge if any of the batteries doesn't appear to be there.
      if(bat40v < MIN_BAT_VOLTAGE || bat160v < MIN_BAT_VOLTAGE ||
          bat280v < MIN_BAT_VOLTAGE)
        break;
      // If the batteries are uneven, start charging
      if(!(system_state & BAT_OK))
        set_charge_state(1);
      // Otherwise, charge only if the battery voltage has fallen a little
      else if(bat40v < MIN_BAT_CHARGE_VOLTAGE)
        set_charge_state(1);
      else {
        charger40_disable();
        charger160_disable();
        charger280_disable();
      }
      break;
    // Start charger 40
    case 1:
      charger40_enable();
      charge_state = 2;
      break;
    // Charger 40 on normal
    case 2:
      charger40_enable();
      stat = charger40_status();
      switch(stat) {
        case 0:
          // Stay in this mode
          break;
        case 1:
          // This is never a state the charger should be in.
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR);
          charge_state = 200;
          break;
        case 2:
          set_charge_state(3);
          break;
        case 3:
          // To get here means charger 40 went directly into shutdown before
          // topping off, which is very bad
          system_state |= CHARGER40_ERROR;
          charge_state = 200;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR &
              CHARGER160_ERROR & CHARGER280_ERROR);
          charge_state = 200;
          break;
      }
      break;
    // Charger 40 top off, start charger 160
    case 3:
      charger40_enable();
      charger160_enable();
      charge_state = 4;
      break;
    // Charger 40 top off, charger 160 normal or top off
    case 4:
      charger40_enable();
      charger160_enable();
      // Eventually we will get to this state if the batteries are unplugged
      // by this point all batteries should be very low, so reset to charge_state
      // 0 and we'll stay there happily.
      if(bat280v < MIN_BAT_VOLTAGE) {
        charger40_disable();
        charger160_disable();
        set_charge_state(0);
        break;
      }
      stat = charger40_status();
      switch(stat) {
        case 0:
          // Charger 40 was trickling, but went back to a full charge.
          // This is okay because of what happens on resume, so no errors.
          break;
        case 1:
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR);
          charge_state = 200;
          break;
        case 2:
          // Stay in this mode
          break;
        case 3:
          // Charger 40 has completed its charge
          set_charge_state(5);
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR &
              CHARGER160_ERROR & CHARGER280_ERROR);
          charge_state = 200;
          break;
      }
      stat = charger160_status();
      switch(stat) {
        case 0:
          // We should stay here
          break;
        case 1:
          system_state |= (CHARGER_COMM_ERROR & CHARGER160_ERROR);
          charge_state = 200;
          break;
        case 2:
          // Even if we're topping off stay here until Charger40 completes
          break;
        case 3:
          // This means charger160 disabled itself before charger40 completed its
          // top-off, this is possible if the batteries were at uneven charge levels,
          // and bat 160 happened to be fully charged when 40 wasn't, and the timeout
          // for 160 happened to be slightly shorter than for 40.  Any other situation
          // this should be considered an error.  Not sure how to discern that.
          // For now we shall treat it as an error.
          system_state |= CHARGER160_ERROR;
          charge_state = 200;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR &
              CHARGER160_ERROR & CHARGER280_ERROR);
          charge_state = 200;
          break;
      }
      break;
    // Charger 160 normal
    case 5:
      charger40_disable();
      charger160_enable();
      stat = charger160_status();
      switch(stat) {
        case 0:
          // Stay here
          break;
        case 1:
          system_state |= (CHARGER_COMM_ERROR | CHARGER160_ERROR);
          charge_state = 200;
          break;
        case 2:
          set_charge_state(6);
          break;
        case 3:
          // This would mean charger 160 either went from full charge to disabled,
          // or was disabled when we got to state 5.  For now treating this a as an error
          system_state |= CHARGER160_ERROR;
          charge_state = 200;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR & CHARGER160_ERROR
              & CHARGER280_ERROR);
          charge_state = 200;
          break;
      }
      break;
    // Charger 160 top off, start charger 280 or top off
    case 6:
      charger160_enable();
      charger280_enable();
      charge_state = 7;
      break;
    // Charger 160 top off, charger 280 normal
    case 7:
      charger160_enable();
      charger280_enable();
      stat = charger160_status();
      switch(stat) {
        case 0:
          // Correction:  To account for resuming charging, having charger160
          // in full charge mode here is acceptable.
          break;
        case 1:
          system_state |= (CHARGER_COMM_ERROR | CHARGER160_ERROR);
          charge_state = 200;
          break;
        case 2:
          // Stay here.
          break;
        case 3:
          set_charge_state(8);
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR &
              CHARGER160_ERROR & CHARGER280_ERROR);
          charge_state = 200;
          break;
      }
      stat = charger280_status();
      switch(stat) {
        case 0:
          // Stay here
          break;
        case 1:
          system_state |= (CHARGER_COMM_ERROR | CHARGER280_ERROR);
          charge_state = 200;
          break;
        case 2:
          // Even though we are topping off, stay here until 160 finishes
          break;
        case 3:
          // This is a confusing case (see above)
          system_state |= CHARGER280_ERROR;
          charge_state = 200;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR &
              CHARGER160_ERROR & CHARGER280_ERROR);
          charge_state = 200;
          break;
      }
      break;
    // Charger 280 normal
    case 8:
      charger160_disable();
      charger280_enable();
      stat = charger280_status();
      switch(stat) {
        case 0:
          // Stay here
          break;
        case 1:
          system_state |= (CHARGER_COMM_ERROR | CHARGER280_ERROR);
          charge_state = 200;
          break;
        case 2:
          set_charge_state(9);
          break;
        case 3:
          // Again, confusing, see above
          system_state |= CHARGER280_ERROR;
          charge_state = 200;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR &
              CHARGER160_ERROR & CHARGER280_ERROR);
          charge_state = 200;
          break;
      }
      break;
    // Charger 280 top off
    case 9:
      charger280_enable();
      stat = charger280_status();
      switch(stat) {
        case 0:
          // Ff we were interrupted and are resuming, charger280 may
          // still be in full charge mode for a little bit, so this is okay.
          break;
        case 1:
          system_state |= (CHARGER_COMM_ERROR | CHARGER280_ERROR);
          charge_state = 200;
          break;
        case 2:
          // Stay here
          break;
        case 3:
          // Not quite clean code, but to explain, this will set EEPROM charge state to 0
          // but keep the ram state to 10, if we restart we know we didn't interrupt
          // charging, but if we keep running then we know we have fully charged.
          set_charge_state(0);
          charge_state = 10;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR & CHARGER40_ERROR &
              CHARGER160_ERROR & CHARGER280_ERROR);
          charge_state = 200;
          break;
      }
      break;
    // Charge complete
    case 10:
      charger40_disable();
      charger160_disable();
      charger280_disable();
      // LED1_PORT |= LED1;
      break;
    // Error!
    default:
      charger40_disable();
      charger160_disable();
      charger280_disable();
      break;
  }
}
