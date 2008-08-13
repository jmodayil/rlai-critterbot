/*
 *  lib_adcspi.h
 *  
 *
 *  Created by Marc G. Bellemare on 1/08/08.
 *
 *  Driver for the off-chip ADC.
 *
 */

#ifndef LIB_ADCSPI_H
#define LIB_ADCSPI_H

/** Some flags that are sent to the ADC */
#define ADCSPI_STRAIGHT_CODING  (1 << 0)  
#define ADCSPI_HALF_RANGE       (1 << 1)
#define ADCSPI_WEAK_OUTPUT      (1 << 2)
#define ADCSPI_SHADOW_BIT       (1 << 3)
#define ADCSPI_ADDRESS          (0xF << 6)
#define ADCSPI_SEQ_BIT          (1 << 10)
#define ADCSPI_WRITE            (1 << 11)
#define ADCSPI_READ             (0 << 11)

#define ADCSPI_PM_NORMAL        (0x3 << 4)
#define ADCSPI_PM_FULL_SHUTDOWN (0x2 << 4)
#define ADCSPI_PM_AUTO_SHUTDOWN (0x1 << 4)
#define ADCSPI_PM_AUTO_STANDBY  (0x0 << 4)

#define ADCSPI_NO_SEQUENCE      (0x0)
#define ADCSPI_SELECT_SHADOW    (ADCSPI_SHADOW_BIT)
#define ADCSPI_CONTINUE_SEQ     (ADCSPI_SEQ_BIT)
#define ADCSPI_CONSECUTIVE      (ADCSPI_SHADOW_BIT | ADCSPI_SEQ_BIT)

#define ADCSPI_CONTROL_FLAGS    (ADCSPI_STRAIGHT_CODING |\
                                 ADCSPI_PM_NORMAL |\
                                 ADCSPI_HALF_RANGE)

#define ADCSPI_NUM_OUTPUTS      16
#define ADCSPI_DEVICE_ID        7

/** Initialization routine for the off-chip ADC */
int adcspi_init();

/** Event routine */
int adcspi_event();

/** Selects a set of inputs that should be converted. The inputs are numbered
  *  from 0 to 15.
  */
void adcspi_select(int input);

/** Deselects a previously selected input for conversion. */
void adcspi_deselect(int input);

/** Returns whether a particular input channel is selected for conversion */
int adcspi_is_selected(int input);

/** Sends packets to select a new set of inputs (via the shadow register) 
  * to the ADC.
  */
void adcspi_send_select();

/** Sends packets to read in data from the ADC (only for selected inputs) */
void adcspi_read_data();

/** Returns a 12-bit output from the ADC */
int adcspi_get_output(int index);

/** Test whether the provided addresses correspond to our output indices.
  *
  * Used for error checking.
  */
void adcspi_test_addresses();

#endif /* LIB_ADCSPI_H */

