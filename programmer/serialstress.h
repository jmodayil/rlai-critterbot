#ifndef SERIAL_STRESS_H
#define SERIAL_STRESS_H

#define STRESS_TYPE_TX    0
#define STRESS_TYPE_RXTX  1

// Maximum length of data to be sent in stress test 0 (transmit only)
#define STRESS_MAX_LEN_TX 128
#define STRESS_TX_MUL     9887
#define STRESS_TX_MOD     257

// These are defined somewhere else; better code would #include the proper
//  driver header files
#define ARM_BUFFER_SIZE   1024
#define UI_CMD_BUFFER_SIZE 256

// Default sleeping time, in us. Should be a divisor of 1 million.
#define USLEEP_STEP       50
#define STEPS_IN_SECOND   (1000000/USLEEP_STEP)

/** Flushes all received (but yet unread) data */
void flush_received();

/** Function that tests only the transmitting part of the ARM OS (provided
  *  basically receiving capabilities are in place to receive the initial
  *  request for the test). */
int stress_tx(int port);
/** Function that tests first receiving, then transmitting, in the ARM OS.
  *  This is done by requesting that the UI send back the same string that we
  *  send it. */ 
int stress_rxtx(int port);

/** Attempts to read a single character and waits up to one second before 
  * giving up. It then returns -1. Otherwise, returns 0 on EOF and nonzero
  * on success. */
int read_b(int port, char * data);

// For testing purposes, write and read are redefined
#define write(a,b,c) (test_write(a,b,c))
void test_write(int port, void * data, int ct);
#define read(a,b,c) (test_read(a,b,c))
int test_read(int port, void * data, int ct);

void test_command();

void test_send_tx();

#endif

