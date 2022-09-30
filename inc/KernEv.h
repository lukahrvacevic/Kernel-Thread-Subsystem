/*
 * KernEv.h
 *
 *  Created on: May 22, 2021
 *      Author: OS1
 */

#ifndef KERNEV_H_
#define KERNEV_H_

typedef unsigned char IVTNo;

class PCB;

class KernelEv {

	int val;
	PCB* owner;
	IVTNo number;

public:
	KernelEv(IVTNo number);
	~KernelEv();

	void signal();

	void wait();
};

#endif /* KERNEV_H_ */
