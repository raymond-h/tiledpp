/*
 * Color.h
 *
 *  Created on: 19 dec 2012
 *      Author: Raymond
 */

#ifndef COLOR_H_
#define COLOR_H_

#include <string>

namespace tiledpp {

typedef unsigned char color_value;

struct Color {
	color_value r;
	color_value g;
	color_value b;
	color_value a; //*** Unused by Tiled itself ***
};

Color DecodeColorFromHex(const std::string color);
bool DecodeColorFromHex(const std::string str, Color* color);
std::string EncodeColorToHex(Color color);

}

#endif /* COLOR_H_ */
