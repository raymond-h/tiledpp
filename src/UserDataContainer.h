/*
 * UserDataContainer.h
 *
 *  Created on: 29 jan 2013
 *      Author: Raymond
 */

#ifndef USERDATACONTAINER_H_
#define USERDATACONTAINER_H_

namespace tiledpp {

class UserDataContainer {
public:
	void* getUserData() { return userdata; };
	
	template <typename T>
	T* getUserDataAs() { return static_cast<T*>( userdata ); };
	
	void setUserData(void* userdata) { this->userdata = userdata; };
	
private:
	void* userdata;
};

} //namedspace tiledpp

#endif /* USERDATACONTAINER_H_ */
