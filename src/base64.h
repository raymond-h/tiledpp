/*
 * base64.h
 *
 *  Created on: 21 dec 2012
 *      Author: Raymond
 */

#ifndef BASE64_H_
#define BASE64_H_

#include <string>

namespace tiledpp {
namespace base64 {

size_t DecodedBufferSize(size_t);

size_t EncodedBufferSize(size_t);

std::string Decode(std::string input);

std::string Encode(std::string data);

}
}

#endif /* BASE64_H_ */
