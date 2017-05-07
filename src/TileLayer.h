/*
 * Layer.h
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include <map>
#include <string>

#include <ticpp/ticpp.h>

#ifndef LAYER_H_
#define LAYER_H_

#include "Map.h"
#include "Layer.h"
#include "GidBitFlags.h"

namespace tiledpp {

enum TileEncoding { ENCODING_UNKNOWN, ENCODING_XML, ENCODING_BASE64, ENCODING_CSV };
enum TileCompression { COMPRESSION_NONE, COMPRESSION_UNKNOWN, COMPRESSION_ZLIB, COMPRESSION_GZIP };

class TileLayer : public Layer {
private:
	TileLayer(Map* owner);
	virtual ~TileLayer();
	
	friend class Map;
	
	void load(ticpp::Element* layer);
	void save(ticpp::Element* layer);
	
public:
	LayerType getLayerType() { return LAYER_TILE; };
	
	unsigned long getWidth() { return width; };
	unsigned long getHeight() { return height; };
	
	TileEncoding getEncoding() { return encoding; };
	TileCompression getCompression() { return compression; };
	
	unsigned long* getTileData() { return tileDataGids; };
	
	Map* getOwner() { return owner; };

private:
	Map* owner;
	
	unsigned long width;
	unsigned long height;
	
	TileEncoding encoding;
	TileCompression compression;
	
	unsigned long* tileDataGids;

};

} /* namespace tiledpp */
#endif /* LAYER_H_ */
