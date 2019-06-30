#include <stdio.h>
#include <rand.h>
#include <gb/gb.h>
#include "rpsls_map.c"

#define RSPLS_ROCK 0
#define RSPLS_PAPER 1
#define RSPLS_SISSOR 2
#define RSPLS_LIZARD 3
#define RSPLS_SPOCK 4
#define RSPLS_UNDEF 128

#define GAME_CODE 31

#define PH_MATCHMAKING 0
#define PH_CHOOSING 1
#define PH_MATCHING 2

const char* const RSPLS_NAMES[5] = {
	"ROCK", 
	"PAPER", 
	"SISSOR", 
	"LIZARD", 
	"SPOCK"
};

const char* const RSPLS_BEATS[25] = {
	"", "", "smashes", "crushes", "",
	"covers", "", "", "", "disprooves",
	"", "cuts", "", "decapitates", "",
	"", "eats", "", "", "poisons",
	"vaporises", "", "smashes", "", ""
};

INT16 player_num = 1;
INT16 opponent_choise = RSPLS_UNDEF;
INT16 my_choise = RSPLS_ROCK;
INT16 game_phase = PH_MATCHMAKING;
INT16 i = 0;
INT16 j = 0;
INT16 d = 0;
INT16 seed = 0;
INT16 seedinited = 0;
INT16 points_mine = 0;
INT16 points_his = 0;

INT16 ramapos_x = 0;
INT16 ramapos_y = 0;

void matchmaking(){
	
	printf("READY.GAME.JAM!\n\n");
	for (i=0;i<5;i++){
		printf("%s\n", RSPLS_NAMES[i]);
	}
	printf("\n\nStart = Host game\nA = join to game\nB = single play\n\n");
	
	while (1){
		if (joypad() == J_B) {
			waitpadup();
			printf("Single player mode\n\n");
			player_num = 0;
			break;
		}		
		if (joypad() == J_A) {
			waitpadup();
			printf("Joining game\n\n");
			player_num = 2;
			//todo: checking host
			break;
		}
		if (joypad() == J_START) {
			waitpadup();
			printf("Hosting game\n\n");
			player_num = 1;
			//todo: wainting player
			break;
		}
	}
		
	game_phase = PH_CHOOSING;
}

void selection_changed(){
	if (my_choise < 0){
		my_choise = 4;
	}
	if (4 < my_choise){
		my_choise = 0;
	}
	printf("Select: %d %s\n", my_choise, RSPLS_NAMES[my_choise]);
}
	

void choosing(){
	
	if (player_num == 2) {
		receive_byte();
	}
	
	selection_changed();
	while(1){
		if (!seedinited)
			seed++;
		if (joypad() == J_RIGHT || joypad() == J_DOWN) {
			waitpadup();
			my_choise++;
			selection_changed();
		}
		if (joypad() == J_LEFT || joypad() == J_UP){
			waitpadup();
			my_choise--;
			selection_changed();
		}
		if (joypad() == J_A){
			waitpadup();
			printf("\n\nYour choise:\n %d %s\n", my_choise, RSPLS_NAMES[my_choise]);
			break;
		}
	}
	
	if (!seedinited){
		initarand(seed);
		//printf("rand inited with seed: %d\n", seed);
		seedinited = 1;
	}
	
	if (player_num == 0) {
		
		// single player random with opponent
		opponent_choise = rand();
		if (opponent_choise<0)
			opponent_choise = -opponent_choise;
		opponent_choise = opponent_choise % 5;
	} else if (player_num == 1) {
		//player 1 startegie: send first, recevieve after
		_io_out = my_choise;
		send_byte();
		while (_io_status == IO_SENDING);
		
		receive_byte();
		while (_io_status == IO_RECEIVING);
		opponent_choise = (int)_io_in;
		
	} else if (player_num == 2) {
		
		//player 2 startegie: recevieve first, then send
		while (_io_status == IO_RECEIVING);
		opponent_choise = (int)_io_in;
		
		_io_out = my_choise;
		send_byte();
		while (_io_status == IO_SENDING);
	}
	
	//opponent_choise = 4; //fixed select for testing
	
	printf("Opponent's choise:\n %d %s\n\n", opponent_choise, RSPLS_NAMES[opponent_choise]);
	game_phase = PH_MATCHING;
}


