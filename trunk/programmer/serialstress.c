#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "crctable.h"
#include <stdlib.h>
#include <string.h>

#include "serialstress.h"

#define BAUDRATE B115200
#define DEVICE "/dev/ttyS0"

int ser_port;
int flag_echo_recv = 0;
int flag_test_noise = 0;

struct termios oldterm;

void initport(int port) {
  
  struct termios options;
  tcflush(port, TCIOFLUSH);
  tcgetattr(port, &oldterm);
  tcgetattr(port, &options);
  options.c_cflag &= ~(CSIZE | CSTOPB | PARENB | CRTSCTS);
  options.c_cflag |= (CS8 | CLOCAL | CREAD);
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_iflag &= (INPCK | IXON | IXOFF);
  options.c_iflag |= IGNPAR | OCRNL;;
  options.c_oflag &= ~OPOST; 
  options.c_cc[VTIME] = 0;
  options.c_cc[VMIN] = 1;
  cfsetispeed(&options, BAUDRATE);
  cfsetospeed(&options, BAUDRATE);
  tcflush(port, TCIOFLUSH);
  tcsetattr(port, TCSANOW, &options);
}

void closeport(int port) {

  tcsetattr(port, TCSANOW, &oldterm);
  tcflush(port, TCIOFLUSH);

}

char command_str[1024];

int send_string(int port, char * str)
{
  int ct = 0;
  char * ptr = str;

  // Walk the whole string, chunking out each character in turn with a 50us
  //  pause
  //fprintf (stderr, "SS %s\n", ptr);
  while (*ptr != 0)
  {
    write(port, ptr, 1);
    usleep(50);
    ptr++;
    ct++;
  }

  return ct;
}

void send_header(int port, int type) 
{
  sprintf (command_str, "test stress %d ", type);
  send_string(port, command_str);
}

void send_int(int port, int data) 
{
  sprintf (command_str, "%d", data);
  send_string(port, command_str);
}

int read_b(int port, char * data)
{
  int time = 0;
  int result;

  while ( (result = read(port, data, 1)) == -1 )
  {
    usleep(USLEEP_STEP);
    time += USLEEP_STEP;

    // On time out (1s), return -1
    if (time >= STEPS_IN_SECOND)
      return -1;
  }

  if (flag_echo_recv)
    fprintf (stdout, "%c", *data);

  // If success, return the result from read() 
  return result;
}

void flush_received(int port)
{
  unsigned char * dummy;

  while (read(port, &dummy, 1) != -1)
    usleep(50);
}

void print_help()
{
  fprintf (stdout, "Usage: stress [options]\n\n");
  fprintf (stdout, "-e\t\techo received data\n");
  fprintf (stdout, "-n\t\ttest noise (only when testing locally)\n");
}

main(int argc, char *argv[]) {
  srandom(time(NULL));

  // Parse arguments
  while (argc > 1)
  {
    if (strcmp(argv[1], "-e") == 0)
      flag_echo_recv = 1;
    else if (strcmp(argv[1], "-n") == 0)
      flag_test_noise = 1;
    else
    {
      fprintf (stderr, "Unknown argument: %s\n", argv[1]);
      print_help();
      return -1;
    }

    argc--;
    argv++;
  }
  
  #if 0
  printf("Opening serial port.\n");
  if (0 > (ser_port = open(DEVICE, O_RDWR | O_NOCTTY | O_NDELAY))) {
    printf("Could not open serial port.\n");
    return -1;
  }
  printf("Serial port open with file descriptor %d.\n", ser_port);
  fcntl(port, F_SETFL, O_NONBLOCK);
  
  printf("Initializing serial port.\n");
  initport(ser_port);
  #endif

  // Flush received data to avoid false alarms
  flush_received(ser_port);

  // Run a bunch of tests; abort if one fails (returns non-zero)
  if (stress_tx(ser_port))
  {
    printf ("Transmit-only failed on ARM.\n");
    return;
  }
 
  #if 0
  closeport(ser_port);
  close(ser_port);
  #endif
  return 0;
}

