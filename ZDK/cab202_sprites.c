#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cab202_graphics.h"
#include "cab202_sprites.h"
#include "curses.h"


/*
*	Initialise a sprite.
*
*	Input:
*		x, y:	The initial location of the sprite. Floating point values are
*				used to allow sub-pixel resolution (in a chunky display),
*				making it easier to achieve "smooth" movement.
*
*		width, height: The dimensions of the sprite.
*
*		bitmap:	The characters to show. A dynamically allocated copy of the bitmap
*				is created. If you alter the bitmap pointer later, care must be taken
*				to ensure that memory resources are preserved, restored and deallocated
*				correctly.
*
*	Output:
*		Returns the address of an initialised sprite object.
*/

sprite_id sprite_create( double x, double y, int width, int height, char * image ) {
	assert( width > 0 );
	assert( height > 0 );
	assert( image != NULL );

	sprite_id sprite = malloc( sizeof( sprite_t ) );

	if ( sprite != NULL ) {
		sprite->is_visible = TRUE;
		sprite->x = x;
		sprite->y = y;
		sprite->width = width;
		sprite->height = height;
		sprite->dx = 0;
		sprite->dy = 0;
		sprite->bitmap = image;
	}

	return sprite;
}

/**
*	Releases the memory resources being used by a sprite.
*/

void sprite_destroy( sprite_id sprite ) {
	if ( sprite != NULL ) {
		free( sprite );
	}
}


/*
 *	Draws the image of a visible sprite.
 *
 *	Input:
 *		id: The ID of the sprite which is to be made visible.
 *
 *	Output:
 *		n/a
 */

void sprite_draw( sprite_id sprite ) {
	assert( sprite != NULL );

	if ( !sprite->is_visible ) return;

	int x = (int)round( sprite->x );
	int y = (int)round( sprite->y );
	int offset = 0;

	for ( int row = 0; row < sprite->height; row++ ) {
		for ( int col = 0; col < sprite->width; col++ ) {
			char ch = sprite->bitmap[offset++] & 0xff;

			if ( ch != ' ' ) {
				draw_char( x + col, y + row, ch );
			}
		}
	}
}


/*
 *	sprite_turn:
 *
 *	Sets the internally stored direction. That is, the step that is taken when
 *	the sprite moves forward or backward.
 *
 *	The new direction is relative to the old one. If the old direction is 0,0 then
 *	the new one will also be 0,0.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 *		degrees: The angle to turn.
 */

void sprite_turn( sprite_id sprite, double degrees ) {
	double radians = degrees * M_PI / 180;
	double s = sin( radians );
	double c = cos( radians );
	double dx = s * sprite->dx + c * sprite->dy;
	double dy = -c * sprite->dx + s * sprite->dy;
	sprite->dx = dx;
	sprite->dy = dy;
}


/*
*	Sets the sprites direction to a new value.
*/
void sprite_turn_to( sprite_id sprite, double dx, double dy ) {
	assert( sprite != NULL );
	sprite->dx = dx;
	sprite->dy = dy;
}

/*
*	Moves the designated sprite to a new absolute location.
*/
void sprite_move_to( sprite_id sprite, double x, double y ) {
	assert( sprite != NULL );
	sprite->x = x;
	sprite->y = y;
}

/*
*	Updates the sprite's location, adding the internally stored dx
*	value to x and the internally stored dy to y.
*
*	Input:
*		sprite: The ID of a sprite.
*/
void sprite_step( sprite_id sprite ) {
	assert( sprite != NULL );
	sprite->x += sprite->dx;
	sprite->y += sprite->dy;
}

/*
*	Updates the sprite's location, subtracting the internally stored dx
*	value from x and the internally stored dy from y.
*
*	Input:
*		sprite: The ID of a sprite.
*/
void sprite_back( sprite_id sprite ) {
	assert( sprite != NULL );
	sprite->x -= sprite->dx;
	sprite->y -= sprite->dy;
}

/*
*	Updates the sprite's location, adding dx to x and dy to y.
*
*	Input:
*		sprite: The ID of a sprite.
*		dx: The amount to move in the x-direction.
*		dy: the amount to move in the y-direction.
*/
void sprite_move( sprite_id sprite, double dx, double dy ) {
	assert( sprite != NULL );
	sprite->x += dx;
	sprite->y += dy;
}

/*
*	Gets the width of the sprite bitmap.
*
*	Input:
*		sprite: The ID of a sprite.
*/
int sprite_width( sprite_id sprite ) {
	assert( sprite != NULL );
	return sprite->width;
}

/*
*	Gets the height of the sprite bitmap.
*
*	Input:
*		sprite: The ID of a sprite.
*/
int sprite_height( sprite_id sprite ) {
	assert( sprite != NULL );
	return sprite->height;
}

/*
*	Gets the current x coordinate of the sprite.
*
*	Input:
*		sprite: The ID of a sprite.
*/
double sprite_x( sprite_id sprite ) {
	assert( sprite != NULL );
	return sprite->x;
}

/*
*	Gets the height of the sprite bitmap.
*
*	Input:
*		sprite: The ID of a sprite.
*/
double sprite_y( sprite_id sprite ) {
	assert( sprite != NULL );
	return sprite->y;
}

/*
*	Get the current step-size in the x-direction.
*
*	Input:
*		sprite: The ID of a sprite.
*/
double sprite_dx( sprite_id sprite ) {
	assert( sprite != NULL );
	return sprite->dx;
}

/*
*	Get the current step-size in the y-direction.
*
*	Input:
*		sprite: The ID of a sprite.
*/
double sprite_dy( sprite_id sprite ) {
	assert( sprite != NULL );
	return sprite->dy;
}

/*
*	Makes a sprite visible.
*
*	Input:
*		sprite: The ID of a sprite.
*/
void sprite_show( sprite_id sprite ) {
	assert( sprite != NULL );
	sprite->is_visible = true;
}

/*
*	Makes a sprite invisible.
*
*	Input:
*		sprite: The ID of a sprite.
*/
void sprite_hide( sprite_id sprite ) {
	assert( sprite != NULL );
	sprite->is_visible = false;
}

/*
*	Returns true if and ony if the designated sprite is visible.
*
*	Input:
*		sprite: The ID of a sprite.
*/
bool sprite_visible( sprite_id sprite ) {
	assert( sprite != NULL );
	return sprite->is_visible;
}

/*
*	Updates the pattern of characters displayed when the designated
*	sprite is rendered.
*
*	Input:
*		sprite: The ID of a sprite.
*
*		image: a string containing the new "bitmap" to be displayed.
*/
void sprite_set_image( sprite_id sprite, char * image ) {
	assert( sprite != NULL );
	assert( image != NULL );
	sprite->bitmap = image;
}
