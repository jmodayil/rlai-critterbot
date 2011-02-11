#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <unistd.h>

#include "CritterSerialLink.h"

static std::string serialPort("/dev/ttyS0");

CritterSerialLink::CritterSerialLink(ActionWriter& actionWriter) :
	actionWriter(actionWriter) {
	fid = -1;
}

CritterSerialLink::~CritterSerialLink() {
}

void CritterSerialLink::initport() {
	printf("Initializing serial port.\n");
	struct termios options;
	tcflush(fid, TCIOFLUSH);
	tcgetattr(fid, &oldterm);
	tcgetattr(fid, &options);
	options.c_cflag &= ~(CSIZE | CSTOPB | CRTSCTS);
	options.c_cflag |= (CS8 | CLOCAL | CREAD | PARENB | PARODD);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	//options.c_lflag = 0;
	options.c_iflag &= ~(IXON | IXOFF | OCRNL);
	//options.c_iflag = 0;
	options.c_iflag |= (INPCK | PARMRK | IGNBRK);
	//options.c_oflag &= ~OPOST;
	options.c_oflag = 0;
	options.c_cc[VTIME] = 0;
	options.c_cc[VMIN] = 1;
	cfsetispeed(&options, BAUDRATE);
	cfsetospeed(&options, BAUDRATE);
	tcflush(fid, TCIOFLUSH);
	tcsetattr(fid, TCSANOW, &options);
}

void CritterSerialLink::close() {
	tcsetattr(fid, TCSANOW, &oldterm);
	tcflush(fid, TCIOFLUSH);
}

bool CritterSerialLink::init() {
	printf("Opening serial port.\n");
	// if (0 > (fid = open(serialPort.c_str(), O_RDWR | O_NOCTTY | O_NDELAY))) {
	if (0 > (fid = open(serialPort.c_str(), O_RDWR | O_NOCTTY | O_SYNC))) {
		perror("Could not open serial port.\n");
		return false;
	}
	printf("Serial port open with file descriptor %d.\n", fid);
	// We want the serial port to be blocking
	// fcntl(fid, F_SETFL, O_NONBLOCK);
	initport();
	return actionWriter.start(fid);
}

// Weird: select() does not seems to wait for data to be available
void CritterSerialLink::waitForData() {
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO(&rfds);
	FD_SET(fid, &rfds);

	tv.tv_sec = 5;
	tv.tv_usec = 0;

	retval = select(fid + 1, &rfds, NULL, NULL, &tv);

	if (retval == -1)
		perror("CritterSerialLink::waitForData");
	else if (retval == 0)
		printf("No data within five seconds.\n");
}

