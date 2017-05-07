/*
 * base64.cpp
 *
 *  Created on: 21 dec 2012
 *      Author: Raymond
 */

#include <iostream>
#include <iomanip>

#include "Base64.h"

namespace tiledpp {
namespace base64 {

size_t DecodedBufferSize(size_t base64_length) {
	return (base64_length / 4) * 3 + 1;
}

size_t EncodedBufferSize(size_t data_length) {
	return (data_length / 3) * 4 + 1;
}

char DecodeValue(char c) {
	if('A' <= c && c <= 'Z') return c-'A';
	if('a' <= c && c <= 'z') return c-'a'+26;
	if('0' <= c && c <= '9') return c-'0'+52;
	if(c == '+') return 62;
	if(c == '/') return 63;
	if(c == '=') return 64;
	return -1;
}

char EncodeValue(char c) {
	if(0 <= c && c <= 25) return c+'A';
	if(26 <= c && c <= 51) return c-26+'a';
	if(52 <= c && c <= 61) return c-52+'0';
	if(c == 62) return '+';
	if(c == 63) return '/';
	if(c == 64) return '=';
	return -1;
}

std::string Decode(std::string input) {
	std::string output;
	size_t input_size = input.size();
	output.reserve( DecodedBufferSize(input_size) );
	
	std::string fixed_input;
	for(size_t i = 0; i < input_size; i++) {
		char c = DecodeValue(input[i]);
		if(c >= 0) fixed_input += c;
	}
	
	input_size = fixed_input.size();
	unsigned long decoded = 0;
	for(size_t i = 0; i < input_size; i+=4) {
		if(fixed_input[i+2] == 64) { //Two padding
			decoded = fixed_input[i] << 18 | fixed_input[i+1] << 12;
			output.append(1, (decoded & 0xFF0000) >> 16);
		}
		else if(fixed_input[i+3] == 64) { //One padding
			decoded = fixed_input[i] << 18 | fixed_input[i+1] << 12 | fixed_input[i+2] << 6;
			output.append(1, (decoded & 0xFF0000) >> 16);
			output.append(1, (decoded & 0x00FF00) >> 8);
		}
		else { //No padding
			decoded = fixed_input[i] << 18 | fixed_input[i+1] << 12 | fixed_input[i+2] << 6 | fixed_input[i+3];
			output.append(1, (decoded & 0xFF0000) >> 16);
			output.append(1, (decoded & 0x00FF00) >> 8);
			output.append(1, (decoded & 0x0000FF));
		}
	}
	
	return output;
}

std::string Encode(std::string data) {
	std::string base64_string;
	size_t input_size = data.size();
	base64_string.reserve( EncodedBufferSize(input_size) );
	
	int padding = 3 - (input_size % 3);
	if(padding == 3) padding = 0;
	
	for(size_t i = 0; i < input_size;) {
		unsigned char	b0 = data[i++],
						b1 = i < input_size ? data[i++] : 0,
						b2 = i < input_size ? data[i++] : 0;
				
		char c[4];
		unsigned long tmp = (b0 << 16) | (b1 << 8) | (b2);
		c[3] = (i >= input_size && padding >= 1) ? '=' : EncodeValue( tmp & 0x3F );
		c[2] = (i >= input_size && padding == 2) ? '=' : EncodeValue( (tmp >> 6) & 0x3F );
		c[1] = EncodeValue( (tmp >> 12) & 0x3F );
		c[0] = EncodeValue( (tmp >> 18) & 0x3F );
		
		base64_string.append(c, 4);
	}
	
	return base64_string;
}

}
}
