/*
 * Layer.cpp
 *
 *  Created on: 8 dec 2012
 *      Author: Raymond
 */

#include <iostream>
#include <sstream>
#include <fstream>

#include <zlib.h>

#include "TileLayer.h"

#include "util.h"
#include "Base64.h"

namespace tiledpp {

TileLayer::TileLayer(Map* owner):owner(owner),width(),height(),
		encoding(ENCODING_UNKNOWN),compression(COMPRESSION_NONE),tileDataGids(NULL) {
}

TileLayer::~TileLayer() {
	delete [] tileDataGids;
}

void TileLayer::load(ticpp::Element* layer) {
	//Get basic attributes of the layer (name, width and height)
	layer->GetAttribute("name", &name);
	layer->GetAttribute("opacity", &opacity, false);
	layer->GetAttribute("visible", &visible, false);
	
	layer->GetAttribute("width", &width);
	layer->GetAttribute("height", &height);
	
	properties = util::ParseProperties(layer->FirstChildElement("properties", false));
	
	tileDataGids = new unsigned long[width*height];
	unsigned long long dataPosition = 0;
	
	ticpp::Element* data = layer->FirstChildElement("data");
	std::string encoding = data->GetAttribute("encoding"), compression = data->GetAttribute("compression");
	
	if(!encoding.length()) { //No encoding, XML format
		this->encoding = ENCODING_XML;
		
		ticpp::Iterator<ticpp::Element> tileIter("tile");
		for(tileIter = tileIter.begin(data); tileIter != tileIter.end(); ++tileIter) {
			tileIter->GetAttribute("gid", tileDataGids+dataPosition);
			++dataPosition;
		}
	}
	else if(encoding == "base64") { //Base64 encoding, GIds stored as unsigned 4 bytes (possibly zlib or gzip compressed)
		this->encoding = ENCODING_BASE64;
		
		std::string tileData = data->GetText();
		
		std::string result = base64::Decode(tileData);
		const char* tileGids = result.c_str();
		
		if(compression == "zlib" || compression == "gzip") { //zlib or gzip compression; TODO Add error checking
			this->compression = (compression == "zlib" ? COMPRESSION_ZLIB : COMPRESSION_GZIP);
			
			unsigned char* decompressedData = new unsigned char[width*height*4];
			unsigned long decompressedSize = width*height*4;
			
			z_stream zs;
			zs.zalloc = Z_NULL;
			zs.zfree = Z_NULL;
			zs.opaque = Z_NULL;
			
			inflateInit2(&zs, 32+MAX_WBITS); //32+MAX_WBITS makes it handle both zlib and gzip automatically
			
			zs.avail_in = result.size();
			zs.next_in = (unsigned char*)tileGids;
			
			zs.avail_out = decompressedSize;
			zs.next_out = decompressedData;
			
			inflate(&zs, Z_FINISH);
			
			inflateEnd(&zs);
			
			tileGids = (char*)decompressedData;
		}
		else if(compression.length()) {
			//"compression" attrib is right there, but not a supported value
			//Unsupported compression format yo
			this->compression = COMPRESSION_UNKNOWN;
		}
		
		//If "compression" attrib wasn't there, the data is already uncompressed and ready to use
		
		const unsigned long long tileAmount = width*height;
		for(; dataPosition < tileAmount; ++dataPosition) {
			tileDataGids[dataPosition] =
					tileGids[dataPosition*4] |
					tileGids[dataPosition*4 + 1] << 8 |
					tileGids[dataPosition*4 + 2] << 16 |
					tileGids[dataPosition*4 + 3] << 24;
		}
		
		delete [] tileGids;
	}
	else if(encoding == "csv") { //CSV encoding, stored as numbers separated by commas
		this->encoding = ENCODING_CSV;
		
		std::string csv = data->GetText();
		std::istringstream csv_in(csv);
		std::string tmp;
		
		while(csv_in) {
			if(!getline(csv_in, tmp, ',')) break;
			
			unsigned long gid = 0;
			std::istringstream tmp_in(tmp);
			tmp_in >> gid;
			tileDataGids[dataPosition++] = gid;
		}
	}
	
	/*
	unsigned long long bufSize = width*height;
	std::ofstream fout("test.txt");
	for(unsigned long long i = 0; i < bufSize; ++i) {
		fout << tileDataGids[i];
		if(i+1 < bufSize) fout << ',';
		if( (i+1) % width == 0 ) fout << std::endl;
	}
	//*/
}

void TileLayer::save(ticpp::Element* layer) {
	layer->SetAttribute("name", name);
	layer->SetAttribute("width", width);
	layer->SetAttribute("height", height);
	if(opacity != 1.0f) layer->SetAttribute("opacity", opacity);
	if(!visible) layer->SetAttribute("visible", visible);
	
	ticpp::Element* propertiesElem = new ticpp::Element("properties");
	util::SaveProperties(properties, propertiesElem);
	if(!propertiesElem->NoChildren()) layer->LinkEndChild(propertiesElem);
	else delete propertiesElem;
	
	ticpp::Element* data = new ticpp::Element("data");
	
	switch(encoding) {
		case ENCODING_XML: {
			//XML encoding is represented by leaving out the 'encoding' attribute,
			//so we don't bother setting any for <data>
			
			for(unsigned long long i = 0; i < width*height; ++i) {
				ticpp::Element* tile = new ticpp::Element("tile");
				tile->SetAttribute("gid", tileDataGids[i]);
				data->LinkEndChild(tile);
			}
			
			break;
		}
		
		case ENCODING_CSV: {
			data->SetAttribute("encoding", "csv");
			
			std::ostringstream sout;
			sout << '\n';
			for(unsigned long long i = 0; i < width*height; ++i) {
				sout << tileDataGids[i];
				if(i != width*height-1) {
					sout << ",";
				}
				if((i+1) % width == 0) sout << '\n';
			}
			data->SetText(sout.str());
			
			break;
		}
		
		case ENCODING_BASE64: {
			data->SetAttribute("encoding", "base64");
			
			unsigned char* tileGids = (unsigned char*)tileDataGids;
			unsigned long long dataLength = width*height*4;
			switch(compression) {
				case COMPRESSION_ZLIB: case COMPRESSION_GZIP: {
					data->SetAttribute("compression", compression == COMPRESSION_ZLIB ? "zlib" : "gzip");
					
					unsigned long compressedSize = compressBound(width*height*4);
					unsigned char* compressedData = new unsigned char[compressedSize];
					
					z_stream zs;
					zs.zalloc = Z_NULL;
					zs.zfree = Z_NULL;
					zs.opaque = Z_NULL;
					
					deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
							MAX_WBITS + ( compression == COMPRESSION_ZLIB ? 0 : 16 ),
							8, Z_DEFAULT_STRATEGY);
					
					zs.avail_in = width*height*4;
					zs.next_in = (unsigned char*)tileGids;
					
					zs.avail_out = compressedSize;
					zs.next_out = compressedData;
					
					deflate(&zs, Z_FINISH);
					
					tileGids = compressedData;
					dataLength = zs.total_out;
					
					deflateEnd(&zs);
					
					break;
				}
				
				default: {
					//Either there's no compression method or an unknown one set
					//so we just encode the gids directly
					
					break;
				}
			}
			
			//At this point, tileGids should point to data that is to be encoded to base64,
			//no matter if it's compressed or not, and dataLength contains its length
			std::string base64 = base64::Encode( std::string( (char*)tileGids, dataLength ) );
			
			data->SetText(base64);
						
			break;
		}
		
		default: {
			break;
		}
	}
	
	layer->LinkEndChild(data);
}

} /* namespace tiledpp */
