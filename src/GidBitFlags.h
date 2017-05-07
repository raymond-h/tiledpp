/*
 * GidBitFlags.h
 *
 *  Created on: 22 feb 2013
 *      Author: Raymond
 */

#ifndef GIDBITFLAGS_H_
#define GIDBITFLAGS_H_

const unsigned long TILE_FLIP_HORIZONTAL = 1 << 31;
const unsigned long TILE_FLIP_VERTICAL = 1 << 30;
const unsigned long TILE_FLIP_DIAGONAL = 1 << 29;

const unsigned long TILE_FLIP_BITS = (TILE_FLIP_HORIZONTAL | TILE_FLIP_VERTICAL | TILE_FLIP_DIAGONAL);
const unsigned long TILE_GID_BITS = 0xFFFFFFFF ^ TILE_FLIP_BITS;

#define GID_BITS(x) (x & TILE_GID_BITS)
#define FLIP_BITS(x) (x & TILE_FLIP_BITS)

/**
 * Returns how many degrees rotation the flip bits (horizontal, vertical and diagonal) represent,
 * or a negative value if it's not a valid rotation. The amount of degrees is clockwise from the
 * original tile, which is 0 degrees.
 */
inline int flipBitsToDegrees(unsigned long gid) {
	switch(gid & TILE_FLIP_BITS) {
		case 0: return 0; //No flags at all = 0 degrees clockwise
		case (TILE_FLIP_HORIZONTAL | TILE_FLIP_DIAGONAL): return 90; //Horizontal + diagonal flip = 90 degrees clockwise
		case (TILE_FLIP_HORIZONTAL | TILE_FLIP_VERTICAL): return 180; //Horizontal + vertical flip = 180 degrees clockwise
		case (TILE_FLIP_VERTICAL | TILE_FLIP_DIAGONAL): return 270; //Vertical + diagonal flip = 270 degrees clockwise
		
		//The flip bits together makes some weird combination that is not precisely a rotation of the original tile
		//It could just be simply flipped in any one way; return -1 to show it's not a valid rotation
		default: return -1;
	}
}

/**
 * Returns the flip bits (horizontal, vertical and diagonal) that represents a rotation of
 * the specified amount of degrees clockwise, or a negative value if it's impossible to represent it
 * using flip bits, I.E. degrees is not a multiple of 90. Anything below 0 or above 360
 * will be wrapped around. The return value is suitable for ORing together with a global tile ID without flip bits,
 * for instance:
 * 
 * 		unsigned long gid = 55 | degreesToFlipBits(90)
 * 		
 * 	to get the tile with global tile ID of 55 rotated 90 degrees clockwise.
 */
inline unsigned long degreesToFlipBits(int degrees) {
	switch(degrees % 360) {
		case 0: return 0; //0 degrees = no flags are set at all
		case 90: return (TILE_FLIP_HORIZONTAL | TILE_FLIP_DIAGONAL); //90 degrees clockwise = horizontal + diagonal flip
		case 180: return (TILE_FLIP_HORIZONTAL | TILE_FLIP_VERTICAL); //180 degrees clockwise = horizontal + vertical flip
		case 270: return (TILE_FLIP_VERTICAL | TILE_FLIP_DIAGONAL); //270 degrees clockwise = vertical + diagonal flip
		
		//The angle given is not a multiple of 90, which means
		//it can't be represented using flip bits
		default: return -1;
	}
}

//This is based off of libtiled's own code for rotating tiles
static const char rotateMasks[] = { 5, 4, 1, 0, 7, 6, 3, 2 };

inline unsigned long rotate(unsigned long gid, int degrees) {
	degrees %= 360;
	int turns = degrees / 4;
	
	for(int i = 0; i < turns; i++) {
		gid = GID_BITS(gid) | ( rotateMasks[FLIP_BITS(gid) >> 29] << 29);
	}
	
	return gid;
}

#endif /* GIDBITFLAGS_H_ */
