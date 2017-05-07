/*
 * tiledpp.h
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include <string>
#include <iostream>

#ifndef TILEDPP_H_
#define TILEDPP_H_

#include "base64.h"
#include "Map.h"
#include "Tileset.h"
#include "Layer.h"
#include "TileLayer.h"
#include "ObjectLayer.h"
#include "ImageLayer.h"
#include "Color.h"
#include "Point.h"
#include "GidBitFlags.h"

namespace tiledpp {

Map* load(std::string filename);

void save(Map* map, std::string filename);

//Map* load(std::istream& in);

void free(Map* map);

}

#endif /* TILEDPP_H_ */
