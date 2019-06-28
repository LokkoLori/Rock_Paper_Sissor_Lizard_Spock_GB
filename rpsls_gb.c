#include <stdio.h>
#include <gb/gb.h>

#define RSPLS_ROCK 0
#define RSPLS_PAPER 1
#define RSPLS_SISSOR 2
#define RSPLS_LIZARD 3
#define RSPLS_SPOCK 4

const char** BEATS = {
	{"", "", "Rock smashes sissor", "Rock crushes lizard", ""},
	{"Paper covers rock", "", "", "", "Paper disprooves Spock"},
	{"", "Sissor cuts paper", "", "Sissor decapitates lizard", ""},
	{"", "Lizard eats paper", "", "", "Lizard poisons Spock"},
	{"Spock vaporises stone", "", "Spock smashes sissor", "", ""}
};

void main(void) {
	while (1) {
		
		_io_out = joypad();
		send_byte();
		while (_io_status == IO_SENDING);
		
		receive_byte();
		while (_io_status == IO_RECEIVING);
		
		if (_io_status == IO_IDLE)
				printf("%d\n", (int)_io_in);
	}
}