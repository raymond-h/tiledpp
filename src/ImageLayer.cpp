/*
 * ImageLayer.cpp
 *
 *  Created on: 29 jan 2013
 *      Author: Raymond
 */

#include "ImageLayer.h"

namespace tiledpp {

ImageLayer::ImageLayer(Map* owner):owner(owner),imageSource(),imageTransparentColor(),width(0),height(0) {
	imageTransparentColor.r = 160;
	imageTransparentColor.g = 160;
	imageTransparentColor.b = 164;
	imageTransparentColor.a = 255;
}

ImageLayer::~ImageLayer() {
}

void ImageLayer::load(ticpp::Element* imagelayer) {
	imagelayer->GetAttribute("name", &name);
	imagelayer->GetAttribute("width", &width, false);
	imagelayer->GetAttribute("height", &height, false);
	imagelayer->GetAttribute("opacity", &opacity, false);
	imagelayer->GetAttribute("visible", &visible, false);
	
	properties = util::ParseProperties(imagelayer->FirstChildElement("properties", false));
	
	ticpp::Element* image = imagelayer->FirstChildElement("image");
	image->GetAttribute("source", &imageSource);
	if(image->HasAttribute("trans")) {
		DecodeColorFromHex(image->GetAttribute("trans"), &imageTransparentColor);
	}
}

void ImageLayer::save(ticpp::Element* imagelayer) {
	imagelayer->SetAttribute("name", name);
	imagelayer->SetAttribute("width", width);
	imagelayer->SetAttribute("height", height);
	if(opacity != 1.0f) imagelayer->SetAttribute("opacity", opacity);
	if(!visible) imagelayer->SetAttribute("visible", visible);
	
	ticpp::Element* image = new ticpp::Element("image");
	image->SetAttribute("source", imageSource);
	image->SetAttribute("trans", EncodeColorToHex(imageTransparentColor));
	imagelayer->LinkEndChild(image);
	
	ticpp::Element* propertiesElem = new ticpp::Element("properties");
	util::SaveProperties(properties, propertiesElem);
	if(!propertiesElem->NoChildren()) imagelayer->LinkEndChild(propertiesElem);
	else delete propertiesElem;
}

} /* namespace tiledpp */
