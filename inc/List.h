/*
 * List.h
 *
 *  Created on: Apr 27, 2021
 *      Author: OS1
 */

#ifndef LIST_H_
#define LIST_H_

class PCB;

class List{
public:
	struct Elem{
		PCB* pcb;
		Elem* next;
		Elem(PCB* pcb): pcb(pcb), next(0) {}
	};

	Elem *head,*tail;
	int size;

	List();

	~List();

	void put(PCB* pcb);

	PCB* get();

	int sz();

	void removePCB(PCB* pcb);

};



#endif /* LIST_H_ */
