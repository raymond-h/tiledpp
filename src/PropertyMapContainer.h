/*
 * PropertyMap.h
 *
 *  Created on: 28 jan 2013
 *      Author: Raymond
 */

#include <map>
#include <sstream>

#ifndef PROPERTYMAPCONTAINER_H_
#define PROPERTYMAPCONTAINER_H_

#include "Color.h"

namespace tiledpp {

typedef std::map<std::string, std::string> PropertyMap;

class PropertyMapContainer {
public:
	PropertyMap getProperties() { return properties; };
	
	std::string getProperty(const std::string& name) { return properties[name]; };
	
	template <typename T>
	bool getProperty(const std::string& name, T* value) {
		if(!hasProperty(name)) return false;
		
		std::istringstream iss(properties[name]);
		iss >> *value;
		return !iss.fail();
	};
	
	bool getProperty(const std::string& name, std::string* value) {
		if(!hasProperty(name)) return false;
		
		*value = properties[name];
		return true;
	}
	
	bool getProperty(const std::string& name, Color* value) {
		if(!hasProperty(name)) return false;
		
		return DecodeColorFromHex(properties[name], value);
	}
	
	template <typename T>
	T getPropertyAs(const std::string& name) {
		T r = T();
		getProperty(name, &r);
		return r;
	}
	
	template <typename T>
	T getPropertyAs(const std::string& name, const T def) {
		T r = T();
		if(!getProperty(name, &r)) return def;
		return r;
	}
	
	bool hasProperty(const std::string& name) { return properties.count(name) > 0; };
	
protected:
	PropertyMap properties;
};

} //namespace tiledpp

#endif /* PROPERTYMAP_H_ */
