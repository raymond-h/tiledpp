/*
 * ObjectLayer.cpp
 *
 *  Created on: 19 dec 2012
 *      Author: Raymond
 */

#include <sstream>

#include "ObjectLayer.h"
#include "util.h"

namespace tiledpp {

ObjectLayer::ObjectLayer(Map* owner):owner(owner),color(),objects() {
}

ObjectLayer::~ObjectLayer() {
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		delete *it;
	}
}

void ObjectLayer::load(ticpp::Element* objectgroup) {
	objectgroup->GetAttribute("name", &name);
	DecodeColorFromHex(objectgroup->GetAttribute("color"), &color);
	objectgroup->GetAttribute("opacity", &opacity, false);
	objectgroup->GetAttribute("visible", &visible, false);
	
	properties = util::ParseProperties(objectgroup->FirstChildElement("properties", false));
	
	ticpp::Iterator<ticpp::Element> objectIter("object");
	for(objectIter = objectIter.begin(objectgroup); objectIter != objectIter.end(); ++objectIter) {
		Object* object = NULL;
		
		if(objectIter->HasAttribute("gid")) {
			//Tile object
			TileObject* tile = new TileObject();
			objectIter->GetAttribute("x", &(tile->x));
			objectIter->GetAttribute("y", &(tile->y));
			objectIter->GetAttribute("gid", &(tile->gid));
			object = tile;
		}
		else {
			if(objectIter->FirstChildElement("polygon", false)) {
				//Polygon object
				PolygonObject* poly = new PolygonObject();
				objectIter->GetAttribute("x", &(poly->x));
				objectIter->GetAttribute("y", &(poly->y));
				
				ticpp::Element* polygon = objectIter->FirstChildElement("polygon", false);
				std::istringstream in(polygon->GetAttribute("points"));
				std::string tmp;
				
				while( in >> tmp ) {
					Point p;
					int cp = tmp.find(',');
					p.x = atol( tmp.substr(0,cp).c_str() );
					p.y = atol( tmp.substr(cp+1).c_str() );
					poly->points.push_back(p);
				}
				
				object = poly;
			}
			else if(objectIter->FirstChildElement("polyline", false)) {
				//Polyline object
				PolylineObject* poly = new PolylineObject();
				objectIter->GetAttribute("x", &(poly->x));
				objectIter->GetAttribute("y", &(poly->y));
				
				ticpp::Element* polygon = objectIter->FirstChildElement("polyline", false);
				std::istringstream in(polygon->GetAttribute("points"));
				std::string tmp;
				
				while( (in >> tmp) ) {
					Point p;
					int cp = tmp.find(',');
					p.x = atol( tmp.substr(0,cp).c_str() );
					p.y = atol( tmp.substr(cp+1).c_str() );
					poly->points.push_back(p);
				}
				
				object = poly;
			}
			
			else if(objectIter->FirstChildElement("ellipse", false)) { //Ellipse object
				//Rectangle object
				EllipseObject* ellipse = new EllipseObject();
				objectIter->GetAttribute("x", &(ellipse->x));
				objectIter->GetAttribute("y", &(ellipse->y));
				objectIter->GetAttribute("width", &(ellipse->w), false);
				objectIter->GetAttribute("height", &(ellipse->h), false);
				object = ellipse;
			}
			
			else { //Rectangle object
				RectangleObject* rect = new RectangleObject();
				objectIter->GetAttribute("x", &(rect->x));
				objectIter->GetAttribute("y", &(rect->y));
				objectIter->GetAttribute("width", &(rect->w), false);
				objectIter->GetAttribute("height", &(rect->h), false);
				object = rect;
			}
		}
		
		if(object) {
			objectIter->GetAttribute("name", &(object->name), false);
			objectIter->GetAttribute("type", &(object->type), false);
			objectIter->GetAttribute("visible", &(object->visible), false);
			
			object->properties = util::ParseProperties(objectIter->FirstChildElement("properties", false));
			
			objects.push_back(object);
		}
	}
}

