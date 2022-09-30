/*
 * NumList.h
 *
 *  Created on: May 10, 2021
 *      Author: OS1
 */

#ifndef NUMLIST_H_
#define NUMLIST_H_

class NumList {
public:
	struct Elem{
		int num;
		Elem* next;
		Elem(int num): num(num), next(0){}
	};

	Elem *head,*tail;

	NumList(){
		head=tail=0;
	}

	~NumList(){
		while(head){
			Elem *tmp=head->next;
			delete head;
			head=tmp;
		}
	}

	void add(int num);

	void removeNum(int num);

	int removeFirst();

	int isEmpty(){return head==0;}
};

#endif /* NUMLIST_H_ */
