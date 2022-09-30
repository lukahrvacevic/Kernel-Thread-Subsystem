/*
 * IVTEntry.cpp
 *
 *  Created on: May 22, 2021
 *      Author: OS1
 */

#include "IVTEntry.h"
#include "KernEv.h"
#include <dos.h>

#include "Kernel.h"

volatile IVTEntry* IVTEntry::entries[256]={0};

IVTEntry::IVTEntry(IVTNo number, intRoutine newRoutine){
	Kernel::lock();
#ifndef BCC_BLOCK_IGNORE
	ilock;
	oldRoutine = getvect(number);
	setvect(number,newRoutine);
	iunlock;
#endif
	this->event = 0;
	this->number = number;
	IVTEntry::entries[number] = this;
	Kernel::unlock();
}

IVTEntry::~IVTEntry(){
	Kernel::lock();
	IVTEntry::entries[number] = 0;
#ifndef BCC_BLOCK_IGNORE
	ilock;
	setvect(number,oldRoutine);
	iunlock;
#endif
	Kernel::unlock();
}

void IVTEntry::signal(){
	if(event!=0) event->signal();
}

void IVTEntry::callOldRoutine(){
	oldRoutine();
}

void IVTEntry::setEvent(KernelEv* event){
	this->event = event;
}

void IVTEntry::deleteEvent(KernelEv* event){
	if(this->event==event) this->event = 0;
}