void ObjectLayer::save(ticpp::Element* objectgroup) {
	objectgroup->SetAttribute("color", EncodeColorToHex(color));
	objectgroup->SetAttribute("name", name);
	if(opacity != 1.0f) objectgroup->SetAttribute("opacity", opacity);
	if(!visible) objectgroup->SetAttribute("visible", visible);
	
	ticpp::Element* propertiesElem = new ticpp::Element("properties");
	util::SaveProperties(properties, propertiesElem);
	if(!propertiesElem->NoChildren()) objectgroup->LinkEndChild(propertiesElem);
	else delete propertiesElem;
	
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		Object* o = *it;
		ticpp::Element* object = new ticpp::Element("object");
		if(o->name.length() > 0) object->SetAttribute("name", o->name);
		if(o->type.length() > 0) object->SetAttribute("type", o->type);
		
		ticpp::Element* propertiesElem = new ticpp::Element("properties");
		util::SaveProperties(o->properties, propertiesElem);
		if(!propertiesElem->NoChildren()) object->LinkEndChild(propertiesElem);
		else delete propertiesElem;
		
		switch(o->getObjectType()) {
			case OBJECT_RECTANGLE: {
				RectangleObject* rect = (RectangleObject*)o;
				object->SetAttribute("x", rect->x);
				object->SetAttribute("y", rect->y);
				object->SetAttribute("width", rect->w);
				object->SetAttribute("height", rect->h);
				
				break;
			}
			case OBJECT_ELLIPSE: {
				EllipseObject* ellipse = (EllipseObject*)o;
				object->SetAttribute("x", ellipse->x);
				object->SetAttribute("y", ellipse->y);
				object->SetAttribute("width", ellipse->w);
				object->SetAttribute("height", ellipse->h);
				
				object->LinkEndChild(new ticpp::Element("ellipse"));
				
				break;
			}
			case OBJECT_POLYGON: {
				PolygonObject* polygon = (PolygonObject*)o;
				object->SetAttribute("x", polygon->x);
				object->SetAttribute("y", polygon->y);
				
				ticpp::Element* polygonElem = new ticpp::Element("polygon");
				std::ostringstream sout;
				for(std::vector<Point>::iterator it = polygon->points.begin(); it != polygon->points.end(); ++it) {
					if(sout.tellp() != 0) sout << " ";
					sout << it->x << ',' << it->y;
				}
				polygonElem->SetAttribute("points", sout.str());
				object->LinkEndChild(polygonElem);
				
				break;
			}
			case OBJECT_POLYLINE: {
				PolylineObject* polyline = (PolylineObject*)o;
				object->SetAttribute("x", polyline->x);
				object->SetAttribute("y", polyline->y);
				
				ticpp::Element* polylineElem = new ticpp::Element("polyline");
				std::ostringstream sout;
				for(std::vector<Point>::iterator it = polyline->points.begin(); it != polyline->points.end(); ++it) {
					if(sout.tellp() != 0) sout << " ";
					sout << it->x << ',' << it->y;
				}
				polylineElem->SetAttribute("points", sout.str());
				object->LinkEndChild(polylineElem);
				
				break;
			}
			case OBJECT_TILE: {
				TileObject* tile = (TileObject*)o;
				object->SetAttribute("gid", tile->gid);
				object->SetAttribute("x", tile->x);
				object->SetAttribute("y", tile->y);
				
				break;
			}
			default: {
				std::cout << "WIP OBJECTTT" << std::endl;
				
				break;
			}
		}
		
		objectgroup->LinkEndChild(object);
	}
}

ObjectVector ObjectLayer::getObjects(ObjectType objectType) {
	ObjectVector result;
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->getObjectType() == objectType ) result.push_back(*it);
	}
	return result;
}

ObjectVector ObjectLayer::getObjectsByName(std::string name) {
	ObjectVector result;
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->name == name ) result.push_back(*it);
	}
	return result;
}

Object* ObjectLayer::getSingleObjectByName(std::string name) {
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->name == name ) return *it;
	}
	
	return NULL;
}

ObjectVector ObjectLayer::getObjectsByType(std::string type) {
	ObjectVector result;
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->type == type ) result.push_back(*it);
	}
	return result;
}

Object* ObjectLayer::getSingleObjectByType(std::string type) {
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->type == type ) return *it;
	}
	
	return NULL;
}

ObjectVector ObjectLayer::getObjectsByName(std::string name, ObjectType objectType) {
	ObjectVector result;
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->name == name && (*it)->getObjectType() == objectType ) result.push_back(*it);
	}
	return result;
}

Object* ObjectLayer::getSingleObjectByName(std::string name, ObjectType objectType) {
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->name == name && (*it)->getObjectType() == objectType ) return *it;
	}
	
	return NULL;
}

ObjectVector ObjectLayer::getObjectsByType(std::string type, ObjectType objectType) {
	ObjectVector result;
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->type == type && (*it)->getObjectType() == objectType ) result.push_back(*it);
	}
	return result;
}

Object* ObjectLayer::getSingleObjectByType(std::string type, ObjectType objectType) {
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->type == type && (*it)->getObjectType() == objectType ) return *it;
	}
	
	return NULL;
}

ObjectVector ObjectLayer::getObjectsByNameType(std::string name,
		std::string type) {
	ObjectVector result;
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->name == name && (*it)->type == type ) result.push_back(*it);
	}
	return result;
}

ObjectVector ObjectLayer::getObjectsByNameType(std::string name,
		std::string type, ObjectType objectType) {
	ObjectVector result;
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->name == name && (*it)->type == type && (*it)->getObjectType() == objectType ) result.push_back(*it);
	}
	return result;
}

Object* ObjectLayer::getSingleObjectByNameType(std::string name,
		std::string type) {
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->name == name && (*it)->type == type ) return *it;
	}
	
	return NULL;
}

Object* ObjectLayer::getSingleObjectByNameType(std::string name,
		std::string type, ObjectType objectType) {
	for(ObjectVector::iterator it = objects.begin(); it != objects.end(); ++it) {
		if( (*it)->name == name && (*it)->type == type && (*it)->getObjectType() == objectType ) return *it;
	}
	
	return NULL;
}

} /* namespace tiledpp */
