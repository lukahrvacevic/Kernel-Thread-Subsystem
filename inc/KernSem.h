/*
 * KernSem.h
 *
 *  Created on: May 1, 2021
 *      Author: OS1
 */

#ifndef KERNSEM_H_
#define KERNSEM_H_

#include "PCB.h"

class List;
class PCB;
class PQueue;
class NumList;

class KernelSem {

	friend class Kernel;

	int value;
	List* unlimitedWaitingPCBs; //niti koje cekaju neograniceno vreme
	PQueue* limitedWaitingPCBs; //niti koje cekaju ograniceno
	NumList* fifoList; //da znam koji tip niti je po fifo redu: kad dodje nit koja ceka neograniceno ubacujem 1, za nit koja ceka ograniceno ubacujem 0
	KernelSem* next; //Za ulancavanje u globalnu listu svih semafora

public:

	KernelSem(int init);

	~KernelSem();

	int val() const{
		return value;
	}

	int wait(Time maxTimeToWait);

	void signal();
};

#endif /* KERNSEM_H_ */
