/*
 * IVTEntry.h
 *
 *  Created on: May 22, 2021
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

typedef void interrupt (*intRoutine)(...);
typedef unsigned char IVTNo;


class KernelEv;
class Kernel;

#include "Kernel.h"

class IVTEntry {

	IVTNo number;
	intRoutine oldRoutine;
	KernelEv* event;

public:
	IVTEntry(IVTNo number, intRoutine newRoutine);
	~IVTEntry();

	void signal();

	void callOldRoutine();

	void setEvent(KernelEv* event);

	void deleteEvent(KernelEv* event);

	static volatile IVTEntry *entries[];
};

#endif /* IVTENTRY_H_ */
