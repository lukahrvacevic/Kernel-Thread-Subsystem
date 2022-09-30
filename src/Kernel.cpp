/*
 * Kernel.cpp
 *
 *  Created on: Apr 27, 2021
 *      Author: OS1
 */

#include "Kernel.h"

#include "IdleThr.h"
#include "PCB.h"
#include "List.h"
#include "KernSem.h"
#include "PQueue.h"
#include <dos.h>
#include <iostream.h>
#include "NumList.h"
#include <stdlib.h>
#include "UMThr.h"

volatile PCB* Kernel::running=0;
volatile Time Kernel::slicesLeft=0;
Thread* Kernel::mainThread=0;
Thread* Kernel::idleThread=0;
UserMainThread* Kernel::userMainThread = 0;
volatile char Kernel::lockFlag=0;
volatile char Kernel::switchOnDemand=0;
volatile char Kernel::changeBecauseOfLock=0;
PCB* Kernel::IdlePCB=0;
volatile PCB* Kernel::pcbhead=0;
volatile PCB* Kernel::pcbtail=0;
volatile KernelSem* Kernel::semhead=0;
volatile KernelSem* Kernel::semtail=0;
void interrupt (*Kernel::oldRoutine)(...) = 0;
volatile PCB* justCreatedPCB = 0;
volatile Thread* justCreatedThread = 0;

volatile unsigned int tsp=0;
volatile unsigned int tss=0;
volatile unsigned int tbp=0;
volatile PCB* tmp=0;

extern void tick();

void Kernel::decrementSems(){
	KernelSem *tmp = (KernelSem*)semhead;
	while(tmp!=0){
		tmp->limitedWaitingPCBs->head->time--;
		tmp = tmp->next;
	}
}

void Kernel::unblockSems(){
	KernelSem *tmp = (KernelSem*)semhead;
	while(tmp!=0){
		while(!tmp->limitedWaitingPCBs->isEmpty() && tmp->limitedWaitingPCBs->headIs0()){
			PCB *pcb = tmp->limitedWaitingPCBs->get();
			pcb->state = PCB::READY;
			Scheduler::put(pcb);
			tmp->value++;
			tmp->fifoList->removeNum(0);
		}
		tmp = tmp->next;
	}
}

int Kernel::pcbInList(PCB* pcb){
	PCB* tmp = 0;
	for(tmp=(PCB*)pcbhead; tmp!=0 && tmp!=pcb; tmp=tmp->next) ;
	return tmp!=0;
}

int Kernel::semInList(KernelSem* sem){
	KernelSem* tmp = 0;
	for(tmp=(KernelSem*)semhead; tmp!=0 && tmp!=sem; tmp=tmp->next) ;
	return tmp!=0;
}

void Kernel::addPCB(PCB* pcb){ //NIKAKO staviti isti PCB vise puta
	if(pcbhead==0) pcbhead=pcb;
	else pcbtail->next = pcb;
	pcbtail=pcb;
}

void Kernel::addSem(KernelSem* sem){ //NIKAKO staviti isti sem vise puta
	if(semhead==0) semhead = sem;
	else semtail->next = sem;
	semtail=sem;
}

void Kernel::removePCB(PCB* pcb){
	if(pcbhead==0) return;
	if(pcb==pcbhead) {
		pcbhead=pcbhead->next;
		if(pcbhead==0) pcbtail=0;
	}
	else{
		PCB* tmp=(PCB*)pcbhead;
		while(tmp!=0 && tmp->next!=pcb) tmp=tmp->next;
		if(tmp!=0) tmp->next = pcb->next;
		if(pcb==pcbtail) pcbtail=tmp;
	}
	pcb->next=0;
}

void Kernel::removeSem(KernelSem* sem){
	if(semhead==0) return;
	if(sem==semhead) {
		semhead=semhead->next;
		if(semhead==0) semtail=0;
	}
	else{
		KernelSem* tmp=(KernelSem*)semhead;
		while(tmp!=0 && tmp->next!=sem) tmp=tmp->next;
		if(tmp!=0) tmp->next = sem->next;
		if(sem==semtail) semtail=tmp;
	}
	sem->next=0;
}

