/*
 * Semaphor.cpp
 *
 *  Created on: May 1, 2021
 *      Author: OS1
 */

#include "semaphor.h"

#include "Kernel.h"
#include "KernSem.h"

/*
 Zasto proveravam da li je semafor u globalnoj listi - semafor nije u globalnoj listi a pozivaju se neke metode nad njim - nesto nije u redu.
 Moguce da je na steku neke niti alociran lokalni objekat semafora, pa se zatim pozvao fork i na steku deteta se nalazi kopija lokalnog
 objekta semafora koji ima isti myImpl pokazivac. Ako se roditelj zavrsi prvi, obrisace se semafor odnosno myImpl ce se delete-ovati
 i semafor izbaciti iz globalne liste, a s obzirom da je myImpl delete-ovan mogu se desiti lose stvari ako sada dete pokusa da uradi nesto
 sa semaforom ili ga samo izbrise, zbog toga se pre bilo koje akcije nad semaforom proverava da li je on u globalnoj listi odnosno da li
 on stvarno postoji.
 Kada bismo ozbiljno spremali ovaj podsistem za neku vecu upotrebu, morali bismo po performansama da vidimo da li ovakva provera gde mora da se 
 prolazi kroz listu semafora previse pogorsava performanse, ako previse pogorsava mozemo je izostaviti i pustiti da u ovom retkom slucaju program pukne,
 jer je na neki nacin i programer pogresio ako se to desilo.
 */

Semaphore::Semaphore(int init){
	Kernel::lock();
	myImpl = new KernelSem(init);
	Kernel::unlock();
}

Semaphore::~Semaphore(){
	Kernel::lock();
	if(Kernel::semInList(myImpl)) delete myImpl; //Objasnjenje provere uslova iznad svih metoda
	Kernel::unlock();
}

int Semaphore::val() const{
	if(Kernel::semInList(myImpl)) return myImpl->val();
	else return 0; //Objasnjenje provere uslova iznad svih metoda
}

int Semaphore::wait(Time maxTimeToWait){
	if(Kernel::semInList(myImpl)) return myImpl->wait(maxTimeToWait);
	else return 0; //Objasnjenje provere uslova iznad svih metoda
}

void Semaphore::signal(){
	if(Kernel::semInList(myImpl)) myImpl->signal(); //Objasnjenje provere uslova iznad svih metoda
}
