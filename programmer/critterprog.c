#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "crctable.h"
#include <stdlib.h>

#define BAUDRATE B115200
#define DEVICE "/dev/ttyS0"

FILE *boot;
int port;

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

  int data, i, header;
  unsigned char dat;
  unsigned short crc;
  char *file_name;
 
  if (argc < 2)
  {
    printf ("Specify file to be uploaded.\n");
    return -1;
  }
  
  file_name = argv[1];
 
  printf("Opening boot file.\n"); 
  if(NULL == (boot = fopen(file_name, "r"))) {
    printf("Could not open file: %s\n", file_name);
    return -1;
  }

  printf("Opening serial port.\n");
  if (0 > (port = open(DEVICE, O_RDWR | O_NOCTTY | O_NDELAY))) {
    printf("Could not open serial port.\n");
    return -1;
  }
  printf("Serial port open with file descriptor %d.\n", port);
  //fcntl(port, F_SETFL, O_NONBLOCK);
  
  printf("Initializing serial port.\n");
  initport(port);
  crc = 0;
  i = 0;
  header = 1;
  printf("Sending file\n");
  while( EOF != (data = fgetc(boot))) {
    if(0 == (i % 1024))
      printf("%d bytes sent.\n", i);
    dat = (unsigned char)data;
    if(!header)
      crc = (crc<<8) ^ crctable[(crc >> 8) ^ dat];
    if('\r' == dat)
      header = 0;
    while(1 != (write(port, &dat, 1)));
    i++;
    usleep(50);
  }
  printf("%d bytes sent.", i);
  dat = crc >> 8;
  write(port, &dat, 1);
  dat = (unsigned char)crc;
  write(port, &dat, 1);
  printf("\nDone.\n");
  do {
    if(1 == read(port, &dat, 1)){
      printf("%c", dat);
      if( '\r' == dat || '\n' == dat)
        printf("\n");
    }
  } while(1);
  closeport(port);
  fclose(boot);
  close(port);
  return 0;
}
