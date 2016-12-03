/* signal.c - support for signal handling
   This file is not used until Assignment 3
 */

#include <xeroskernel.h>
#include <xeroslib.h>

//registered provided newhandler as handler when signal
int sighandler(int signal, void (*newhandler)(void *),void (**oldHandler)(void *)){
	pcb *p = getCurrentProcess();

	kprintf("*SIGHANDLER*%d*%d", signal, newhandler, *oldHandler);
	//if signal is invalid, return -1
	if(signal < 0 || signal >= MAX_SIGNALS){
		return -1;
	}

	//if handler has invalid address, return -2
	if(newhandler < 0){
		return -2;
	}

	*oldHandler = p->signaltable[signal];
	p->signaltable[signal] = newhandler;

	//test
//	int i;
//	for(i=0; i<MAX_SIGNALS; i++){
//		kprintf("*i:%d %d", i, p->signaltable[i]);
//	}

	//handler successfully installed
	return 0;
}


//retrieve old return value
void sigreturn(void *old_sp){
	//replace stack pointer in this process's PCB with old_sp
	pcb *p = getCurrentProcess();
	//indicate signal can be delivered again
	memset(p->signaltable, '\0', sizeof(p->signaltable));

	p->esp = old_sp;

}


int wait(int PID){

	return 0;
}




//when kernel decides to deliver a signal to a process
//kernel modifies the application's stack so sigtramp is executed with process is
//switched to
//runs in user space as part of application
//used to control signal processing in the application
//sigtramp calls the provided handler with argument cntx
//when handler returns, sigtramp calls syssigreturn() with cntx
//syssigreturn never returns
void sigtramp(void (*handler)(void*), void *cntx){

	handler(cntx);
	void *old_sp = cntx-sizeof(cntx);
	sigreturn(old_sp); //sets stack pointer so that a context switch back to the target will pick up the old context
}

//internal to kernel
//registered when signal is to be registered for delivery to a process
//pid is destination pid, sign_no is signal to deliver
int signal(int dest_pid, int sig_no){
	if(sig_no<0 || sig_no>31){
		return -2;
	}

	if(findPCB(dest_pid) == NULL){
		return -1;
	}
	//note: process can signal themselves

	pcb *dest_process = findPCB(dest_pid);
	funcptr_signal sig_handler = dest_process->signaltable[sig_no];

	void *savedESP = dest_process->esp;

	//if a process is blocked on a syscall, when targeted to receive signal

	//target is unblocked
	//return error to syscall -362


	//first process signal
	//remember value for context switch corresponding to system call return

	return 0;
}


