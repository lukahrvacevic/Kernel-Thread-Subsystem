/*
 * PQueue.h
 *
 *  Created on: May 2, 2021
 *      Author: OS1
 */

#ifndef PQUEUE_H_
#define PQUEUE_H_

class PCB;

class PQueue {
public:
	static int globalId;

	struct Elem{
		PCB* pcb;
		unsigned int time;
		int id;
		Elem* next;
		Elem(PCB* pcb, int time): pcb(pcb), time(time), next(0){
			id = PQueue::globalId++;
		}
	};
	Elem *head;

	PQueue(){
		head=0;
	}

	~PQueue(){
		while(head){
			Elem* tmp=head->next;
			delete head;
			head = tmp;
		}
	}

	int isEmpty() const {return head==0;}

	void put(PCB* pcb, int time);

	int headIs0() const {return head->time==0;}

	PCB* get();

	PCB* fifoGet();
};

#endif /* PQUEUE_H_ */
