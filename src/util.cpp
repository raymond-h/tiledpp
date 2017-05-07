/*
 * util.cpp
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include <iostream>
#include <sstream>
#include <iomanip>

#include "util.h"

#include "Tileset.h"
#include "GidBitFlags.h"

namespace tiledpp {

namespace util {

PropertyMap ParseProperties(
		ticpp::Element* properties) {
	PropertyMap props;
	
	if(properties) {
		ticpp::Iterator<ticpp::Element> iter("property");
		for(iter = iter.begin(properties); iter != iter.end(); ++iter) {
			props[ iter->GetAttribute("name") ] = iter->GetAttribute("value");
		}
	}
	
	return props;
}

void SaveProperties(const PropertyMap& propMap, ticpp::Element* properties) {
	
	if(properties) {
		for(PropertyMap::const_iterator it = propMap.begin(); it != propMap.end(); ++it) {
			ticpp::Element* property = new ticpp::Element("property");
			property->SetAttribute("name", it->first);
			property->SetAttribute("value", it->second);
			properties->LinkEndChild(property);
		}
	}
}

Point MapPixelSize(Map* map) {
	Point s;
	switch(map->getOrientation()) {
		case ORIENTATION_ORTHOGONAL: {
			s.x = map->getWidth()*map->getTileWidth();
			s.y = map->getHeight()*map->getTileHeight();
			break;
		}
		case ORIENTATION_ISOMETRIC: {
			s.x = map->getTileWidth()+
					(map->getWidth()-1)*(map->getTileWidth()/2)+
					(map->getHeight()-1)*(map->getTileWidth()/2);
			s.y = map->getTileHeight()+
					(map->getWidth()-1)*(map->getTileHeight()/2)+
					(map->getHeight()-1)*(map->getTileHeight()/2);
			break;
		}
		default: {
			break; //Unsupported map orientation ???
		}
	}
	return s;
}

std::string GetPath(std::string filepath) {
	unsigned int slashPos = filepath.find_last_of("\\/");
	if(slashPos == std::string::npos) return "";
	return filepath.substr(0, slashPos);
}

}

} /* namespace tiledpp */