CritterStateDrop* CritterSerialLink::parseDataToDrop(unsigned char buf[]) {
	int i;
	unsigned char holding;
	unsigned short their_crc;
	// USeconds *theTime
	i = 0;
	// stateDrop.time = *theTime;
	stateDrop.time.setAsNow();
	stateDrop.bus_voltage = (int) buf[i++];
	stateDrop.charge_state = (CritterStateDrop::Charge_State) buf[i++];
	stateDrop.batv40 = (unsigned char) buf[i++];
	stateDrop.batv160 = (unsigned char) buf[i++];
	stateDrop.batv280 = (unsigned char) buf[i++];
	stateDrop.motor100.command = (char) buf[i++];
	stateDrop.motor100.velocity = (char) buf[i++];
	stateDrop.motor100.current = (int) buf[i++];
	stateDrop.motor100.temp = (int) buf[i++];
	stateDrop.motor220.command = (char) buf[i++];
	stateDrop.motor220.velocity = (char) buf[i++];
	stateDrop.motor220.current = (int) buf[i++];
	stateDrop.motor220.temp = (int) buf[i++];
	stateDrop.motor340.command = (char) buf[i++];
	stateDrop.motor340.velocity = (char) buf[i++];
	stateDrop.motor340.current = (int) buf[i++];
	stateDrop.motor340.temp = (int) buf[i++];

	stateDrop.accel.x = ((char) buf[i++]) << 4;
	stateDrop.accel.y = ((char) buf[i++]) << 4;
	stateDrop.accel.z = ((char) buf[i++]) << 4;
	holding = ((unsigned char) buf[i++]);
	stateDrop.accel.x |= (holding >> 4);
	stateDrop.accel.y |= (holding & 0x0F);
	holding = ((unsigned char) buf[i++]);
	stateDrop.accel.z |= (holding >> 4);
	stateDrop.mag.x = ((unsigned char) buf[i++]) << 2;
	stateDrop.mag.y = ((unsigned char) buf[i++]) << 2;
	stateDrop.mag.z = ((unsigned char) buf[i++]) << 2;
	stateDrop.rotation = ((char) buf[i++]) << 2;
	holding = ((unsigned char) buf[i++]);
	stateDrop.mag.x |= (holding >> 6);
	stateDrop.mag.y |= (holding & 0x30) >> 4;
	stateDrop.mag.z |= (holding & 0x0C) >> 2;
	stateDrop.rotation |= (holding & 0x03);
	for (int j = 0; j < 10; j++)
		stateDrop.ir_distance[j] = (int) buf[i++];
	holding = ((unsigned char) buf[i++]);
	stateDrop.ir_distance[0] |= (holding >> 6);
	stateDrop.ir_distance[1] |= (holding & 0x30) >> 4;
	stateDrop.ir_distance[2] |= (holding & 0x0C) >> 2;
	stateDrop.ir_distance[3] |= (holding & 0x03);
	holding = ((unsigned char) buf[i++]);
	stateDrop.ir_distance[4] |= (holding >> 6);
	stateDrop.ir_distance[5] |= (holding & 0x30) >> 4;
	stateDrop.ir_distance[6] |= (holding & 0x0C) >> 2;
	stateDrop.ir_distance[7] |= (holding & 0x03);
	holding = ((unsigned char) buf[i++]);
	stateDrop.ir_distance[8] |= (holding >> 6);
	stateDrop.ir_distance[9] |= (holding & 0x30) >> 4;
	for (int j = 0; j < 4; j++)
		stateDrop.light[j] = ((int) buf[i++]) << 2;
	holding = ((unsigned char) buf[i++]);
	stateDrop.light[0] |= (holding >> 6);
	stateDrop.light[1] |= (holding & 0x30) >> 4;
	stateDrop.light[2] |= (holding & 0x0C) >> 2;
	stateDrop.light[3] |= (holding & 0x03);
	for (int j = 0; j < 8; j++) {
		stateDrop.thermal[j] = (unsigned int) buf[i++];
		stateDrop.thermal[j] |= ((unsigned int) buf[i++] << 8);
	}
	for (int j = 0; j < 8; j++)
		stateDrop.ir_light[j] = ((unsigned int) buf[i++]) << 2;
	holding = ((unsigned char) buf[i++]);
	stateDrop.ir_light[0] |= (holding >> 6);
	stateDrop.ir_light[1] |= (holding & 0x30) >> 4;
	stateDrop.ir_light[2] |= (holding & 0x0C) >> 2;
	stateDrop.ir_light[3] |= (holding & 0x03);
	holding = ((unsigned char) buf[i++]);
	stateDrop.ir_light[4] |= (holding >> 6);
	stateDrop.ir_light[5] |= (holding & 0x30) >> 4;
	stateDrop.ir_light[6] |= (holding & 0x0C) >> 2;
	stateDrop.ir_light[7] |= (holding & 0x03);
	stateDrop.error_flags = buf[i++] << 24;
	stateDrop.error_flags |= buf[i++] << 16;
	stateDrop.error_flags |= buf[i++] << 8;
	stateDrop.error_flags |= buf[i++];
	stateDrop.cycle_time = (int) buf[i++];
	stateDrop.monitor_state = buf[i++];
	their_crc = ((unsigned short) buf[i++]) << 8;
	their_crc += buf[i++];
	if (stateDrop.bus_voltage > 170)
		stateDrop.power_source = CritterStateDrop::SHORE;
	else
		stateDrop.power_source = CritterStateDrop::BATTERY;
	return &stateDrop;
}

bool CritterSerialLink::sendActionIfPossible(
		const CritterControlDrop& controlDrop) {
	return actionWriter.sendActionIfPossible(controlDrop);
}

bool CritterSerialLink::parseByte(unsigned char buf) {
	static enum {
		HEADER, DATA, FLAG
	} state = HEADER;
	unsigned char header[] = { SER_HEADER1, SER_HEADER2, SER_HEADER3,
			SER_HEADER4 };
	static int i;
	switch (state) {
	case HEADER:
		if (buf == header[i]) {
			if (i >= 3) {
				i = 0;
				state = DATA;
			} else
				i++;
			break;
		}
		fprintf(stderr, "Misaligned Packet!!!\n");
		break;
	case DATA:
		if (buf == 0xFF) {
			state = FLAG;
			break;
		}
		data[i++] = buf;
		if (i == STATE_LENGTH) {
			i = 0;
			state = HEADER;
			return true;
		}
		break;
	case FLAG:
		if (buf == 0xFF) {
			data[i++] = buf;
			if (i == STATE_LENGTH) {
				i = 0;
				state = HEADER;
				return true;
			}
			state = DATA;
			break;
		} else if (buf == 0x00) {
			if (read(fid, &buf, 1) == 1)
				fprintf(stderr, "Parity error on buf: %u\n", buf);
			else
				fprintf(stderr, "Error reading from serial port?!?!\n");
			state = HEADER;
			i = 0;
			break;
		} else {
			fprintf(stderr, "Error, not sure how this could happen, buf: %u",
					buf);
			i = 0;
			state = HEADER;
			break;
		}
	default:
		i = 0;
		state = HEADER;
		break;
	}
	return false;
}

CritterStateDrop* CritterSerialLink::readStateDrop() {
	static unsigned char buf;
	while (read(fid, &buf, 1) == 1) {
		if (parseByte(buf))
			return parseDataToDrop(data);
	}
	return NULL;
}
