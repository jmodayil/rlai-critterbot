#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
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
  options.c_iflag &= ~(OCRNL | IXON | IXOFF);
  options.c_iflag |= (INPCK | PARMRK | IGNBRK);
  options.c_oflag = 0; 
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

  int force = 0;
  int i;
  char buf;
  unsigned char data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x69, 0x00, 0x00, 0x00, 0x00}; // 0x01 for enable charging
  unsigned char leds[48];

  if(argc > 1) {
    if(strcmp(argv[1],"-f") == 0)
      force = 1;
  }
  
  printf("Opening serial port.\n");
  if (0 > (port = open(DEVICE, O_RDWR | O_NOCTTY | O_NDELAY))) {
    printf("Could not open serial port.\n");
    return -1;
  }
  printf("Serial port open with file descriptor %d.\n", port);
  fcntl(port, F_SETFL, O_NONBLOCK);
  
  printf("Initializing serial port.\n");
  initport(port);
  
  printf("Listening for data...");
  for(i = 0; i < 200; i++) {
    if(read(port, &buf, 1) != -1) {
      force = 1;
    }
    usleep(100);
  }
 
  if(!force) {
    printf("Connection doesn't seem to be active.\nTry -f to force\n");
    closeport(port);
    close(port);
    return 0;
  }
  printf("OK\n");
  printf("\n---------------\nEnabling Charging\n");
  for(i = 0; i < 1; i++) {
    write(port, &data, 9);
    //write(port, &data, 48);
    usleep(10000);
  }
  closeport(port);
  close(port);
  return 0;
}
