/*
 * List.cpp
 *
 *  Created on: Apr 27, 2021
 *      Author: OS1
 */

#include "List.h"
#include "PCB.h"

extern volatile char _forking;
extern volatile char _failedalloc;

List::List(){
		head=0;
		tail=0;
		size=0;
}

List::~List(){
		while(head){
			Elem* tek = head->next;
			head->next=0;
			delete head;
			size--;
			head = tek;
		}
}

void List::removePCB(PCB* pcb){
	Elem* tek = head,*prev=0;
	while(tek!=0 && tek->pcb!=pcb){
		prev = tek;
		tek = tek->next;
	}
	if(tek!=0){
		if(prev!=0) prev->next = tek->next;
		else head=tek->next;
		if(tail==tek) tail=prev;
		delete tek;
	}
}

int List::sz(){return size;}

void List::put(PCB* pcb){
		Elem* novi = new Elem(pcb);
		if(novi==0){
			if(_forking==1) _failedalloc=1;
			return;
		}
		if(head==0) head = novi;
		else tail->next = novi;
		tail=novi;
		size++;
}

PCB* List::get(){
		if(size==0) return 0;
		PCB* pcb = head->pcb;
		Elem *tek = head->next;
		head->next = 0;
		if(tek==0) tail=0;
		delete head;
		head = tek;
		size--;
		return pcb;
}
