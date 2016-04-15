/*
 *	Author: Kevin San Diego
 *
 *	Revision History:
 *	08-Apr-2016 -- Created.
 *  10-Apr-2016 -- Player sprite and platforms set up. (No collision as of yet)
 */

 // Added string.h for strlen()

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "cab202_graphics.h"
#include "cab202_timers.h"
#include "cab202_sprites.h"

// ----------------------------------------------------------------
// Global variables containing "long-term" state of program
// ----------------------------------------------------------------

// Randomise the platform using rand()

// Game is ideal for a 55x70 screen
#define MAX_SCREEN_WIDTH 55
#define MAX_SCREEN_HEIGHT 49 // CHANGE To 70 WHEN DONE

// Timer to count elapsed time
timer_id game_timer;

// Player sprite
sprite_id player;

// Max number of platforms, sprites declaration, and size.
#define N_PLATFORMS 14 // Max number of platforms assuming minimal distance separation
#define PLATFORM_THICKNESS 2
sprite_id platforms[N_PLATFORMS];
int plat_min_width; // 7 for level 1 or 3 for others
int plat_max_width; // 7 for level 1 or 10 for others
int safe_or_deadly; // 0 for safe platform or 1 for deadly platform

// Player-alive status: true if and only if the player died.
bool dead;

// Game-in-progress status: true if and only if the games is finished.
bool over;

// Current-level status: integer for determining current level.
int level;

// Current-lives status: integer for determining current lives.
int lives;

// Current-score status: integer for determining score.
int score;

// ----------------------------------------------------------------
//	Configuration
// ----------------------------------------------------------------
#define LEFT '4'
#define RIGHT '6'
#define UP '8'
#define DOWN '2'
#define LEVEL 'l'
#define QUIT 'q'

// ----------------------------------------------------------------
// Forward declarations of functions
// ----------------------------------------------------------------

void setup();
void setup_player();
void setup_platforms();
void cleanup();
void event_loop();
void draw_hud();
void draw_all();
void pause_for_exit();

char *make_platform();

bool process_key();

int first_platform();
int hori_plat_offset();
int vert_plat_offset();
// ----------------------------------------------------------------
// main function
// ----------------------------------------------------------------

int main( void ) {
	srand(time(NULL));
	setup();
	make_platform();
	event_loop();
	cleanup();
	return 0;
}

/*
 *	Set up the game. Sets the terminal to curses mode and places the player
 */

void setup() {
	dead = false;
	over = false;
	level = 1;
	lives = 3;
	score = 0;

	setup_platforms();
	setup_screen();
	setup_player();
	// To do: Setup platforms
}

/*
 * Set up the player in it's initial position
 */
void setup_player() {
	static char * player_bitmap = 
	"o"
	"T"
	"^";

player = sprite_create((MAX_SCREEN_WIDTH / 2), (MAX_SCREEN_HEIGHT) - 6, 1, 3, player_bitmap);
}

/*
 * Makes a block of varying properties
 */
char *make_platform(int min_width, int max_width, int type) {
	char * platform_type = "";
	int platform_width = 0;

	if(type == 0) {
		platform_type = "=";
	}
	else if(type == 1) {
		platform_type = "x";
	}

	if(level == 1) {
		platform_width = min_width;
	}
	else {
		do {
			platform_width = (rand() % max_width);
		} while(platform_width < min_width);
	}

	platform_width = platform_width * 2;
	char * platform_bitmap = malloc(platform_width + 1);

	for(int i = 0; i < platform_width; i++) {
		platform_bitmap[i] = *platform_type;
	}
	return platform_bitmap;
	free(platform_bitmap);
}

/*
 * Generates an x location the first platform at random
 */
int first_platform(int platform_width) {
	int first_x_pos = rand() % (MAX_SCREEN_WIDTH - 1 - platform_width);
	return first_x_pos;
}

/*
 * Generates the horizontal space between each platform
 */
int hori_plat_offset(int prev_x_pos, int prev_plat_width, int platform_width) {
	int new_hori_offset;
	int offset_side = rand() % 2;

	do {
		new_hori_offset = rand() % 16;
	} while(new_hori_offset < 3);

	if(((new_hori_offset + prev_x_pos + prev_plat_width) > MAX_SCREEN_WIDTH - 1) && (prev_x_pos - (new_hori_offset + platform_width) < 0)) {
		if(offset_side == 0) {
			new_hori_offset = prev_x_pos - (new_hori_offset + platform_width);
		}
		else if(offset_side == 1) {
			new_hori_offset = new_hori_offset + prev_x_pos + prev_plat_width;
		}
	}	
	else if((new_hori_offset + prev_x_pos + prev_plat_width) > MAX_SCREEN_WIDTH - 1) {
		new_hori_offset = prev_x_pos - (new_hori_offset + platform_width);
	}
	else if(prev_x_pos - (new_hori_offset + platform_width) < 0) {
		new_hori_offset = new_hori_offset + prev_x_pos + prev_plat_width;
	}
	return new_hori_offset;
}

