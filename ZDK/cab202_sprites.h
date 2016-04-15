#ifndef __SIMPLE_SPRITE_H__
#define __SIMPLE_SPRITE_H__

#include <stdbool.h>

/* 
 * ------------------------------------------------------------
 *	File: cab202_sprites.h
 *	Author: Lawrence Buckingham
 *	Copyright: Queensland University of Technology
 *	Date: 16-Mar-2015, 19-Jul-2015, 29-Feb-2016.
 *
 *	A simple curses-based sprite library.
 * ------------------------------------------------------------
 */

/*
 *	Data structure used to manage and render moving sprites.
 *
 *	Members:
 *		x, y:	The location of the sprite, represented as double to allow
 *				fractional positioning.
 *
 *		width:	The width of the sprite. This must be less than or equal to 
 *				SS_WIDTH.
 *
 *		height:	The height of the sprite. This must be less than or equal to 
 *				SS_HEIGHT.
 *
 *		dx, dy: A pair of floating point values that will update the sprite location 
 *				each time the sprite moves forward.
 *
 *		is_visible: Current visibility of the sprite. TRUE == visible; false == invisible.
 *
 *		bitmap: an array of characters that represents the image. ' ' (space) is 
 *				treated as transparent.
 */

typedef struct sprite {
	int width;
	int height;
	double x, y, dx, dy;
	bool is_visible;
	char * bitmap;
} sprite_t;

/* 
 *	Data type to uniquely identify all registered sprites. 
 */

typedef sprite_t * sprite_id;

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

sprite_id sprite_create( double x, double y, int width, int height, char * bitmap );

/**
 *	Releases the memory resources being used by a sprite.
 */

void sprite_destroy( sprite_id sprite );

/*
 *	Draws the sprite image. The top left corner of the (rectangular)
 *	bitmap is drawn at the screen coordinate closest to the (x,y)
 *	position of the sprite.
 *
 *	Input:
 *	-	id: The ID of the sprite which is to be made visible.
 *
 *	Output:
 *	-	n/a
 */
void sprite_draw( sprite_id id );

/*
 *	Sets the sprites direction to a new value.
 */
void sprite_turn_to( sprite_id sprite, double dx, double dy );

/*
 *	Sets the sprites position to a new value.
 */
void sprite_move_to( sprite_id sprite, double x, double y );

/*
 *	Updates the internally stored direction. This is the step that is taken when 
 *	the sprite moves forward or backward.
 *
 *	The new direction is relative to the previsou direction. If the old direction is 0,0 then
 *	the new one will also be 0,0.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 *		degrees: The angle to turn, measured in degrees.
 */
void sprite_turn( sprite_id sprite, double degrees );

/*
*	Updates the sprite's location, adding the internally stored dx 
*	value to x and the internally stored dy to y.
*
*	Input:
*		sprite: The ID of a sprite.
*/
void sprite_step( sprite_id sprite );

/*
*	Updates the sprite's location, subtracting the internally stored dx 
*	value from x and the internally stored dy from y.
*
*	Input:
*		sprite: The ID of a sprite.
*/
void sprite_back( sprite_id sprite );

/*
*	Updates the sprite's location, adding dx to x and dy to y.
*
*	Input:
*		sprite: The ID of a sprite.
*		dx: The amount to move in the x-direction.
*		dy: the amount to move in the y-direction.
*/
void sprite_move( sprite_id sprite, double dx, double dy );

/*
 *	Gets the width of the sprite bitmap. 
 *
 *	Input:
 *		sprite: The ID of a sprite.
 */
int sprite_width( sprite_id sprite );

/*
 *	Gets the height of the sprite bitmap. 
 *
 *	Input:
 *		sprite: The ID of a sprite.
 */
int sprite_height( sprite_id sprite );

/*
 *	Gets the current x coordinate of the sprite.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 */
double sprite_x( sprite_id sprite );

/*
 *	Gets the height of the sprite bitmap.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 */
double sprite_y( sprite_id sprite );

/*
 *	Get the current step-size in the x-direction.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 */
double sprite_dx( sprite_id sprite );

/*
 *	Get the current step-size in the y-direction.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 */
double sprite_dy( sprite_id sprite );

/*
 *	Makes a sprite visible.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 */
void sprite_show( sprite_id sprite );

/*
 *	Makes a sprite invisible.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 */
void sprite_hide( sprite_id sprite );

/*
 *	Returns true if and ony if the designated sprite is visible.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 */
bool sprite_visible( sprite_id sprite );

/*
 *	Updates the pattern of characters displayed when the designated 
 *	sprite is rendered.
 *
 *	Input:
 *		sprite: The ID of a sprite.
 *
 *		image: a string containing the new "bitmap" to be displayed.
 */
void sprite_set_image( sprite_id sprite, char * image );

#endif
