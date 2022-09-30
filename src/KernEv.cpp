/*
 * KernEv.cpp
 *
 *  Created on: May 22, 2021
 *      Author: OS1
 */

#include "KernEv.h"

#include "IVTEntry.h"
#include "Kernel.h"
#include "SCHEDULE.h"
#include "PCB.h"

KernelEv::KernelEv(IVTNo number){
	IVTEntry* entry = (IVTEntry*)IVTEntry::entries[number];
	if(entry!=0) entry->setEvent(this);
	this->owner = (PCB*)Kernel::running;
	this->val = 0;
	this->number = number;
}

KernelEv::~KernelEv(){
	IVTEntry* entry = (IVTEntry*)IVTEntry::entries[number];
	if(entry!=0) entry->deleteEvent(this);
}

void KernelEv::signal(){
	if(val<1){
		val++;
		if(val==0){
			owner->state = PCB::READY;
			Scheduler::put(owner);
		}
	}
}

void KernelEv::wait(){
	if(Kernel::running==owner){
		val--;
		if(val<0) {
			Kernel::running->state = PCB::BLOCKED;
			dispatch();
		}
	}
}
