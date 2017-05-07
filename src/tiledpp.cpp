/*
 * tiledpp.cpp
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include <iostream>
#include <fstream>

#include <ticpp/ticpp.h>

#include "tiledpp.h"

namespace tiledpp {

Map* load(std::string filename) {
	/*
	std::ifstream fin(filename.c_str());
	Map* map = load(fin);
	fin.close();
	//*/
	std::string path = util::GetPath(filename);
	std::cout << "filename: '" << filename << "'; path: '" << path << "'" << std::endl;
	
	Map* map = new Map();
	ticpp::Document doc(filename);
	doc.LoadFile();
	map->load(doc.FirstChildElement("map"), path, supplyTilesetFromFile);
	
	return map;
}

void save(Map* map, std::string filename) {
	ticpp::Document doc;
	doc.LinkEndChild( new ticpp::Declaration("1.0", "UTF-8", "") );
	ticpp::Element* mapElem = new ticpp::Element("map");
	doc.LinkEndChild( mapElem );
	map->save(mapElem);
	
	//doc.SaveFile(filename);
	TiXmlPrinter* p = new TiXmlPrinter();
	p->SetIndent(" ");
	doc.Accept(p);
	
	std::ofstream fout(filename.c_str());
	fout << p->CStr();
	fout.close();
}

void free(Map* map) {
	delete map;
}

}
