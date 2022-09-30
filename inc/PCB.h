/*
 * PCB.h
 *
 *  Created on: Apr 27, 2021
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

#include "SCHEDULE.h"
#include "thread.h"

class NumList;

class List;

class Thread;

#include <List.h>

class PCB {
public:

	enum State{STARTING, READY, BLOCKED, FINISHED};

	PCB(Thread* my,StackSize sz,Time ts);

	~PCB();

	static Thread* newThread();

	static void wrapper();

	static void exit();

	ID getId() volatile;

	void initializeStack(); //Odvojena metoda za ovo jer ako je main nit ne treba joj stek

	static ID fork();
private:

	Thread *myThread;
	unsigned *stack;
	unsigned ss;
	unsigned sp;
	unsigned bp;
	unsigned sz;
	Time timeSlice;
	List myWaitingLine; //Niti koje cekaju da se zavrsim
	List forkChildren;
	PCB* next; //Ulancavanje u okviru samih pcb-ova za listu svih pcb-ova
	volatile State state;
	volatile char semReturn; //Za povratak iz semaphor::wait - 1 znaci odblokirana je zbog signal, 0 znaci odblokirana je zbog isteka vremena
	static ID id;
	ID myId;
	ID parentID;

	friend class Thread;
	friend class Kernel;
	friend class KernelSem;
	friend class KernelEv;

	friend void interrupt timer(...);
	friend void interrupt forkRoutine();

};

#endif /* PCB_H_ */
