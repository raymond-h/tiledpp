/*
 * ImageLayer.h
 *
 *  Created on: 29 jan 2013
 *      Author: Raymond
 */

#include <string>

#ifndef IMAGELAYER_H_
#define IMAGELAYER_H_

#include "Map.h"
#include "Layer.h"

namespace tiledpp {

class ImageLayer : public Layer {
private:
	ImageLayer(Map* owner);
	virtual ~ImageLayer();
	
	friend class Map;
	
	void load(ticpp::Element* imagelayer);
	void save(ticpp::Element* imagelayer);
	
public:
	LayerType getLayerType() { return LAYER_IMAGE; };
	
	std::string getImageSource() { return imageSource; };
	Color getImageTransparentColor() { return imageTransparentColor; };
	
	Map* getOwner() { return owner; };
	
private:
	Map* owner;
	
	std::string imageSource;
	Color imageTransparentColor;
	
	int width;
	int height;
};

} /* namespace tiledpp */
#endif /* IMAGELAYER_H_ */
