/*
 * ObjectLayer.h
 *
 *  Created on: 19 dec 2012
 *      Author: Raymond
 */

#ifndef OBJECTLAYER_H_
#define OBJECTLAYER_H_

#include <vector>

#include "Map.h"
#include "Layer.h"
#include "Color.h"
#include "Point.h"
#include "PropertyMapContainer.h"
#include "UserDataContainer.h"

namespace tiledpp {

enum ObjectType { OBJECT_UNKNOWN, OBJECT_RECTANGLE, OBJECT_POLYGON, OBJECT_POLYLINE, OBJECT_TILE, OBJECT_ELLIPSE };

class ObjectLayer;

class Object : public PropertyMapContainer, public UserDataContainer {
public:
	Object():name(),type(),visible(true) {};
	virtual ~Object() {};

	virtual ObjectType getObjectType() = 0;
	
	std::string name;
	std::string type;
	
	bool visible;
	
	friend class ObjectLayer;
};

typedef std::vector<Object*> ObjectVector;

class ObjectLayer : public Layer {
private:
	ObjectLayer(Map* owner);
	virtual ~ObjectLayer();
	
	friend class Map;
	
	void load(ticpp::Element* objectgroup);
	void save(ticpp::Element* objectgroup);
	
public:
	LayerType getLayerType() { return LAYER_OBJECT; };
	
	ObjectVector getObjects() { return objects; };
	ObjectVector getObjects(ObjectType objectType);
	
	ObjectVector getObjectsByName(std::string name);
	ObjectVector getObjectsByName(std::string name, ObjectType objectType);
	
	ObjectVector getObjectsByType(std::string type);
	ObjectVector getObjectsByType(std::string type, ObjectType objectType);
	
	ObjectVector getObjectsByNameType(std::string name, std::string type);
	ObjectVector getObjectsByNameType(std::string name, std::string type, ObjectType objectType);
	
	Object* getSingleObjectByName(std::string name);
	Object* getSingleObjectByName(std::string name, ObjectType objectType);
	
	Object* getSingleObjectByType(std::string type);
	Object* getSingleObjectByType(std::string type, ObjectType objectType);
	
	Object* getSingleObjectByNameType(std::string name, std::string type);
	Object* getSingleObjectByNameType(std::string name, std::string type, ObjectType objectType);
	
	Map* getOwner() { return owner; };
	
private:
	Map* owner;
	
	Color color;
	
	ObjectVector objects;
};

class RectangleObject : public Object {
public:
	ObjectType getObjectType() { return OBJECT_RECTANGLE; }
	
	long x;
	long y;
	
	long w;
	long h;
};

class PolygonObject : public Object {
public:
	ObjectType getObjectType() { return OBJECT_POLYGON; };
	
	long x;
	long y;
	
	std::vector<Point> points;
};

class PolylineObject : public Object {
public:
	ObjectType getObjectType() { return OBJECT_POLYLINE; };
	
	long x;
	long y;
	
	std::vector<Point> points;
};

class TileObject : public Object {
public:
	ObjectType getObjectType() { return OBJECT_TILE; };
	
	long x;
	long y;
	
	unsigned long gid;
};

class EllipseObject : public Object {
public:
	ObjectType getObjectType() { return OBJECT_ELLIPSE; };
	
	long x;
	long y;
	
	long w;
	long h;
};

} /* namespace tiledpp */
#endif /* OBJECTLAYER_H_ */
