/*
 *  armio.h
 *  
 *  Created by Michael Sokolsky on 6/28/08.
 *
 */

#define MAX_INT_PRINT_SIZE 32
#define SIGNED 1
#define UNSIGNED 0
#define SER_BAUD_CD (MCK / (16 * SER_BAUD_RATE))
#define SER_TX_BUF_SIZE 1024
#define SER_RX_BUF_SIZE 1024

char ser_tx_buf[SER_TX_BUF_SIZE];
char ser_rx_buf[SER_RX_BUF_SIZE];
char *ser_rx_head, *ser_rx_tail;
char *ser_tx_head, *ser_tx_tail;

/*
 * outputs a character to the serial port
 */ 
void armputchar(char);

/*
 * gets a character from the serial port
 * 
 * returns the character or EOF if buffer is empty
 */
int armgetchar();

/*
 * gets a new \n terminated line from the serial port
 * char* is the buffer to copy data to
 * int is the size of the buffer
 * 
 * returns the number of characters read, or EOF if there
 * is no full line in the receive buffer
 */
int armgetline(char*, int);

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
 * scans data from a string according to a formatted string
 * char* is the string to read from
 * char* is the format string, supporting options in armprintf
 * remaining arguments must be pointers and must equal the number 
 * of tokens in the format string
 * 
 * returns number of tokens successfully matched
 */
int armsscanf(char*, char*, ...);

void itoa(int, char*, int, int );
void strrev(char*);

/*
 * Interrupt routine for serial port.
 * Must be run in RAM.
 */ 
RAM_FUNCTION void ser_isr(void);

/*
 * Initialize the serail port for character stream input/output
 */
void init_serial_port_stdio(void);
