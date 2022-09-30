/*
 * Kernel.h
 *
 *  Created on: Apr 27, 2021
 *      Author: OS1
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include "List.h"
#include "PCB.h"

class List;
class PCB;
class Thread;
class IdleThread;
class KernelSem;
class UserMainThread;

#define ilock asm{pushf; \
	cli}

#define iunlock asm{popf}

class Kernel {


public:
	static volatile PCB* running;
	static volatile Time slicesLeft;
	static Thread* mainThread;
	static Thread* idleThread;
	static UserMainThread* userMainThread;
	static volatile char lockFlag;
	static volatile char switchOnDemand;
	static volatile char changeBecauseOfLock;
	static PCB* IdlePCB;
	static volatile PCB* pcbhead;
	static volatile PCB* pcbtail;
	static volatile KernelSem* semhead;
	static volatile KernelSem* semtail;
	static void interrupt (*oldRoutine)(...);

	static void initialize();

	static void restore();

	static void dispatch();

	static void lock();

	static void unlock(int dontDispatch=0);

	static void addPCB(PCB* pcb);

	static void addSem(KernelSem* sem);

	static void removePCB(PCB* pcb);

	static void removeSem(KernelSem* sem);

	static PCB* getPCBbyId(ID id);

	static void decrementSems();

	static void unblockSems();

	static int pcbInList(PCB* pcb);

	static int semInList(KernelSem* sem);

private:

	friend class PCB;

	friend class Thread;

	friend class List;

};

#endif /* KERNEL_H_ */
