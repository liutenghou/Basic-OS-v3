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



// This function takes 2 paramenters:
//  currP  - a pointer into the pcbtab that identifies the currently running process
//  pid    - the proces ID of the process to be killed.
//
// Note: this function needs to be augmented so that it delivers a kill signal to a
//       a particular process. The main functionality of the this routine will remain the
//       same except that when the process is located it needs to be put onto the readyq
//       and a signal needs to be marked for delivery.
//

//requests that a signal be delivered to process
//pid is receiver pid
//return -712 if process does not exist
//return -651 if signal number is invalid
int kill(pcb *currP, int pid, int signalNumber) {
	pcb * targetPCB;

	kprintf("Current pid %d Killing %d\n", currP->pid, pid);

	if (pid == currP->pid) {   // Trying to kill self
		return -2;
	}

	// Don't let it kill the idle process, which from the user side
	// of things isn't a real process
	// IDLE process had PID 0

	if (pid == 0) {
		return -1;
	}

	if (!(targetPCB = findPCB(pid))) {
		// kprintf("Target pid not found\n");
		return -1;
	}

	if (targetPCB->state == STATE_STOPPED) {
		kprintf("Target pid was stopped\n");
		return -1;
	}

	// PCB has been found,  and the proces is either sleepign or running.
	// based on that information remove the process from
	// the appropriate queue/list.

	if (targetPCB->state == STATE_SLEEP) {
		// kprintf("Target pid %d sleeping\n", targetPCB->pid);
		removeFromSleep(targetPCB);
	}

	if (targetPCB->state == STATE_READY) {
		// remove from ready queue
		// kprintf("Target pid %d is ready\n", targetPCB->pid);
		removeFromReady(targetPCB);
	}

	// Check other states and do state specific cleanup before stopping
	// the process
	// In the new version the process will not be marked as stopped but be
	// put onto the readyq and a signal marked for delivery.

	targetPCB->state = STATE_STOPPED;
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


}

//internal to kernel
//registered when signal is to be registered for delivery to a process
//pid is destination pid, sign_no is signal to deliver
int signal(int dest_pid, int sig_no){

	//processes can signal themselves

	//if a process is blocked on a syscall, when targeted to receive signal

	//target is unblocked
	//return error to syscall -362


	//first process signal
	//remember value for context switch corresponding to system call return

	return 0;
}


