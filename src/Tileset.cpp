/*
 * Tileset.cpp
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include <iostream>
#include <fstream>

#include "Tileset.h"

#include "GidBitFlags.h"
#include "util.h"

namespace tiledpp {

std::string supplyTilesetFromFile(std::string source, std::string mappath) {
	std::ifstream in( ((mappath.length() > 0 ? mappath + "/" : "") + source).c_str() );
	std::string out;
	out.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	return out;
}

Tileset::Tileset(Map* owner):owner(owner),firstGId(0),name(""),external(false),externalSource(),tileWidth(0),tileHeight(0),
		spacing(0),margin(0),widthTiles(0),heightTiles(0),tileOffsetX(0),tileOffsetY(0),imageSource(""),imageHasTransparent(false),
		imageTransparentColor(),imageWidth(0),imageHeight(0) {
}

Tileset::~Tileset() {
}

void Tileset::load(ticpp::Element* tileset, std::string path, ExternalTilesetSupplier exts) {
	tileset->GetAttribute("firstgid", &firstGId);
	
	//std::cout << "Path is " << path << std::endl;
	
	ticpp::Document* externalDoc = NULL;
	
	if(tileset->HasAttribute("source")) {
		external = true;
		externalSource = tileset->GetAttribute("source");
		//This is where we load the referenced tileset file
		//After that, we set tileset to the new <tileset> root element in the file
		//and act using that for the rest of the file
		externalDoc = new ticpp::Document();
		externalDoc->Parse( exts( externalSource, path ) );
		tileset = externalDoc->FirstChildElement("tileset");
	}
	
	tileset->GetAttribute("name", &name);
	tileset->GetAttribute("tilewidth", &tileWidth);
	tileset->GetAttribute("tileheight", &tileHeight);
	tileset->GetAttribute("spacing", &spacing, false);
	tileset->GetAttribute("margin", &margin, false);
	
	properties = util::ParseProperties(tileset->FirstChildElement("properties", false));
	
	ticpp::Element* image = tileset->FirstChildElement("image");
	image->GetAttribute("source", &imageSource);
	if(image->HasAttribute("trans")) {
		imageHasTransparent = true;
		imageTransparentColor = DecodeColorFromHex(image->GetAttribute("trans"));
	}
	image->GetAttribute("width", &imageWidth);
	image->GetAttribute("height", &imageHeight);
	
	ticpp::Element* tileoffset = tileset->FirstChildElement("tileoffset", false);
	if(tileoffset) {
		tileoffset->GetAttribute("x", &tileOffsetX);
		tileoffset->GetAttribute("y", &tileOffsetY);
	}
	
	updateTilesSize();
	
	unsigned long id = 0;
	ticpp::Iterator<ticpp::Element> tileIter("tile");
	for(tileIter = tileIter.begin(tileset); tileIter != tileIter.end(); ++tileIter) {
		tileIter->GetAttribute("id", &id);
		tileProperties[id] = util::ParseProperties( tileIter->FirstChildElement("properties", false) );
	}
	
	if(externalDoc) delete externalDoc;
}

void Tileset::save(ticpp::Element* tileset) {
	tileset->SetAttribute("firstgid", firstGId);
	
	if(external) {
		//We keep things simple here, really
		tileset->SetAttribute("source", externalSource);
		return;
	}
	
	//*** Saving Tiled tileset properties
	tileset->SetAttribute("name", name);
	tileset->SetAttribute("tilewidth", tileWidth);
	tileset->SetAttribute("tileheight", tileHeight);
	if(spacing != 0) tileset->SetAttribute("spacing", spacing);
	if(margin != 0) tileset->SetAttribute("margin", margin);
	
	//*** Saving custom properties
	ticpp::Element* propertiesElem = new ticpp::Element("properties");
	util::SaveProperties(properties, propertiesElem);
	if(!propertiesElem->NoChildren()) tileset->LinkEndChild(propertiesElem);
	else delete propertiesElem;
	
	if(tileOffsetX != 0 || tileOffsetY != 0) {
		ticpp::Element* tileoffset = new ticpp::Element("tileoffset");
		tileoffset->SetAttribute("x", tileOffsetX);
		tileoffset->SetAttribute("y", tileOffsetY);
		tileset->LinkEndChild(tileoffset);
	}
	
	ticpp::Element* image = new ticpp::Element("image");
	image->SetAttribute("source", imageSource);
	if(imageHasTransparent) image->SetAttribute("trans", EncodeColorToHex(imageTransparentColor));
	image->SetAttribute("width", imageWidth);
	image->SetAttribute("height", imageHeight);
	tileset->LinkEndChild(image);
	
	for(std::map<unsigned long, PropertyMap>::iterator it = tileProperties.begin(); it != tileProperties.end(); ++it) {
		ticpp::Element* tile = new ticpp::Element("tile");
		tile->SetAttribute("id", it->first);
		
		ticpp::Element* propertiesElem = new ticpp::Element("properties");
		util::SaveProperties(it->second, propertiesElem);
		if(!propertiesElem->NoChildren()) tile->LinkEndChild(propertiesElem);
		else delete propertiesElem;
		
		tileset->LinkEndChild(tile);
	}
}

Point Tileset::getTileCoordsOnSheet(unsigned long gid) {
	gid &= TILE_GID_BITS;
	long id = gid - firstGId;
	Point p;
	if(id < 0 || (unsigned)id >= widthTiles*heightTiles) {
		p.x = p.y = -1;
		return p;
	}
	//x = m+n(tx+s), y = m+n(ty+s)
	p.x = margin + (id % widthTiles)*(tileWidth + spacing);
	p.y = margin + (id / widthTiles)*(tileHeight + spacing);
	return p;
}

} /* namespace tiledpp */