void matching(){
	
	if (my_choise == opponent_choise) {
		printf("TIE!\n\n");
	} else {
		char tc = RSPLS_BEATS[my_choise*5+opponent_choise][0];
		if (tc != '\0'){
			printf("%s %s %s\n\nYOU WIN!\n\n", RSPLS_NAMES[my_choise], RSPLS_BEATS[my_choise*5+opponent_choise], RSPLS_NAMES[opponent_choise]);
			points_mine++;
		} else {
			printf("%s %s %s\n\nYOU LOOSE!\n\n", RSPLS_NAMES[opponent_choise], RSPLS_BEATS[opponent_choise*5+my_choise], RSPLS_NAMES[my_choise]);
			points_his++;
		}
	}
	
	printf("scores %d:%d\n\n", points_mine, points_his);
	game_phase = PH_CHOOSING;
}

void set_rama_sprite(){
	
	SPRITES_8x8;
	
	set_sprite_data(0, 88, RPSLS_MAP);
	
	set_sprite_tile(0, 80);
	set_sprite_tile(1, 81);
	set_sprite_tile(2, 81);
	set_sprite_tile(3, 82);
	set_sprite_tile(4, 83);
	set_sprite_tile(5, 83);
	set_sprite_tile(6, 84);
	set_sprite_tile(7, 85);
	set_sprite_tile(8, 85);
	set_sprite_tile(9, 86);
	set_sprite_tile(10, 87);
	set_sprite_tile(11, 87);
}

void show_rame_sprite_at(INT16 x, INT16 y) {

	move_sprite(0, x, y);
	move_sprite(1, x+8, y);
	move_sprite(2, x+16, y);
	move_sprite(3, x+24, y);
	move_sprite(4, x+24, y+8);
	move_sprite(5, x+24, y+16);
	move_sprite(6, x+24, y+24);
	move_sprite(7, x+16, y+24);
	move_sprite(8, x+8, y+24);
	move_sprite(9, x, y+24);
	move_sprite(10, x, y+16);
	move_sprite(11, x, y+8);
	
	
	SHOW_SPRITES;
}

void send_rama_to(INT16 x, INT16 y, INT16 in_step) {
	
	// this function is a pile of dogshit!!!
	
	INT16 x_count = x - ramapos_x;
	INT16 y_count = y - ramapos_y;
	
	INT16 x_step = in_step / x_count;
	INT16 y_step = in_step / y_count;
	
	printf("%d %d %d\n", y, ramapos_y, y_count);
	
	x_count = 0;
	y_count = 0;
	
	for(i=0;i<in_step;i++){
		if (x_step < 0)
			x_count--;
		else
			x_count++;
		if (x_count == x_step) {
			x_count = 0;
			if (x_step < 0)
				ramapos_x--;
			else
				ramapos_x++;
			show_rame_sprite_at(ramapos_x, ramapos_y); 
		}
		
		if (y_step < 0)
			y_count--;
		else
			y_count++;
		if (y_count == y_step) {
			y_count = 0;
			if (y_step < 0)
				ramapos_y--;
			else
				ramapos_y++;
			show_rame_sprite_at(ramapos_x, ramapos_y); 
		}
	}
	
	ramapos_x = x;
	ramapos_y = y;
	
	show_rame_sprite_at(ramapos_x, ramapos_y); 
}

void main(void) {
	
	INT16 a = 0;
	printf("%d\n", a);
	a = a - 90;
	printf("%d\n", a);
	
	set_rama_sprite();
	send_rama_to(45, 90, 10000);
	
	send_rama_to(90, 0, 10000);
	
	while(1)
		seed++;
	
	while (1) {
		
		switch (game_phase) {
			case PH_MATCHMAKING:
				matchmaking();
				break;
			case PH_CHOOSING:
				choosing();
				break;
			case PH_MATCHING:
				matching();
				break;
				
		}
	}
}