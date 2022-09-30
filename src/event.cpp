/*
 * event.cpp
 *
 *  Created on: May 22, 2021
 *      Author: OS1
 */

#include "event.h"

#include "Kernel.h"
#include "KernEv.h"

Event::Event(IVTNo number){
	Kernel::lock();
	myImpl = new KernelEv(number);
	Kernel::unlock();
}

Event::~Event(){
	Kernel::lock();
	delete myImpl;
	Kernel::unlock();
}

void Event::wait(){
	myImpl->wait();
}

void Event::signal(){
	myImpl->signal();
}
