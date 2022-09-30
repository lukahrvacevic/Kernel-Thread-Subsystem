/*
 * KernSem.cpp
 *
 *  Created on: May 1, 2021
 *      Author: OS1
 */

#include "KernSem.h"

#include "Kernel.h"
#include "List.h"
#include "PQueue.h"
#include "NumList.h"

KernelSem::KernelSem(int init){
	value = init;
	if(value<0) value = 0;
	unlimitedWaitingPCBs = new List();
	limitedWaitingPCBs = new PQueue();
	fifoList = new NumList();
	next = 0;
	Kernel::addSem(this);
}

KernelSem::~KernelSem(){
	//mozda treba da se odblokiraju svi pcbovi
	delete unlimitedWaitingPCBs;
	delete limitedWaitingPCBs;
	Kernel::removeSem(this);
}

int KernelSem::wait(Time maxTimeToWait){
	Kernel::lock();
	Kernel::running->semReturn = 0;
	value--;
	int flag = 0;
	if(value<0){
		flag = 1;
#ifndef BCC_BLOCK_IGNORE
		if(maxTimeToWait==0){
			ilock;
			this->unlimitedWaitingPCBs->put((PCB*)Kernel::running);
			this->fifoList->add(1);
			iunlock;
		}
		else{
			ilock;
			this->limitedWaitingPCBs->put((PCB*)Kernel::running, maxTimeToWait);
			this->fifoList->add(0);
			iunlock;
		}
#endif
		Kernel::running->state = PCB::BLOCKED;
		Kernel::unlock(1);
		dispatch();
	}
	else Kernel::running->semReturn = 1; // maybe
	int ret = Kernel::running->semReturn;
	Kernel::running->semReturn = 0;
	if(!flag) Kernel::unlock();
	return ret;
}

void KernelSem::signal(){
	Kernel::lock();
	value++;
	if(value<=0){
		PCB* tmp = 0;
#ifndef BCC_BLOCK_IGNORE
		ilock;
		if(this->fifoList->removeFirst()==1) tmp = this->unlimitedWaitingPCBs->get();
		else tmp = this->limitedWaitingPCBs->fifoGet();
		iunlock;
#endif
		if(tmp!=0){
			tmp->state = PCB::READY;
			tmp->semReturn = 1;
			Scheduler::put(tmp);
		}
	}
	Kernel::unlock();
}
