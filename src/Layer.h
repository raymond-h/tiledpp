/*
 * ILayer.h
 *
 *  Created on: 19 dec 2012
 *      Author: Raymond
 */

#include <string>

#ifndef ILAYER_H_
#define ILAYER_H_

#include "PropertyMapContainer.h"
#include "UserDataContainer.h"

namespace tiledpp {

enum LayerType { LAYER_UNKNOWN, LAYER_TILE, LAYER_OBJECT, LAYER_IMAGE };

class Layer : public PropertyMapContainer, public UserDataContainer {
public:
	Layer():name(""),opacity(1.0),visible(true) {};
	virtual ~Layer() {};

	virtual LayerType getLayerType() = 0;
	
	friend class Map;
	
	std::string getName() { return name; };
	
	float getOpacity() { return opacity; };
	bool isVisible() { return visible; };
	
protected:
	std::string name;
	
	float opacity;
	bool visible;
	
private:
	virtual void load(ticpp::Element* element) = 0;
	virtual void save(ticpp::Element* element) = 0;
};

} /* namespace tiledpp */
#endif /* ILAYER_H_ */
