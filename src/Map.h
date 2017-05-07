/*
 * Map.h
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <ticpp/ticpp.h>

#ifndef MAP_H_
#define MAP_H_

#include "Layer.h"
#include "util.h"
#include "PropertyMapContainer.h"
#include "UserDataContainer.h"

namespace tiledpp {

typedef std::string (*ExternalTilesetSupplier)(std::string, std::string);

class Tileset;
class TileLayer;

typedef std::set<Tileset*, bool (*)(tiledpp::Tileset*, tiledpp::Tileset*)> TilesetSet;
typedef std::vector<Layer*> LayerVector;

typedef std::map<unsigned long, Tileset*> TilesetMap;

enum MapOrientation { ORIENTATION_NONE, ORIENTATION_ORTHOGONAL, ORIENTATION_ISOMETRIC };

class Map : public PropertyMapContainer, public UserDataContainer {
private:
	Map();
	~Map();
	
public:
	std::string getFormatVersion() { return formatVersion; }
	MapOrientation getOrientation() { return orientation; };
	
	long getWidth() { return width; };
	long getHeight() { return height; };
	long getTileWidth() { return tileWidth; };
	long getTileHeight() { return tileHeight; };
	
	Color getBackgroundColor() { return backgroundColor; };
	
	TilesetSet getTilesets() { return tilesets; };
	
	Tileset* findTileset(unsigned long gid);
	
	LayerVector getLayers() { return layers; };
	
	LayerVector getLayers(LayerType layerType);
	LayerVector getLayersByName(std::string name);
	LayerVector getLayersByName(std::string name, LayerType layerType);
	
	Layer* getSingleLayer(LayerType layerType);
	Layer* getSingleLayerByName(std::string name);
	Layer* getSingleLayerByName(std::string name, LayerType layerType);
	
	void clearTilesetRetrievalCache() { tilesetCache.clear(); };
	
	void precacheTilesetRetrieval();
	
	friend Map* load(std::string);
	friend Map* load(std::istream&);
	friend void save(Map*, std::string);
	friend void free(Map*);
	
	friend class Tileset;
	friend class Layer;
	
private:
	void load(ticpp::Element* map, std::string path, ExternalTilesetSupplier exts);
	
	void save(ticpp::Element* map);
	
	std::string formatVersion;
	
	MapOrientation orientation;
	
	long width;
	long height;
	
	short tileWidth;
	short tileHeight;
	
	Color backgroundColor;
	
	TilesetSet tilesets;
	LayerVector layers;
	
	TilesetMap tilesetCache;
};

} /* namespace tiledpp */
#endif /* MAP_H_ */
