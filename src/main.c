#include <stdio.h>
#include <stdint.h>

#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>

#include "midi_parser.h"
#include "serial.h"


#define SERIAL_PORT
#define SEND_SERIAL
#define REAL_TIME
#define SHOW_KEYBOARD


void serial_midi_event_send(int fd, uint8_t note, uint8_t event_on)
{
    static const int WAIT = 2;

    note -= 21;
    note &= 0x7F;

    if (event_on)
        note |= 0x80;

    write(fd, &note, 1);
    usleep((useconds_t) (WAIT * 835));
}


void show_keyboard(uint8_t *notes, size_t size, FILE *output)
{
    for (size_t i = 21; i <= 108; ++i) {
        putc(notes[i] ? 'H' : '.', output);
    }

    putc('\n', output);
}

uint8_t midi_parse(FILE *midi, FILE *output, int fd)
{
    struct midi_parser parser[1];
	midi_parser_new(parser, midi);

    uint8_t note, event_on, notes[128] = { 0 };

	// for (; !parser->end_of_file; parser->timestamp += parser->dtime) {
	for (struct midi_event event; !midi_parser_eof(parser);) {
		midi_parser_next(parser, midi, &event);
        event_on = 0;
		switch (MIDI_EVENT_TYPE(&event)) {
			case EventNoteOn:
				event_on = event.midi_data[1] != 0;
			case EventNoteOff:
				note = event.midi_data[0];
				notes[note] = event_on;
                #ifdef SEND_SERIAL
                    serial_midi_event_send(fd, note, event_on);
                #endif
		}

        #ifdef SHOW_KEYBOARD
            show_keyboard(notes, 128, output);
        #endif
        #ifdef REAL_TIME
            usleep((useconds_t) MIDI_DELAY(parser));
        #endif
	}

	return 0;
}

int main(int argc, char **argv)
{
    const char *portname = "/dev/ttyUSB1";

    int fd = 1;
    uint8_t return_status;

    FILE
    *midi = stdin,
    *output = stderr;

    switch (argc) {
    case 3:
        output = fopen(argv[2], "wb");
    case 2:
        midi = fopen(argv[1], "rb");
    }

    #ifdef SERIAL_PORT
        fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
        if (fd < 0) {
            printf("Error %d opening %s: %s", errno, portname, strerror(errno));
            return -1;
        }

        serial_interface_set(fd, B9600, 0);
        serial_blocking_set(fd, 0);
    #endif

    return_status = midi_parse(midi, output, fd);

    fclose(midi);
    fclose(output);
    return return_status;
}