int stress_tx(int port)
{
  int data_len;
  int i;
  unsigned int root;
  char recv;
  unsigned int bad_char_count;
  int read_result;

  // Request a test of type 0 with length 'data_len'
  for (data_len = 1; data_len <= STRESS_MAX_LEN_TX; data_len *= 2)
  {
    fprintf (stdout, "TX test, data length %d... ", data_len);
    send_header(port, STRESS_TYPE_TX);
    send_int (port, data_len);
    send_string (port, "\r");

    root = 1;
    bad_char_count = 0;
    for (i = 0; i < data_len; i++)
    {
      // receive char
      read_result = read_b(port, &recv);

      // Abort on time out or EOF (which is unexpected)
      if (read_result == -1 || read_result == 0)
        break;

      // compare with what it should be
      if (recv != (unsigned char)(root % 97 + 31))
        bad_char_count++;

      // Compute the next character that we should receive
      root = (root * STRESS_TX_MUL) % STRESS_TX_MOD;
    }

    if (bad_char_count > 0)
    {
      fprintf (stdout, "%d bad characters, aborting.\n", bad_char_count);
      return bad_char_count;
    }
    else if (read_result == -1)
    {
      fprintf (stdout, "timed out while waiting for data.\n");
      return 1;
    }
    else if (read_result == 0)
    {
      fprintf (stdout, "unexpected EOF.\n");
      return 1;
    }
    else
      fprintf (stdout, "OK.\n");
  }

  return 0;
}



char rx_buffer[ARM_BUFFER_SIZE];
char * rx_ptr = rx_buffer;
char tx_buffer[ARM_BUFFER_SIZE];
char * tx_ptr = tx_buffer;
char * tx_head = tx_buffer;

/** Testing interface; emulates receiving and transmitting by the other end */
void test_write(int port, void * data, int ct)
{
  unsigned char * dptr = (unsigned char*)data;
  int i;

  for (i = 0; i < ct; i++)
  {
    *rx_ptr = *dptr;
    // Emulate getline - do we have a stress command?
    if (*rx_ptr == '\r')
    {
      test_command();
      // Clear buffer
      rx_ptr = rx_buffer;
    }
    else
      rx_ptr++; dptr++;
  }
  
}

int test_read(int port, void * data, int ct)
{
  int i;
  unsigned char * dptr = (unsigned char*)data;

  for (i = 0; i < ct; i++)
  {
    if (tx_ptr == tx_head) return -1;

    *dptr = *tx_head;
    dptr++; tx_head++;
    if (tx_head == tx_buffer + sizeof(tx_buffer))
      tx_head = tx_buffer;
  }
}

char tmp_str[64];

void test_command()
{
  char * str_end = strchr(rx_buffer, '\r');
  int type;
  unsigned int length;

  *str_end = 0;
  //fprintf (stderr, "Received '%s'\n", rx_buffer);
  *str_end = '\r';

  // Parse it!
  sscanf (rx_buffer, "%s %s %d %u", tmp_str, tmp_str, &type, &length);

  switch (type)
  {
    case STRESS_TYPE_TX:
      // Generate the data to be sent
      test_send_tx(length);
      break;
    default:
      fprintf (stdout, "ARM emulator received invalid command: %d\n", type);
      break;
  }
}

void test_send_tx(int length)
{
  int root = 1;
  int i;
  char c;
  long r;

  for (i = 0; i < length; i++)
  {
    // Write one more char
    c = (char)(root % 97 + 31);
    if (flag_test_noise)
    {
      r = (long)((random() * 100.0) / (RAND_MAX + 1.0));
      if (r == 0) // Delete character
        c = 0;
      else if (r == 1) // Add an extra character
      {
        *tx_ptr = '!';
        if (tx_ptr++ == tx_buffer + sizeof(tx_buffer))
          tx_ptr = tx_buffer;
        // We ALSO add the c char
      }
      else if (r < 5) // Replace a character
        c = (int)((random() * 97.0) / (RAND_MAX + 1.0)) + 31;
    } 
    
    if (c > 0)
    {
      *tx_ptr = c;
      if (tx_ptr++ == tx_buffer + sizeof(tx_buffer))
        tx_ptr = tx_buffer;
    }
    
    root = (root * STRESS_TX_MUL) % STRESS_TX_MOD;
  }
}

