/*
 * PCB.cpp
 *
 *  Created on: Apr 27, 2021
 *      Author: OS1
 */

#include "PCB.h"
#include "SCHEDULE.h"
#include <dos.h>
#include <iostream.h>

#include "Kernel.h"
#include "List.h"
#include "NumList.h"
#include "thread.h"

const StackSize maxStackSize = 65536/sizeof(unsigned);
const StackSize minStackSize = 1024/sizeof(unsigned);

extern volatile PCB* justCreatedPCB;
extern volatile Thread* justCreatedThread;

ID PCB::id=0;

PCB::PCB(Thread* my,StackSize size,Time ts){
	justCreatedPCB = this;
	justCreatedThread = my;
	myId=id++;
	next=0;
	Kernel::addPCB(this);
	state = STARTING;
	myThread=my;
	timeSlice = (ts<0)?defaultTimeSlice:ts;
	size /= sizeof(unsigned);
	if(size<minStackSize) size = minStackSize;
	if(size>maxStackSize) size = maxStackSize;
	this->sz = size;
	stack=0;
	ss=0;
	sp=0;
	bp=0;
	semReturn = 0;
	parentID = -1;
}

int PCB::getId() volatile{return this->myId;}

void PCB::initializeStack(){
	stack = new unsigned[sz];
	stack[sz-1] = 0x200; ///PSW
	stack[sz-2] = FP_SEG(&(PCB::wrapper));//PC segmentni deo
	stack[sz-3] = FP_OFF(&(PCB::wrapper));//PC offsetni deo
	stack[sz-12] = 0;//BP offsetni deo, izmedju BP i PC su registri opste namene cije vrednosti ne moramo da inicijalizujemo ni na sta specijalno
	sp = FP_OFF(stack+sz-12);
	ss = FP_SEG(stack+sz-12);
	bp = sp;
}

void PCB::wrapper(){
	Kernel::running->myThread->run();
	exit();
}

void PCB::exit(){
	Kernel::lock();
	Kernel::running->state = FINISHED;
	while(Kernel::running->myWaitingLine.sz()>0){ //Vadimo sve niti koje nas cekaju da zavrsimo
		PCB *tmp=Kernel::running->myWaitingLine.get();
		tmp->state = READY;
		Scheduler::put(tmp);
	}
	Kernel::unlock(1);
	Kernel::dispatch();
}
//Pomocne globalne promenljive za forkroutine, imenovao sam ih sa _ ispred da ne bi dolazilo do konflikata imena
volatile unsigned _tmpsp=0,_tmpss=0,_tmpbp=0,_ssize=0;
volatile unsigned *_childstack=0;
volatile Thread* routineThread=0;
volatile PCB* routinePCB=0;
volatile unsigned* _tmpstack=0,*_tmpbase=0;
volatile int _parID = 0;
volatile char _forking=0;
volatile char _failedalloc = 0;
extern volatile PCB* justCreatedPCB;

void interrupt forkRoutine(){
	_forking = 1; //Flag - fork je u toku
	_childstack = new unsigned[Kernel::running->sz];
	if(_childstack==0){//Nema memorije, fork vraca -1
		_parID = -1;
		_forking=0;
		return;
	}
	routineThread = Kernel::running->myThread->clone();
	if(routineThread==0 || _failedalloc==1){//Opet ako nema memorije fork vraca -1, failedalloc se stavlja na 1 ako nije uspesno alociranje PCB-a
		_failedalloc = 0;
		_parID = -1;
		delete[] (unsigned*)_childstack; //Brisemo alocirani stek
		_forking=0;
		return;
	}
	routinePCB = justCreatedPCB;
	routinePCB->sz = Kernel::running->sz; //Za svaki slucaj kopiranje stack sizea i time slicea u slucaju da ne valja nadjacani clone
	routinePCB->timeSlice = Kernel::running->timeSlice;
#ifndef BCC_BLOCK_IGNORE
	asm{
		mov _tmpsp,sp
		mov _tmpss,ss
		mov _tmpbp,bp
	}
	_tmpstack = (volatile unsigned*)MK_FP(_tmpss,_tmpsp);//Racunanje trenutnog stek pointera
	_tmpbase = (volatile unsigned*)MK_FP(_tmpss,_tmpbp);//BP koji cemo posle koristiti za prepravljanje
#endif
	routinePCB->stack = (unsigned*)_childstack;
	_ssize = Kernel::running->sz - (int)((unsigned*)_tmpstack-Kernel::running->stack); //Trenutni broj podataka na steku
	for(_tmpsp = 0; _tmpsp<_ssize; _tmpsp++){
		routinePCB->stack[routinePCB->sz-1-_tmpsp] = Kernel::running->stack[routinePCB->sz-1-_tmpsp]; //Kopiranje steka
	}
#ifndef BCC_BLOCK_IGNORE
	routinePCB->sp = FP_OFF(routinePCB->stack + routinePCB->sz - _ssize); //Postavljanje bp-a i sp-a
	routinePCB->ss = FP_SEG(routinePCB->stack + routinePCB->sz - _ssize);
	routinePCB->bp = routinePCB->sp;
	while(_tmpbase>=Kernel::running->stack && _tmpbase<(Kernel::running->stack+Kernel::running->sz)){ //Azuriranje BP-ova
		routinePCB->stack[_tmpbase - Kernel::running->stack] = FP_OFF(routinePCB->stack + ((unsigned*)MK_FP(_tmpss, *_tmpbase) - Kernel::running->stack));
		_tmpbase = (volatile unsigned*)MK_FP(_tmpss, *_tmpbase);
		if(*_tmpbase == 0) break;
	}
#endif
	routinePCB->parentID = Thread::getRunningId(); //Da budem svestan da imam roditelja
	Kernel::running->forkChildren.put((PCB*)routinePCB); //Potrebno za waitforforkchildren
	if(_failedalloc == 1){ //Moze da se desi (vrlo retko) da ne moze da se alocira element za ulancanu listu, u tom slucaju sam u metodi put stavio da se stavlja flag failedalloc
		delete[] (unsigned*)_childstack;
		_failedalloc = 0;
		_forking = 0;
		_parID = -1;
		routinePCB->state = PCB::FINISHED; //Stavljamo finished da se ne bi u waittocompleteu desilo beskonacno cekanje
		delete routineThread;
		return;
	}
	routinePCB->state = PCB::READY;
	Scheduler::put((PCB*)routinePCB);
	_forking=0;
}

ID PCB::fork(){
	Kernel::lock();
	_parID = Thread::getRunningId(); //Pamtimo ko je pokrenuo fork da znamo kome ide koja povratna vrednost
	forkRoutine();
	if(_parID==-1) {
		Kernel::unlock();
		return -1; //Greska(nema memorije)
	}
	else if(_parID==Thread::getRunningId()) {
		Kernel::unlock();
		return routinePCB->myId; //Ako smo u kontekstu roditelja vracamo id novonastalog deteta
	}
	else{
		Kernel::unlock();
		return 0; //U kontekstu deteta vracamo 0
	}
}


PCB::~PCB(){
	Kernel::removePCB(this);
	if(parentID != -1){
		PCB* tmp = Kernel::getPCBbyId(parentID);
		if(tmp!=0) tmp->forkChildren.removePCB(this);
	}
	if(stack!=0) delete[] stack;
}

Thread* PCB::newThread(){return new Thread();}
