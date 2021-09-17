#ifndef SERIAL_H
#define SERIAL_H


#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>


static int serial_interface_set(int fd, int speed, int parity)
{
	struct termios tty;
	if (tcgetattr(fd, &tty) != 0) {
		printf("Error %d from tcgetattr: %s\n", errno, strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	// 8-bit chars
	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

	// Disable break processing (IGNBRK) for mismatched speed tests;
	// otherwise receive break as \000 chars

	tty.c_iflag &= ~IGNBRK;

	// No signaling chars, no echo,
	tty.c_lflag = 0;

	// No canonical processing
	tty.c_oflag = 0;

	// No remapping, no delays, read doesn't block
	tty.c_cc[VMIN]  = 0;

	// 0.5 seconds read timeout
	tty.c_cc[VTIME] = 5;

	// Shut off xon/xoff ctrl
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);

	// Ignore modem controls, enable reading
	tty.c_cflag |= (CLOCAL | CREAD);

	// Shut off parity
	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error %d from tcsetattr\n", errno);
		return -1;
	}

	return 0;
}

static void serial_blocking_set(int fd, int should_block)
{
	struct termios tty;
	memset(&tty, 0, sizeof(struct termios));

	if (tcgetattr(fd, &tty) != 0) {
		printf("Error %d from tggetattr\n", errno);
		return;
	}

	tty.c_cc[VMIN] = should_block ? 1 : 0;

	// 0.5 seconds read timeout
	tty.c_cc[VTIME] = 5;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error %d setting term attributes\n", errno);
		return;
	}
}


#endif  /* SERIAL_H */