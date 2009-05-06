// *----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
// *----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
// *----------------------------------------------------------------------------
//* File Name           : main.c
//* Object              : main application written in C
//* Creation            : JPP   16/Jun/2004
// *----------------------------------------------------------------------------

// Include Standard files
#include "Board.h"

// give the timer counter an interrupt level so that . . . it
// can issue interrupts?
#define TC_INTERRUPT_LEVEL 6

// set which LED is going to be doing the fancy stuff
#define TC0_LED LED1

// these numbers come from. . . ?

// The number of 'ticks' after which a second has elapsed
#define PULSE_MAX 39062
#define CLOCK_MAX 65536
#define PULSE_WIDTH_MIN 0
#define PULSE_WIDTH_MAX 255

#define NUM_QUANTA 256

// The 'base' clock value which will give us a second with TIMER_CLOCK5
#define CLOCK_BASE (CLOCK_MAX - PULSE_MAX)

// The pulse width increment is defined to be 1/256th of a second in
//  ticks. One second is PULSE_MAX ticks, do the math.
#define PULSE_WIDTH_INC 152

#define NO_SELECTION -1
#define MAX_LED      4

int pulseWidth[4];
int counter;
int currentLed;
int tempWidth;

void init( void )
{
   int i;
   for(i=0; i<4; i++)
     pulseWidth[i] = NUM_QUANTA/2;
   counter=0;
   currentLed=0;
   tempWidth=NO_SELECTION;
}

/** Interrupt handler for TC0. Handles the pulse width modulation. */
void tc0_irq_handler ( void )
{
   int i;
   int led;
  
   // Assume that setting output turns LED on, not pulse
   for(i=0; i<4; i++)
   {
    led = (1<<i);

     if(counter == 0)
      // The leds are defined as 0-3, set each one
      AT91F_PIO_SetOutput( AT91C_BASE_PIOA, led);
     else if (counter == pulseWidth[i])
      AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, led);
   }
   counter++;
   if(counter >= NUM_QUANTA)
     counter=0;
}

void tc1_irq_handler ( void )
{
  int width = pulseWidth[currentLed];

  // If pressing switch 3, change selected leds
  if (AT91F_PIO_GetInput (AT91C_BASE_PIOA) & SW3_MASK)
  {
    // Switch 3 was up
    if (tempWidth == NO_SELECTION)
    {
      currentLed++;
      if (currentLed >= MAX_LED) currentLed = 0;

      tempWidth = pulseWidth[currentLed];
      pulseWidth[currentLed] = PULSE_WIDTH_MAX;
    }
  }
  // Else if the button was pressed before...
  else if (tempWidth != NO_SELECTION)
  {
    {
      // Set the current led's pulse width to its actual value
      pulseWidth[currentLed] = tempWidth;
      tempWidth = NO_SELECTION;
    }
  }

  // If switch 1 is on, increase the pulse width
  else
  {
    if (AT91F_PIO_GetInput (AT91C_BASE_PIOA) & SW1_MASK)
    {
     width++;
     // Cap the pulse width
     if (width > PULSE_WIDTH_MAX)
       width = PULSE_WIDTH_MAX;
     }
    if (AT91F_PIO_GetInput (AT91C_BASE_PIOA) & SW2_MASK)
    {
     width--;
     if (width < PULSE_WIDTH_MIN)
      width = PULSE_WIDTH_MIN;
    }
    pulseWidth[currentLed] = width;
  }
}

int main()
{
  //* Begin
    AT91PS_AIC pAic = AT91C_BASE_AIC;

       AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 <<  AT91C_ID_PIOA ) ;
       AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 <<  AT91C_ID_TC0 ) ;
       AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 <<  AT91C_ID_TC1 ) ;

       AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, LED_MASK ) ;
       //AT91C_BASE_PIOA->PIO_OWER = (LED1 | LED2);

       AT91F_PIO_CfgInput( AT91C_BASE_PIOA, SW_MASK );
        // Clean up and remove?
       //AT91F_PIO_SetOutput( AT91C_BASE_PIOA, LED_MASK ) ;

       // Use the 40Mhz / 1024 clock
       // Also, use wavesel=10 (we reset the clock on RC and generate an
       //  interrupt)
       AT91C_BASE_TC0->TC_CMR = (AT91C_TC_CLKS_TIMER_DIV5_CLOCK |
                                 AT91C_TC_WAVE | AT91C_TC_WAVESEL_UP_AUTO);
      // Set RC to be 1/256th of a second
      AT91C_BASE_TC0->TC_RC = PULSE_WIDTH_INC;

      // Enable the clock and software trigger (?)
       AT91C_BASE_TC0->TC_CCR = ( AT91C_TC_CLKEN | AT91C_TC_SWTRG );

       // Enable the TC0 interrupt
       AT91F_AIC_ConfigureIt (pAic, AT91C_ID_TC0, TC_INTERRUPT_LEVEL,
         AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, tc0_irq_handler);
       // Enable the interrupt on RC compare
       AT91F_TC_InterruptEnable(AT91C_BASE_TC0,AT91C_TC_CPCS);
       // set the interrupt by software (what does this mean?)
       AT91F_AIC_EnableIt (pAic, AT91C_ID_TC0);

        // Enable the timer/counter to isten to the switches
        // Same setup as for TC0, except that now we use the MCK/8 clock
        //  This results in roughly 76 interrupts per second
        AT91C_BASE_TC1->TC_CMR = (AT91C_TC_CLKS_TIMER_DIV2_CLOCK |
                                  AT91C_TC_WAVE | AT91C_TC_WAVESEL_UP);

        AT91C_BASE_TC1->TC_CCR = (AT91C_TC_CLKEN | AT91C_TC_SWTRG);
        AT91F_AIC_ConfigureIt (pAic, AT91C_ID_TC1, TC_INTERRUPT_LEVEL,
          AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, tc1_irq_handler);
        AT91F_TC_InterruptEnable(AT91C_BASE_TC1, AT91C_TC_COVFS);
        AT91F_AIC_EnableIt (pAic, AT91C_ID_TC1);

    // Loop forever
    while(1)
    {

    }// End for
}//* End

