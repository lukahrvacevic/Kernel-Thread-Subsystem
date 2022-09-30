/*
 * NumList.cpp
 *
 *  Created on: May 10, 2021
 *      Author: OS1
 */

#include "NumList.h"

void NumList::add(int num){
	Elem *novi=new Elem(num);
	if(head==0) head = novi;
	else tail->next = novi;
	tail = novi;
}

void NumList::removeNum(int num){
	Elem *tmp=head,*prev=0;
	while(tmp!=0 && tmp->num!=num) {
		prev = tmp;
		tmp=tmp->next;
	}
	if(tmp!=0){
		if(prev!=0) prev->next = tmp->next;
		else head=tmp->next;
		if(tail==tmp) tail=prev;
		delete tmp;
	}
}

int NumList::removeFirst(){
	int ret = head->num;
	Elem *tmp = head->next;
	delete head;
	head = tmp;
	if(head==0) tail = 0;
	return ret;
}
