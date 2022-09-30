/*
 * PQueue.cpp
 *
 *  Created on: May 2, 2021
 *      Author: OS1
 */

#include "PQueue.h"

#include "PCB.h"

int PQueue::globalId = 0;

void PQueue::put(PCB* pcb, int time){
	Elem* novi=new Elem(pcb,time);
	if(head==0) head = novi;
	else{
		Elem* tmp=head;
		Elem* prev=0;
		while(tmp!=0 && tmp->time<=time){
			time -= tmp->time;
			prev = tmp;
			tmp = tmp->next;
		}
		novi->time = time;
		if(tmp!=0){
			tmp->time -= time;
			novi->next = tmp;
			if(prev) prev->next = novi;
			else head=novi;
		}
		else prev->next = novi;
	}
}

PCB* PQueue::get(){
	PCB* ret=0;
	if(head==0) return ret;
	ret = head->pcb;
	Elem* tmp=head->next;
	delete head;
	head = tmp;
	return ret;
}

PCB* PQueue::fifoGet(){
	if(head==0) return 0;
	Elem *tmp = head, *prev = 0;
	int min = tmp->id;
	Elem *tmp2 = tmp->next, *prev2 = head;
	while(tmp2!=0){
		if(tmp2->id<min){
			min = tmp2->id;
			tmp = tmp2;
			prev = prev2;
		}
		prev2 = tmp2;
		tmp2 = tmp2->next;
	}
	PCB* ret = tmp->pcb;
	if(prev!=0) prev->next = tmp->next;
	else head = tmp->next;
	if(tmp->next!=0){
		tmp->next->time += tmp->time;
	}
	delete tmp;
	return ret;
}
