/*
 * IdleThread.h
 *
 *  Created on: Apr 27, 2021
 *      Author: OS1
 */

#ifndef IDLETHR_H_
#define IDLETHR_H_

#include <iostream.h>

#include "thread.h"

class IdleThread: public Thread{

public:
	IdleThread();

	virtual void run();
};





#endif /* IDLETHR_H_ */
