/*
 * Color.cpp
 *
 *  Created on: 25 dec 2012
 *      Author: Raymond
 */

#include "Color.h"

#include <sstream>
#include <iomanip>
#include <cstdlib>

namespace tiledpp {

Color DecodeColorFromHex(const std::string color) {
	Color val;
	
	DecodeColorFromHex(color, &val);
	
	return val;
}

bool DecodeColorFromHex(const std::string str, Color* color) {
	std::string tmp = str;
	
	while(tmp[0] == ' ' || tmp[0] == '\t') tmp.erase(0,1);
	while(tmp[tmp.size()-1] == ' ' || tmp[tmp.size()-1] == '\t') tmp.erase(tmp.size()-1,1);
	
	if(tmp[0] == '#') tmp.erase(0,1);
	
	switch(tmp.size()) {
		case 3: {
			color->r = strtol(tmp.substr(0,1).c_str(), NULL, 16) * 0x11;
			color->g = strtol(tmp.substr(1,1).c_str(), NULL, 16) * 0x11;
			color->b = strtol(tmp.substr(2,1).c_str(), NULL, 16) * 0x11;
			color->a = 255;
			
			break;
		}
		case 4: {
			color->r = strtol(tmp.substr(0,1).c_str(), NULL, 16) * 0x11;
			color->g = strtol(tmp.substr(1,1).c_str(), NULL, 16) * 0x11;
			color->b = strtol(tmp.substr(2,1).c_str(), NULL, 16) * 0x11;
			color->a = strtol(tmp.substr(3,1).c_str(), NULL, 16) * 0x11;
			
			break;
		}
		case 6: {
			color->r = strtol(tmp.substr(0,2).c_str(), NULL, 16);
			color->g = strtol(tmp.substr(2,2).c_str(), NULL, 16);
			color->b = strtol(tmp.substr(4,2).c_str(), NULL, 16);
			color->a = 255;
			
			break;
		}
		case 8: {
			color->r = strtol(tmp.substr(0,2).c_str(), NULL, 16);
			color->g = strtol(tmp.substr(2,2).c_str(), NULL, 16);
			color->b = strtol(tmp.substr(4,2).c_str(), NULL, 16);
			color->a = strtol(tmp.substr(6,2).c_str(), NULL, 16);
			
			break;
		}
		default: {
			return false;
		}
	}
	
	return true;
}

std::string EncodeColorToHex(Color c) {
	std::ostringstream r;
	r << '#' << std::hex <<
			std::setfill('0') << std::setw(2) << (short)c.r <<
			std::setfill('0') << std::setw(2) << (short)c.g <<
			std::setfill('0') << std::setw(2) << (short)c.b;
	if(c.a != 255) r <<
			std::setfill('0') << std::setw(2) << (short)c.a;
	return r.str();
}

} //namespace tiledpp
