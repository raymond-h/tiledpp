/*
 * util.h
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include <map>
#include <string>

#include <ticpp/ticpp.h>

#ifndef UTIL_H_
#define UTIL_H_

#include "Color.h"
#include "Point.h"
#include "PropertyMapContainer.h"

namespace tiledpp {

class Map;

namespace util {

PropertyMap ParseProperties(ticpp::Element* properties);
void SaveProperties(const PropertyMap& propMap, ticpp::Element* properties);

Point MapPixelSize(Map* map);

std::string GetPath(std::string filepath);

}

} /* namespace tiledpp */
#endif /* UTIL_H_ */
