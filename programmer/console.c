#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "crctable.h"
#include <stdlib.h>

#define BAUDRATE B115200
#define DEVICE "/dev/ttyS0"

int port;

struct termios oldterm;

void initport(int port) {
  
  struct termios options;
  tcflush(port, TCIOFLUSH);
  tcgetattr(port, &oldterm);
  tcgetattr(port, &options);
  options.c_cflag &= ~(CSIZE | CSTOPB | CRTSCTS);
  options.c_cflag |= (CS8 | CLOCAL | CREAD | PARENB | PARODD);
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_iflag &= ~(INPCK | IXON | IXOFF);
  options.c_iflag |= IGNPAR | OCRNL;
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

main(int argc, char *argv[]) {

  unsigned char data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xA8, 0, 0, 0, 0xB8};
 
  printf("Opening serial port.\n");
  if (0 > (port = open(DEVICE, O_RDWR | O_NOCTTY | O_NDELAY))) {
    printf("Could not open serial port.\n");
    return -1;
  }
  printf("Serial port open with file descriptor %d.\n", port);
  //fcntl(port, F_SETFL, O_NONBLOCK);
  
  printf("Initializing serial port.\n");
  initport(port);

	printf("Dumping to console.\n");
	write(port, &data, 9);
  
	closeport(port);
  close(port);
  return 0;
}
