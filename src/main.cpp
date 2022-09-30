/*
 * main.cpp
 *
 *  Created on: Apr 27, 2021
 *      Author: OS1
 */


#include "Kernel.h"
#include "UMThr.h"

extern int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]){
	Kernel::initialize();
	Kernel::userMainThread = new UserMainThread(argc,argv);
	Kernel::userMainThread->start();
	Kernel::userMainThread->waitToComplete();
	int value=Kernel::userMainThread->getReturn();
	delete Kernel::userMainThread;
	Kernel::restore();
	return value;
}
