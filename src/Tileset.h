/*
 * Tileset.h
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include <map>
#include <string>

#include <ticpp/ticpp.h>

#ifndef TILESET_H_
#define TILESET_H_

#include "Map.h"
#include "Color.h"
#include "PropertyMapContainer.h"
#include "UserDataContainer.h"

namespace tiledpp {

typedef std::string (*ExternalTilesetSupplier)(std::string, std::string);

std::string supplyTilesetFromFile(std::string source, std::string mappath);

class Tileset;

class TilePropertyContainer : public PropertyMapContainer {
private:
	TilePropertyContainer(const PropertyMap& p) {
		properties = p;
	}
	
	friend class Tileset;
};

class Tileset : public PropertyMapContainer, public UserDataContainer {
private:
	Tileset(Map* owner);
	virtual ~Tileset();
	
	friend class Map;
	
	void load(ticpp::Element* tileset, std::string path, ExternalTilesetSupplier exts);
	void save(ticpp::Element* tileset);

public:
	unsigned long getFirstGId() { return firstGId; };
	std::string getName() { return name; };
	bool isExternal() { return external; };
	
	unsigned short getTileWidth() { return tileWidth; };
	unsigned short getTileHeight() { return tileHeight; };
	unsigned short getSpacing() { return spacing; };
	unsigned short getMargin() { return margin; };
	
	long getTileOffsetX() { return tileOffsetX; };
	long getTileOffsetY() { return tileOffsetY; };
	
	std::string getImageSource() { return imageSource; };
	bool hasImageTransparent() { return imageHasTransparent; };
	Color getImageTransparentColor() { return imageTransparentColor; };
	unsigned long getImageWidth() { return imageWidth; };
	unsigned long getImageHeight() { return imageHeight; };
	
	unsigned long getWidthTiles() { return widthTiles; };
	unsigned long getHeightTiles() { return heightTiles; };
	
	//void updateImageSize(unsigned long w, unsigned long h) { imageWidth = w; imageHeight = h; updateTilesSize(); };
	
	TilePropertyContainer tileProps(unsigned long id) {
		return TilePropertyContainer( tileProperties[id] );
	}
	
	Point getTileCoordsOnSheet(unsigned long gid);
	
	bool isTileGIdPartOfSheet(unsigned long gid) {
		return gid >= firstGId && gid < firstGId+widthTiles*heightTiles;
	}
	
private:
	void updateTilesSize() {
		//n = (-2m+s+w)/(s+tx) where n is amount of tiles, m is margin, s is spacing, w is width, tx is tile width
		widthTiles = (-2 * margin + spacing + imageWidth) / (spacing + tileWidth);
		heightTiles = (-2 * margin + spacing + imageHeight) / (spacing + tileHeight);
	};
	
	Map* owner;
	
	unsigned long firstGId;
	std::string name;
	bool external;
	std::string externalSource;

	unsigned short tileWidth;
	unsigned short tileHeight;
	unsigned short spacing;
	unsigned short margin;
	
	unsigned long widthTiles;
	unsigned long heightTiles;
	
	long tileOffsetX;
	long tileOffsetY;
	
	std::string imageSource;
	bool imageHasTransparent;
	Color imageTransparentColor;
	unsigned long imageWidth;
	unsigned long imageHeight;
	
	std::map<unsigned long, PropertyMap> tileProperties;
};

} /* namespace tiledpp */
#endif /* TILESET_H_ */
