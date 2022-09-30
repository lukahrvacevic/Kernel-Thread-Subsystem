/*
 * UMThr.h
 *
 *  Created on: May 2, 2021
 *      Author: OS1
 */

#ifndef UMTHR_H_
#define UMTHR_H_

#include "thread.h"

/*
 Potrebno je da postoji nit koja se pravi nad userMainom kako bismo imali pristup steku da ga kopiramo kada se pozove fork() u userMainu.
 */


extern int userMain(int argc,char *argv[]);

class UserMainThread: public Thread{

	int argc;
	char **argv;
	int ret;

public:

	UserMainThread(int argc,char *argv[]): Thread(32768,2), argc(argc), argv(argv), ret(0){}

	virtual void run(){
		ret=userMain(argc,argv);
	}

	int getReturn() const{return ret;}

};



#endif /* UMTHR_H_ */
