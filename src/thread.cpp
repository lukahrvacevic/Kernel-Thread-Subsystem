/*
 * Thread.cpp
 *
 *  Created on: Apr 27, 2021
 *      Author: OS1
 */

#include "PCB.h"
#include "List.h"
#include "NumList.h"
#include "thread.h"

#include "Kernel.h"

/*
 U nestatickim metodama i destruktoru ispitujem uslov da li je PCB u globalnoj listi - objasnjenje zasto u semaphore.cpp(isto sve vazi)
 */

Thread* Thread::clone() const{
	if(Kernel::pcbInList(myPCB)){
		Thread* clon = new Thread();
		if(clon!=0){
			clon->myPCB->timeSlice = myPCB->timeSlice;
			clon->myPCB->sz = myPCB->sz;
		}
		return clon;
	}
	else return 0;
}

#include <iostream.h>

void Thread::waitForForkChildren(){
	List::Elem* tmp = Kernel::running->forkChildren.head;
	while(tmp!=0){
		tmp->pcb->myThread->waitToComplete();
		tmp=tmp->next;
	}
}

ID Thread::fork(){
	return PCB::fork();
}

void Thread::exit(){
	PCB::exit();
}

void Thread::run(){}

void Thread::start(){
	Kernel::lock();
	if(Kernel::pcbInList(myPCB)){
		if(myPCB->state==PCB::STARTING){ //Da se ne bi pozivalo start vise puta za istu nit
			myPCB->initializeStack();
			myPCB->state = PCB::READY;
			Scheduler::put(myPCB);
		}
	}
	Kernel::unlock();
}

extern volatile char _forking;
extern volatile char _failedalloc;

Thread::Thread(StackSize stackSize, Time timeSlice){
	Kernel::lock();
	myPCB = new PCB(this, stackSize, timeSlice);
	if(myPCB==0){
		if(_forking==1) _failedalloc=1;
	}
	Kernel::unlock();
}

ID Thread::getId(){
	if(Kernel::pcbInList(myPCB)) return myPCB->getId();
	else return -1;
}

ID Thread::getRunningId(){
	return Kernel::running->getId();
}

void dispatch(){
	Kernel::dispatch();
}

void Thread::waitToComplete(){
	if(Kernel::pcbInList(myPCB)){
		Kernel::lock();
		if(this->myPCB->state == PCB::FINISHED){ //Necemo da je cekamo ako je zavrsila
			Kernel::unlock();
			return;
		}
		if(this->myPCB==Kernel::running || this==Kernel::mainThread || this==Kernel::idleThread){ //Ne zelimo bas da nit ceka samu sebe, i ne dopustamo da ceka da se zavrsi main nit jer
			//sve niti moraju da se zavrse pre nego sto se main nit zavrsi, i nema bas puno smisla da nit ceka idle thread koja nikad nece zavrsiti
			Kernel::unlock();
			return;
		}
		if(this==(Thread*)Kernel::userMainThread && Kernel::running->myThread!=Kernel::mainThread){
			//Ne zelimo da bilo ko ceka usermain nit sem main niti
			Kernel::unlock();
			return;
		}
		Kernel::running->state = PCB::BLOCKED;
		this->myPCB->myWaitingLine.put((PCB*)Kernel::running);
		Kernel::unlock(1);
		dispatch();
	}
}

Thread* Thread::getThreadById(ID id){
	Kernel::lock();
	PCB* pcb = Kernel::getPCBbyId(id);
	Kernel::unlock();
	if(pcb==0) return 0;
	else return pcb->myThread;
}

Thread::~Thread(){
	Kernel::lock();
	if(Kernel::pcbInList(myPCB)) delete myPCB;
	Kernel::unlock();
}
