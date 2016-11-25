/* signal.c - support for signal handling
   This file is not used until Assignment 3
 */

#include <xeroskernel.h>
#include <xeroslib.h>

//registered provided newhandler as handler when signal
int sighandler(int signal, void (*newhandler)(void *), void (** oldhandler)(void *)){
	pcb *p = getCurrentProcess();

	kprintf("*SIGHANDLER*%d*%d", signal, newhandler, *oldhandler);
	//if signal is invalid, return -1
	if(signal < 0 || signal >= MAX_SIGNALS){
		return -1;
	}

	//if handler has invalid address, return -2
	if(newhandler < 0){
		return -2;
	}

	*oldhandler = p->signaltable[signal];
	p->signaltable[signal] = newhandler;

	//test
//	int i;
//	for(i=0; i<MAX_SIGNALS; i++){
//		kprintf("*i:%d %d", i, p->signaltable[i]);
//	}

	//handler successfully installed
	return 0;
}