PCB* Kernel::getPCBbyId(ID id){
	if(id==mainThread->getId() || id==idleThread->getId() || id==userMainThread->getId()) return 0; //Ne dozvoljavam pristup kernelovim MainThread,IdleThread, i UserMainThread da ih korisnik ne bi
										  //(slucajno ili namerno) obrisao ili uradio nesto cudno s njima
	PCB *tmp=(PCB*)pcbhead;
	while(tmp!=0 && tmp->myId!=id) tmp=tmp->next;
	return tmp;
}

void Kernel::lock(){
		lockFlag=1;
}

void Kernel::unlock(int dontDispatch){
		lockFlag=0;
		if(changeBecauseOfLock==1){
			changeBecauseOfLock=0;
			if(dontDispatch==0) dispatch();
		}
}

void interrupt timer(...){
	if(Kernel::switchOnDemand==0) {
		Kernel::oldRoutine();
		tick();
		if(Kernel::slicesLeft > 0) Kernel::slicesLeft--;
		Kernel::decrementSems();
		Kernel::unblockSems();
	}
	if(Kernel::slicesLeft==0 || Kernel::switchOnDemand==1){
		if(!Kernel::lockFlag){
			Kernel::switchOnDemand = 0;
			if(Kernel::running->state==PCB::READY && Kernel::running->myThread!=Kernel::idleThread) Scheduler::put((PCB*)Kernel::running);
			tmp = Scheduler::get();
			if(tmp==0) tmp = (volatile PCB*)(Kernel::IdlePCB);
			if(tmp!=Kernel::running){ //Slucaj: stavio sam nit u scheduler i izvukao istu, ne zelim da  menjam kontekst bez razloga
#ifndef BCC_BLOCK_IGNORE
				asm {
							mov tbp, bp
							mov tsp, sp
							mov tss, ss
					}
#endif
				Kernel::running->sp = tsp;
				Kernel::running->ss = tss;
				Kernel::running->bp = tbp;
				Kernel::running = tmp;
				if(Kernel::running->timeSlice==0) Kernel::slicesLeft=-1;
				else Kernel::slicesLeft = Kernel::running->timeSlice;
				tsp = Kernel::running->sp;
				tss = Kernel::running->ss;
				tbp = Kernel::running->bp;
#ifndef BCC_BLOCK_IGNORE
				asm {
							mov sp, tsp
							mov ss, tss
							mov bp, tbp
					}
#endif
			}
		}
		else Kernel::changeBecauseOfLock=1; //Flag koji nam govori dok si bio u zakljucanoj sekciji desio se prekid izvrsi promenu konteksta sto brze mozes(kad izadjes iz zakljucane sekcije)
	}
}

void Kernel::initialize(){
#ifndef BCC_BLOCK_IGNORE
		ilock;
		oldRoutine = getvect(0x08);
		setvect(0x08, timer);
#endif
		slicesLeft = 2;
		mainThread = PCB::newThread();
		running = justCreatedPCB;
		running->state = PCB::READY;
		idleThread = new IdleThread();
		IdlePCB = (PCB*)justCreatedPCB;
		IdlePCB->initializeStack();
		IdlePCB->state = PCB::READY;
#ifndef BCC_BLOCK_IGNORE
		iunlock;
#endif
}

void Kernel::restore(){
#ifndef BCC_BLOCK_IGNORE
		ilock;
#endif
		setvect(0x08, oldRoutine);
		delete mainThread;
		delete idleThread;
#ifndef BCC_BLOCK_IGNORE
		iunlock;
#endif
}

volatile char inInterrupt = 0;

void Kernel::dispatch(){
#ifndef BCC_BLOCK_IGNORE
	ilock;
	if(inInterrupt==0 || (inInterrupt==1 && lockFlag==0)){
		if(inInterrupt==1) inInterrupt = 0;
		switchOnDemand = 1;
		asm {int 8h}
	}
	else changeBecauseOfLock=1;
	iunlock;
#endif
}
