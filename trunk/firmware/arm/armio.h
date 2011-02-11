/*
 *  armio.h
 *  
 *  Created by Michael Sokolsky on 6/28/08.
 *
 */

#ifndef ARMIO_H
#define ARMIO_H

#include "compiler.h"
#include <string.h>
#include <stdarg.h>
#include "AT91SAM7S256.h"
#include "lib_AT91SAM7S256.h"
#include "armconfig.h"
#include "lib_events.h"

#define MAX_INT_PRINT_SIZE 32
#define SIGNED 1
#define UNSIGNED 0

#ifndef EOF
#define EOF -1
#endif

//#define SER_BAUD_RATE 230400 
#define SER_BAUD_RATE 115200
#define MCK 47923200
#define SER_BRGR (MCK / 16 / SER_BAUD_RATE)

#define SERIAL_A_PINS (AT91C_PA5_RXD0 | AT91C_PA6_TXD0)
#define SERIAL_B_PINS 0

#define SER_TX_BUF_SIZE 1024 
#define SER_RX_BUF_SIZE 1024

char ser_tx_buf[SER_TX_BUF_SIZE];
char ser_rx_buf[SER_RX_BUF_SIZE];

/* 28/09/08 MVS
 * Removed volatile flag from ser_rx_head as I don't think it is needed.
 */
volatile char *ser_rx_head;
volatile char * volatile ser_tx_head;
char *read_loc;

/*
 * outputs a character to the serial port
 */ 
void armputchar(char);

/*
 * outputs a character to the serial port (blocking)
 */ 
ARM_CODE RAMFUNC void __armputchar(char);

/*
 * gets a character from the serial port
 * 
 * returns the character or EOF if buffer is empty
 */
int armgetchar();

/*
 * gets the number of unread characters in the serial buffer
 */
int armgetnumchars(void); 

/*
 * prints a formatted string to the serial port
 * format string % supports:
 * d = signed decimal integer
 * u = unsigned decimal integer
 * X,x,p = unsigned hexadecimal integer
 * o = unsigned octal integer
 * b = unsigned binary integer
 * c = single character
 * s = \0 terminated string
 */
void armprintf(char*, ...);

/*
 * same as above but blocking
 */
void __armprintf(char*, ...);

/*
 * scans data from a string according to a formatted string
 * char* is the string to read from
 * char* is the format string, supporting options in armprintf
 * remaining arguments must be pointers and must equal the number 
 * of tokens in the format string
 * 
 * returns number of tokens successfully matched
 */
int armsscanf(char*, char*, ...);

void armitoa(int, char*, int, int );
int armatoi(char*, int*);
void strrev(char*);
int getvalue(char);

/*
 * Interrupt routine for serial port.
 * Must be run in RAM.
 */ 
ARM_CODE RAMFUNC void ser_isr(void);

/*
 * Initialize the serail port for character stream input/output
 */
int init_serial_port_stdio(void);

/*
 * Initialize the PIO ports to control amplifier and vref power
 */
void init_vref_control(void);
void init_amplifier_control(void);

void vref_enable(void);
void vref_disable(void);
void amplifier_enable(void);
void amplifier_disable(void);

/*
 * Reads a newline terminated string from the serial port.
 *
 * Takes a pointer to the string buffer and the size of the buffer
 * 
 * Non-blocking, returns EOF if there is no full string in the port.
 * Otherwise returns number of characters read
 *
 */
int armreadline(char*, int);

#endif

