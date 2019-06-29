#include <stdio.h>
#include <rand.h>
#include <gb/gb.h>

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

int player_num = 1;
int opponent_choise = RSPLS_UNDEF;
int my_choise = RSPLS_ROCK;
int game_phase = PH_MATCHMAKING;
int i = 0;
int j = 0;
int d = 0;
int seed = 0;
int seedinited = 0;
int points_mine = 0;
int points_his = 0;

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


void main(void) {
	
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