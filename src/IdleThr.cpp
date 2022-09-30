/*
 * IdleThr.cpp
 *
 *  Created on: Apr 28, 2021
 *      Author: OS1
 */

#include "IdleThr.h"
#include <iostream.h>

#include "thread.h"

IdleThread::IdleThread(): Thread(defaultStackSize,1) {}

volatile int petlja=1;

void IdleThread::run(){
		while(petlja){}
}
