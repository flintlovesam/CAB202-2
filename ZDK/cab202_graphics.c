/*
 * Graphics.c
 *
 * Simple character-based "graphics" library for CAB202, 2016 semester 1.
 *
 *	Authors:
 *	 	Lawrence Buckingham
 *	 	Benjamin Talbot 
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cab202_graphics.h"
#include "cab202_timers.h"
#include "curses.h"

#define ABS(x)	(((x) >= 0) ? (x) : -(x))
#define SIGN(x)	(((x) > 0) - ((x) < 0))

bool auto_save_screen = false;

/*
 *	Screen structure is used to manage an overridden screen
 */
typedef struct Screen {
	int width;
	int height;
	char * buffer;
} Screen;

/*
 *	If non-null, refers to the extended "override" screen.
 */
Screen * override_screen = NULL;

/**
 *	Set up the terminal display for curses-based graphics.
 */
void setup_screen( void ) {
	// Enter curses mode.
	initscr();

	// Do not echo keypresses.
	noecho();

	// Turn off the cursor.
	curs_set( 0 );

	// Cause getch to return ERR if no key pressed within 0 milliseconds.
	timeout( 0 );

	// Enable the keypad.
	keypad( stdscr, TRUE );

	// Erase any previous content that may be lingering in this screen.
	clear();
}

/**
*	Restore the terminal to its normal operational state.
*/
void cleanup_screen( void ) {
	// cleanup curses.
	endwin();

	// cleanup the extended override screen, if it exists.
	if ( override_screen != NULL ) {
		free( override_screen->buffer );
		free( override_screen );
		override_screen = NULL;
	}
}

/**
*	Clear the terminal window.
*/
void clear_screen( void ) {
	// Clear the curses screen.
	clear();

	// Erase the contents of the current window.
	if ( override_screen != NULL ) {
		int w = override_screen->width;
		int h = override_screen->height;
		char * scr = override_screen->buffer;
		memset( scr, ' ', w * h );
	}
}

/**
*	Make the current contents of the window visible.
*/
void show_screen( void ) {
	// Save a screen shot, if automatic saves are enabled. 
	if ( auto_save_screen ) {
		save_screen();
	}

	// Force an update of the curses display.
	refresh();
}

/**
*	Draws the specified character at the prescibed location (x,y) on the window.
*/
void draw_char( int x, int y, char value ) {
	// Always attempt to display the character, regardless of the size of the overridden screen.
	mvaddch( y, x, value );

	// Update the overridden screen as well.
	if ( override_screen != NULL ) {
		int w = override_screen->width;
		int h = override_screen->height;

		if ( x >= 0 && x < w && y >= 0 && y < h ) {
			char * scr = override_screen->buffer;
			scr[x + y * w] = value;
		}
	}
}

void draw_line( int x1, int y1, int x2, int y2, char value ) {
	if ( x1 == x2 ) {
		// Draw vertical line
		for ( int i = y1; ( y2 > y1 ) ? i <= y2 : i >= y2; ( y2 > y1 ) ? i++ : i-- ) {
			draw_char( x1, i, value );
		}
	}
	else if ( y1 == y2 ) {
		// Draw horizontal line
		for ( int i = x1; ( x2 > x1 ) ? i <= x2 : i >= x2; ( x2 > x1 ) ? i++ : i-- ) {
			draw_char( i, y1, value );
		}
	}
	else {
		// Get Bresenhaming...
		float dx = x2 - x1;
		float dy = y2 - y1;
		float err = 0.0;
		float derr = ABS( dy / dx );

		for ( int x = x1, y = y1; ( dx > 0 ) ? x <= x2 : x >= x2; ( dx > 0 ) ? x++ : x-- ) {
			draw_char( x, y, value );
			err += derr;
			while ( err >= 0.5 && ( ( dy > 0 ) ? y <= y2 : y >= y2 ) ) {
				draw_char( x, y, value );
				y += ( dy > 0 ) - ( dy < 0 );

				err -= 1.0;
			}
		}
	}
}

