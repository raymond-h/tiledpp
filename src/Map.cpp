/*
 * Map.cpp
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include "Map.h"

#include <iostream>

#include "Tileset.h"
#include "TileLayer.h"
#include "ObjectLayer.h"
#include "ImageLayer.h"
#include "GidBitFlags.h"
#include "util.h"

#include <fstream>

#include <ticpp/ticpp.h>

namespace tiledpp {

bool compareTilesets(tiledpp::Tileset* lhs, tiledpp::Tileset* rhs) {
	return (lhs->getFirstGId() > rhs->getFirstGId());
}

Map::Map():formatVersion("1.0"),orientation(ORIENTATION_NONE),width(0),height(0),tileWidth(0),
		tileHeight(0),backgroundColor(),tilesets(compareTilesets),layers(),tilesetCache() {
	backgroundColor.r = 128;
	backgroundColor.g = 128;
	backgroundColor.b = 128;
	backgroundColor.a = 255;
}

Map::~Map() {
	for(TilesetSet::iterator it = tilesets.begin(); it != tilesets.end(); ++it) {
		delete *it;
	}
	
	for(LayerVector::iterator it = layers.begin(); it != layers.end(); ++it) {
		delete *it;
	}
}

void Map::load(ticpp::Element* map, std::string path, ExternalTilesetSupplier exts) {
	//*** Parsing Tiled map properties
	map->GetAttribute("version", &formatVersion);
	map->GetAttribute("width", &width);
	map->GetAttribute("height", &height);
	map->GetAttribute("tilewidth", &tileWidth);
	map->GetAttribute("tileheight", &tileHeight);
	DecodeColorFromHex(map->GetAttribute("backgroundcolor"), &backgroundColor);
	
	std::string orientation = map->GetAttribute("orientation");
	if(orientation == "orthogonal") this->orientation = ORIENTATION_ORTHOGONAL;
	else if(orientation == "isometric") this->orientation = ORIENTATION_ISOMETRIC;
	
	//*** Parsing custom properties
	properties = util::ParseProperties(map->FirstChildElement("properties", false));
	
	//*** Parsing tilesets
	ticpp::Iterator<ticpp::Element> tilesetIter("tileset");
	for(tilesetIter = tilesetIter.begin(map); tilesetIter != tilesetIter.end(); ++tilesetIter) {
		Tileset* tileset = new Tileset(this);
		tileset->load(&(*tilesetIter), path, exts);
		tilesets.insert(tileset);
	}
	
	//*** Parsing layers
	ticpp::Iterator<ticpp::Element> layerIter;
	for(layerIter = layerIter.begin(map); layerIter != layerIter.end(); ++layerIter) {
		if(layerIter->Value() == "layer") { //Tile Layer
			TileLayer* layer = new TileLayer(this);
			layer->load(&(*layerIter));
			layers.push_back(layer);
		}
		else if(layerIter->Value() == "objectgroup") { //Object Layer
			ObjectLayer* layer = new ObjectLayer(this);
			layer->load(&(*layerIter));
			layers.push_back(layer);
		}
		else if(layerIter->Value() == "imagelayer") { //Image Layer
			ImageLayer* layer = new ImageLayer(this);
			layer->load(&(*layerIter));
			layers.push_back(layer);
		}
	}
}

void Map::save(ticpp::Element* map) {
	//*** Saving Tiled map properties
	map->SetAttribute("version", formatVersion);
	map->SetAttribute("width", width);
	map->SetAttribute("height", height);
	map->SetAttribute("tilewidth", tileWidth);
	map->SetAttribute("tileheight", tileHeight);
	map->SetAttribute("backgroundcolor", EncodeColorToHex(backgroundColor));
	
	std::string orientationStr;
	switch(orientation) {
		case ORIENTATION_ORTHOGONAL: orientationStr = "orthogonal"; break;
		case ORIENTATION_ISOMETRIC: orientationStr = "isometric"; break;
		default: orientationStr = ""; break;
	}
	map->SetAttribute("orientation", orientationStr);
	
	//*** Saving custom properties
	ticpp::Element* propertiesElem = new ticpp::Element("properties");
	util::SaveProperties(properties, propertiesElem);
	if(!propertiesElem->NoChildren()) map->LinkEndChild(propertiesElem);
	else delete propertiesElem;
	
	//TODO Save tilesets
	for(TilesetSet::reverse_iterator it = tilesets.rbegin(); it != tilesets.rend(); ++it) {
		Tileset* tileset = *it;
		ticpp::Element* tilesetElement = new ticpp::Element("tileset");
		tileset->save(tilesetElement);
		map->LinkEndChild(tilesetElement);
	}
	
	//*** Saving layers
	for(LayerVector::iterator it = layers.begin(); it != layers.end(); ++it) {
		Layer* layer = *it;
		ticpp::Element* layerElement = new ticpp::Element();
		switch(layer->getLayerType()) {
			case LAYER_TILE: {
				layerElement->SetValue("layer");
				break;
			}
			case LAYER_OBJECT: {
				layerElement->SetValue("objectgroup");
				break;
			}
			case LAYER_IMAGE: {
				layerElement->SetValue("imagelayer");
				break;
			}
			default: {
				continue; //If the layer is of unknown type, we just skip it - we have no idea how to save it anyway
			}
		}
		layer->save(layerElement);
		map->LinkEndChild(layerElement);
	}
}

Tileset* Map::findTileset(unsigned long gid) {
	gid &= TILE_GID_BITS;
	if(tilesetCache.count(gid) > 0) return tilesetCache[gid];
	
	if(gid == 0) return NULL;
	if(tilesets.size() == 1) return *tilesets.begin();
	
	for(TilesetSet::iterator it = tilesets.begin(); it != tilesets.end(); ++it) {
		if((*it)->getFirstGId() <= gid) {
			tilesetCache[gid] = *it;
			return *it;
		}
	}
	
	return NULL; //Will not ever be reached because of gid == 0 check
}

LayerVector Map::getLayers(LayerType layerType) {
	LayerVector result;
	for(LayerVector::iterator it = layers.begin(); it != layers.end(); ++it) {
		if( (*it)->getLayerType() == layerType ) result.push_back(*it);
	}
	return result;
}

LayerVector Map::getLayersByName(std::string name) {
	LayerVector result;
	for(LayerVector::iterator it = layers.begin(); it != layers.end(); ++it) {
		if( (*it)->name == name ) result.push_back(*it);
	}
	return result;
}

LayerVector Map::getLayersByName(std::string name, LayerType layerType) {
	LayerVector result;
	for(LayerVector::iterator it = layers.begin(); it != layers.end(); ++it) {
		if( (*it)->name == name && (*it)->getLayerType() == layerType ) result.push_back(*it);
	}
	return result;
}

Layer* Map::getSingleLayerByName(std::string name) {
	for(LayerVector::iterator it = layers.begin(); it != layers.end(); ++it) {
		if( (*it)->name == name ) return *it;
	}
	return NULL;
}

Layer* Map::getSingleLayerByName(std::string name, LayerType layerType) {
	for(LayerVector::iterator it = layers.begin(); it != layers.end(); ++it) {
		if( (*it)->name == name && (*it)->getLayerType() == layerType ) return *it;
	}
	return NULL;
}

Layer* Map::getSingleLayer(LayerType layerType) {
	for(LayerVector::iterator it = layers.begin(); it != layers.end(); ++it) {
		if( (*it)->getLayerType() == layerType ) return *it;
	}
	return NULL;
}

void Map::precacheTilesetRetrieval() {
	using namespace std;
	
	for(TilesetSet::reverse_iterator it = tilesets.rbegin(); it != tilesets.rend(); ++it) {
		Tileset* tileset = *it;

		unsigned long tilesEnd = tileset->getWidthTiles() * tileset->getHeightTiles() + tileset->getFirstGId();
		for(unsigned long i = tileset->getFirstGId(); i < tilesEnd; ++i) {
			tilesetCache[i] = tileset;
		}
	}
}

} /* namespace tiledpp */
