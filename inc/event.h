/*
 * event.h
 *
 *  Created on: May 22, 2021
 *      Author: OS1
 */

#ifndef EVENT_H_
#define EVENT_H_

typedef unsigned char IVTNo;

#include "IVTEntry.h"

extern volatile char inInterrupt;

#define PREPAREENTRY(number,old)\
void interrupt intr##number(...);\
IVTEntry entr##number((IVTNo)number, intr##number);\
void interrupt intr##number(...){\
	entr##number.signal();\
	if(old == 1) entr##number.callOldRoutine();\
	inInterrupt=1;\
	dispatch();\
	inInterrupt=0;\
}\

class KernelEv;

class Event {

public:

	Event (IVTNo ivtNo);

	~Event ();

	void wait ();
protected:

	friend class KernelEv;

	void signal(); // can call KernelEv
private:

	KernelEv* myImpl;
};

#endif /* EVENT_H_ */