void draw_string( int x, int y, char * text ) {
	for ( int i = 0; text[i]; i++ ) {
		draw_char( x + i, y, text[i] );
	}
}

void draw_int( int x, int y, int value ) {
	char buffer[100];
	sprintf( buffer, "%d", value );
	draw_string( x, y, buffer );
}

void draw_double( int x, int y, double value ) {
	char buffer[100];
	sprintf( buffer, "%g", value );
	draw_string( x, y, buffer );
}

int get_char() {
	int currentChar = getch();

	// Save the character to the transcript, if screen save is enabled. 
	if ( auto_save_screen ) {
		save_char( currentChar );
	}

	return currentChar;
}

int wait_char() {
	timeout( -1 );
	int result = getch();
	timeout( 0 );
	return result;
}

void get_screen_size_( int * width, int * height ) {
	*width = screen_width();
	*height = screen_height();
}

int screen_width( void ) {
	return override_screen == NULL ? getmaxx( stdscr ) : override_screen->width;
}

int screen_height( void ) {
	return override_screen == NULL ? getmaxy( stdscr ) : override_screen->height;
}

/**
 *	Gets the character at the designated location on the screen.
 *	This uses the override screen if it is non-NULL, or otherwise
 *	uses the current curses terminal.
 */

char get_screen_char( int x, int y ) {
	if ( override_screen == NULL ) {
		return mvinch( y, x ) & 0xff;
	}

	int w = override_screen->width;
	int h = override_screen->height;

	if ( x >= 0 && x < w && y >= 0 && y < h ) {
		char * scr = override_screen->buffer;
		return scr[x + y * w];
	}
	else {
		return 0;
	}
}

/**
*	Saves a screen shot to an automatically named local file.
*/

void save_screen( void ) {
	char * fileName = CAB202_SCREEN_NAME;

	FILE * f = fopen( fileName, "a" );

	if ( f == NULL ) return;

	int width = screen_width();
	int height = screen_height();

	fprintf( f, "Frame(%d,%d,%f)\n", width, height, get_current_time() );

	for ( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			char c = get_screen_char( x, y );

			// Previous version wrote out numeric coordinates and character codes.
			// Retained just in case we decide to revert.
			//
			// if ( c != ' ' ) {
			// 	fprintf( f, "%d,%d,%d\n", x, y, (int) c );
			// }

			fputc( c, f );
		}

		fputc( '\n', f );
	}

	fprintf( f, "EndFrame\n" );
	fclose( f );
}

/**
*	Saves the current character to an automatically named local file.
*/

void save_char( int charCode ) {
	char * fileName = CAB202_SCREEN_NAME;

	FILE * f = fopen( fileName, "a" );

	if ( f == NULL ) return;

	fprintf( f, "Char(%d,%f)\n", charCode, get_current_time() );
	fclose( f );
}

/**
*	This function is provided to support programatic emulation
*	of a resized terminal window.
*	Subsequent calls to screen_width() and screen_height() will
*	return the supplied values of width and height.
*/

void override_screen_size( int width, int height ) {
	if ( override_screen != NULL ) {
		free( override_screen->buffer );
		free( override_screen );
	}

	override_screen = calloc( 1, sizeof( Screen ) );
	override_screen->width = width;
	override_screen->height = height;
	override_screen->buffer = calloc( width * height, sizeof( char ) );
	memset( override_screen->buffer, ' ', width * height );
}

/**
*	This function is provided to support programmatic emulation
*	of a resized terminal window. It undoes the effects of
*	override_screen_size.
*	Subsequent calls to screen_width() and screen_height() will
*	return the width and height of the current terminal window,
*	respectively.
*/

void use_default_screen_size( void ) {
	if ( override_screen != NULL ) {
		free( override_screen->buffer );
		free( override_screen );
		override_screen = NULL;
	}
}

/**
*	Draws formatted text at the specified location.
*	Total length of text must be less than 1000 characters.
*/

void draw_formatted( int x, int y, const char * format, ... ) {
	va_list args;
	va_start( args, format );
	char buffer[1000];
	vsprintf( buffer, format, args );
	draw_string( x, y, buffer );
}