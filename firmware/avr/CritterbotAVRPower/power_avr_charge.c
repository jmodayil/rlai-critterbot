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

enum swish {FUEL_STATE0,FUEL_STATE1,FUEL_STATE2,FUEL_STATE3,FUEL_STATE4,FUEL_STATE5,FUEL_STATE6,FUEL_STATE7,FUEL_STATE8,FUEL_STATE9,FUEL_STATE10,FUEL_STATE11};  // These are all normal states for the charger state machine, errors are 200 and above.

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
  uint8_t charging_disabled;

  // Find the lowest of the battery voltages 
  uint8_t minbatv = (bat40v < bat160v? bat40v : bat160v);
  minbatv = (minbatv < bat280v? minbatv : bat280v);

  charging_disabled  = (commands & POWER_CHARGING_DISABLED &&
     minbatv > OVERRIDE_CHARGING_DISABLED_VOLTAGE);
 
  if (!charging_disabled)
    LED1_PORT |= LED1;
  else
    LED1_PORT &= ~LED1;

  // Stop things and reset if the charger gets unplugged.
  // Signal an error if we were in the middle of charging.
  if(!(system_state & CHARGE_OK)) {
    // Error if we were charging and the charger was unplugged.
    if(charge_state > 0 && charge_state < 11) {
      // We explicitly don't want to save this change to eeprom.
      // Charging was interrupted and we want to preserve the previous
      // state to resume from.
      charge_state = 201;
    }
    // All is okay if we finished charging when it was unplugged.
    if(charge_state == 11) {
      charge_state = FUEL_STATE0;
      // LED1_PORT &= ~LED1;
    }
    return;
  }
  else if (charge_state == 201) // and system_state & CHARGE_OK implicitly
    // If we are plugged back in after being disconnected, resume charge
    read_charge_state();

  // Due to the above test, we should never get this far unless we are in
  // system_state 3 or 7.

  // Some state transitions set charge_state explicitly, this bypasses
  // writing to the EEPROM to minimize wear since the next state will
  // Immediately transition to a new one.
  switch(charge_state) {
    case FUEL_STATE0:
      charger40_disable();
      charger160_disable();
      charger280_disable();
      // Don't charge if any of the batteries doesn't appear to be there.
      // *************** IMPORTANT *************
      // Look at this code if the batteries become fully drained and shut down
      // and the robot does not seem to be charging them.  More detail:
      //
      // This may actually be bad.  If the batteries are fully discharged they
      // shut off, reading 0 volts.  Apparently we are able to charge despite
      // this, my best guess is when the AVR first powers up there is a brief
      // moment when all the chargers are enabled.  This causes a short burst
      // of voltage to be applied to the batteries, and either capacitance
      // means the battery voltages read here still appear high enough, or
      // the batteries turn on again because of the applied power.  Either way
      // this is not the ideal situation, but probably requires electrical
      // modifications to fix.  This also means we can't detect when batteries
      // are actually not connected, and will try to change anyway.
      if(bat40v < MIN_BAT_VOLTAGE || bat160v < MIN_BAT_VOLTAGE ||
          bat280v < MIN_BAT_VOLTAGE)
        break;
      
      // If charging is not disabled, determine whether we should start
      //  charging now
      // If the batteries are uneven, start charging
      if(!(system_state & BAT_OK))
        set_charge_state(FUEL_STATE1);
      else if (!charging_disabled) {
        // Otherwise, charge only if the battery voltage has fallen a little
        if(bat40v < MIN_BAT_CHARGE_VOLTAGE)
          set_charge_state(FUEL_STATE1);
      }
      break;
    // Start charger 40
    case FUEL_STATE1:
      charger40_enable();
      charge_state = FUEL_STATE2;
      break;
    // Charger 40 on normal
    case FUEL_STATE2:
      charger40_enable();
      stat = charger40_status();
      switch(stat) {
        case CHARGER_BASE_CHARGE:
          // Stay in this mode
          break;
        case CHARGER_INVALID:
          // This is never a state the charger should be in.
          system_state |= (CHARGER_COMM_ERROR | CHARGER40_ERROR);
          charge_state = 211;
          break;
        case CHARGER_TOPOFF_CHARGE:
          set_charge_state(FUEL_STATE3);
          break;
        case CHARGER_SHUTDOWN:
          // To get here means charger 40 went directly into shutdown before
          // topping off, which is very bad
          system_state |= CHARGER40_ERROR;
          charge_state = 212;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR | CHARGER40_ERROR |
              CHARGER160_ERROR | CHARGER280_ERROR);
          charge_state = 213;
          break;
      }
      break;
    // Charger 40 top off, start charger 160
    case FUEL_STATE3:
      charger40_enable();
      //      charger160_enable();
      charge_state = FUEL_STATE4;
      break;
    // Charger 40 top off
    case FUEL_STATE4:
      charger40_enable();
      //   charger160_enable();
      // Eventually we will get to this state if the batteries are unplugged
      // by this point all batteries should be very low, so reset to charge_state
      // 0 and we'll stay there happily.
      if(bat280v < MIN_BAT_VOLTAGE) {
        charger40_disable();
        charger160_disable();
        set_charge_state(FUEL_STATE0);
        break;
      }
      stat = charger40_status();
      switch(stat) {
      case CHARGER_BASE_CHARGE:
          // Charger 40 was trickling, but went back to a full charge.
          // This is okay because of what happens on resume, so no errors.
          break;
        case CHARGER_INVALID:
          system_state |= (CHARGER_COMM_ERROR | CHARGER40_ERROR);
          charge_state = 214;
          break;
        case CHARGER_TOPOFF_CHARGE:
          // Stay in this mode
          break;
        case CHARGER_SHUTDOWN:
          // Charger 40 has completed its charge
          set_charge_state(FUEL_STATE5);
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR | CHARGER40_ERROR |
              CHARGER160_ERROR | CHARGER280_ERROR);
          charge_state = 215;
          break;
      }
      break;
    // Charger 160 normal
    case FUEL_STATE5:
      charger40_disable();
      charger160_enable();
      //      charger280_enable();
      charge_state = FUEL_STATE6;
      break;
    case FUEL_STATE6:
      charger40_disable();
      charger160_enable();
      stat = charger160_status();
      switch(stat) {
      case CHARGER_BASE_CHARGE:
          // Stay here
          break;
        case CHARGER_INVALID:
          system_state |= (CHARGER_COMM_ERROR | CHARGER160_ERROR);
          charge_state = 221;
          break;
        case CHARGER_TOPOFF_CHARGE:
          set_charge_state(FUEL_STATE7);
          break;
        case CHARGER_SHUTDOWN:
          // This would mean charger 160 either went from full charge to disabled,
          // or was disabled when we got to state 5.  For now treating this a as an error
          set_charge_state(FUEL_STATE8);
          //system_state |= CHARGER160_ERROR;
          //charge_state = 222;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR | CHARGER40_ERROR | 
              CHARGER160_ERROR | CHARGER280_ERROR);
          charge_state = 223;
          break;
      }
      break;
    // Charger 160 top off
    // Charger 160 top off
    case FUEL_STATE7:
      charger160_enable();
      //charger280_enable();
      stat = charger160_status();
      switch(stat) {
      case CHARGER_BASE_CHARGE:
          // Correction:  To account for resuming charging, having charger160
          // in full charge mode here is acceptable.
          break;
        case CHARGER_INVALID:
          system_state |= (CHARGER_COMM_ERROR | CHARGER160_ERROR);
          charge_state = 224;
          break;
        case CHARGER_TOPOFF_CHARGE:
          // Stay here.
          break;
        case CHARGER_SHUTDOWN:
          set_charge_state(FUEL_STATE8);
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR | CHARGER40_ERROR |
              CHARGER160_ERROR | CHARGER280_ERROR);
          charge_state = 225;
          break;
      }
      break;
    // Charger 280 normal
    case FUEL_STATE8:
      charger160_disable();
      charger280_enable(); // maybe a step is required
      charge_state = FUEL_STATE9;
      break;
    case FUEL_STATE9:
      charger160_disable();
      charger280_enable();
      stat = charger280_status();
      switch(stat) {
      case CHARGER_BASE_CHARGE:
          // Stay here
          break;
        case CHARGER_INVALID:
          system_state |= (CHARGER_COMM_ERROR | CHARGER280_ERROR);
          charge_state = 231;
          break;
        case CHARGER_TOPOFF_CHARGE:
          set_charge_state(FUEL_STATE10);
          break;
        case CHARGER_SHUTDOWN:
          // Again, confusing, see above
          system_state |= CHARGER280_ERROR;
          charge_state = 232;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR | CHARGER40_ERROR |
              CHARGER160_ERROR | CHARGER280_ERROR);
          charge_state = 233;
          break;
      }
      break;
    // Charger 280 top off
    case FUEL_STATE10:
      charger280_enable();
      stat = charger280_status();
      switch(stat) {
      case CHARGER_BASE_CHARGE:
          // If we were interrupted and are resuming, charger280 may
          // still be in full charge mode for a little bit, so this is okay.
          break;
        case CHARGER_INVALID:
          system_state |= (CHARGER_COMM_ERROR | CHARGER280_ERROR);
          charge_state = 234;
          break;
        case CHARGER_TOPOFF_CHARGE:
          // Stay here
          break;
        case CHARGER_SHUTDOWN:
          // Not quite clean code, but to explain, this will set EEPROM charge state to 0
          // but keep the ram state to 10, if we restart we know we didn't interrupt
          // charging, but if we keep running then we know we have fully charged.
          set_charge_state(FUEL_STATE0);
          charge_state = FUEL_STATE11;
          break;
        default:
          // BAD BAD BAD
          system_state |= (CHARGER_COMM_ERROR | CHARGER40_ERROR |
              CHARGER160_ERROR | CHARGER280_ERROR);
          charge_state = 235;
          break;
      }
      break;
    // Charge complete
    case FUEL_STATE11:
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