/*
 * Generates the vertical space between each platform
 */
int vert_plat_offset() {
	int new_vert_offset;

	do {
		new_vert_offset = (rand() % 9);
	} while(new_vert_offset < 4);

	return new_vert_offset;
}

/*
 * Places the platforms in their positions
 */
void setup_platforms() {
	int x_pos;
	int y_pos;
	int true_width;
	int prev_x_pos;
	int prev_plat_width;
	int prev_y_pos;
	int x_offset;
	int y_offset;
	char * bmap;
	
	if(level == 1) {
		plat_min_width = 7;
		plat_max_width = 7;
	}
	else {
		plat_min_width = 3;
		plat_max_width = 10;
	}

	for(int i = 0; i < 13; i++) {
		int type = rand() % 2;
		bmap = make_platform(plat_min_width, plat_max_width, type);
		true_width = strlen(bmap) / 2;

		if(i == 0) {
			x_pos = first_platform(true_width);
			y_pos = 15; // TEMPORARY TEMPORARY TEMPORARY TEMPORARY
		}
		else {
			prev_x_pos = platforms[i -1]->x;
			prev_y_pos = platforms[i - 1]->y;
			prev_plat_width = platforms[i - 1]->width;
			x_offset = hori_plat_offset(prev_x_pos, prev_plat_width, true_width);
			y_offset = vert_plat_offset();

			x_pos = x_offset;
			y_pos = prev_y_pos + y_offset;
			printf("%d in x   %d in y\n", x_pos, y_pos); // TEMPORARY TEMPORARY TEMPORARY TEMPORARY
		}



		platforms[i] = sprite_create(x_pos, y_pos, true_width, 2, bmap);
	}
}

/*
 * Restore the terminal to normal mode
 */
void cleanup() {
	cleanup_screen();
}

/*
 * Displays a message and waits for a keypress before exiting
 */
void pause_for_exit() {
	sprite_id exit_sprite;

	static char * exit_bitmap = 
	" #######  ####### "
	" #     #  #     # "
	" #        #       "
	" #   ###  #   ### "
	" #     #  #     # "
	" #######  ####### ";

	//draw_line(0, 0, MAX_SCREEN_WIDTH - 1, MAX_SCREEN_HEIGHT - 1, ' '); //TO DO MAKE 3 more to clear screen
	exit_sprite = sprite_create((MAX_SCREEN_WIDTH - 16) / 2, (MAX_SCREEN_HEIGHT - 5) / 2, 18, 6, exit_bitmap);
	sprite_draw(exit_sprite);
	wait_char();
}


/*
 * Processes keyboard timer events to progress game
 */
void event_loop() { // To do: process human movement
	draw_all();

	while(!over) {
		bool must_redraw = false;

		must_redraw = must_redraw || process_key();
		//must_redraw = must_redraw || process_timer(); // To Do

		if(must_redraw) {
			draw_all();
		}
		timer_pause(20);
	}
	pause_for_exit();
}

/*
 * Draws the heads-up display
 */
void draw_hud() {
	draw_line(0, 1, (MAX_SCREEN_WIDTH - 1), 1, '-');
	draw_formatted(0, 0, "Lives: %d", lives);
	draw_formatted((MAX_SCREEN_WIDTH - 19), 0, "Time Elapsed: 04:20");
	draw_line(0, (MAX_SCREEN_HEIGHT - 2), (MAX_SCREEN_WIDTH - 1), (MAX_SCREEN_HEIGHT - 2), '-');
	draw_formatted(0, (MAX_SCREEN_HEIGHT - 1), "Level: %d with a Score: %d", level, score);
	if(over) {
		draw_string((MAX_SCREEN_WIDTH / 2) - 13, (MAX_SCREEN_HEIGHT / 2), "No more lives remaining...");
		draw_string((MAX_SCREEN_WIDTH / 2) - 17, (MAX_SCREEN_HEIGHT / 2) + 1, "Press 'r' to restart or 'q' to quit.");
	}
}

/*
 * Process keyboard events, returning true if and only if the
 * player's position has changed
 */
bool process_key() {
	int key = get_char();

	if(key == QUIT) {
		over = true;
		return false;
	}

	// Remember original position
	int x0 = player->x;
	int y0 = player->y;

	// Update position
	if(key == LEFT) {
		player->x--;
	}
	else if(key == RIGHT) {
		player->x++;
	}
	else if(key == UP) {
		player->y--;
	}
	else if(key == DOWN) {
		player->y++;
	}

	// Make sure still inside window
	while(player->x < 0) player->x++;
	while(player->y < 2) player->y++;
	while(player->x > MAX_SCREEN_WIDTH - 1) player->x--;
	while((player->y + 2) > MAX_SCREEN_HEIGHT - 3) player->y--;

	return x0 != player->x || y0 != player->y;

}

/*
 * Draws whatever is to be displayed
 */
void draw_all() {
	clear_screen();

	for(int i = 0; i < 13; i++){
		sprite_draw(platforms[i]);
	}

	draw_hud();
	sprite_draw(player);
	show_screen();

}












