/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>


int syscall( int req, ... ) {
/**********************************/

    va_list     ap;
    int         rc;

    va_start( ap, req );

    __asm __volatile( " \
        movl %1, %%eax \n\
        movl %2, %%edx \n\
        int  %3 \n\
        movl %%eax, %0 \n\
        "
        : "=g" (rc)
        : "g" (req), "g" (ap), "i" (KERNEL_INT)
        : "%eax" 
    );
 
    va_end( ap );

    return( rc );
}

int syscreate( funcptr fp, size_t stack ) {
    return( syscall( SYS_CREATE, fp, stack ) );
}

void sysyield( void ) {
  syscall( SYS_YIELD );
}

 void sysstop( void ) {
   syscall( SYS_STOP );
}

unsigned int sysgetpid( void ) {
    return( syscall( SYS_GETPID ) );
}

void sysputs( char *str ) {
    syscall( SYS_PUTS, str );
}

unsigned int syssleep( unsigned int t ) {
    return syscall( SYS_SLEEP, t );
}

//syscalls A3---------------------------------------------------------------------------------------
int syssighandler(int signal, void (*newhandler)(void *), void (** oldHandler)(void *)){
	return syscall(SYS_SIGHANDLER, signal, newhandler, oldHandler);
}

//used by trampoline cold
//parameter: location on application stack of cf to switch process to
//calls sigreturn() in kernel
void syssigreturn(void *old_sp){
	syscall(SYS_SIGRETURN, old_sp);
}

//modified for A3
//name is not descriptive, actually sends a signal to a process
//calls signal() in kernel
int syskill(int pid, int signalNumber) {
	//do checking here
	//returns -712 if PID is invalid
	if(findPCB(pid) == NULL){
		return -712;
	}

	//returns -651 if signal is invalid
	if(signalNumber<0 || signalNumber >31){
		return -651;
	}

	syscall(SYS_KILL, pid, signalNumber);
	return 0;
}

//causes <calling process> to wait for <process with PID> to terminate
int syswait(int PID){
	return syscall(SYS_WAIT, PID);
}
//--------------------------------------------------------------------------------------------------


int sysgetcputimes(processStatuses *ps) {
  return syscall(SYS_CPUTIMES, ps);
}

//device driver

//where device_no is Major device number
int sysopen(int device_no){
	if(device_no != KEYBOARD_NOECHO && device_no != KEYBOARD_ECHO){
		kprintf("sysopen error\n");
		return -1;
	}
	return syscall(SYS_OPEN, device_no);
}

int sysclose(int fd){
	return syscall(SYS_CLOSE, fd);
}


extern int syswrite(int fd, void *buff, int bufflen){
	if(fd < 0 || fd > 3){
		kprintf("syswrite error\n");
		return -1;
	}
	return syscall(SYS_WRITE, fd, buff, bufflen);
}
extern int sysread(int fd, void *buff, int bufflen){
	if(fd < 0 || fd > 3){
		kprintf("sysread error\n");
		return -1;
	}
	//kprintf("SYSREAD ");
	return syscall(SYS_READ, fd, buff, bufflen);
}
extern int sysioctl(int fd, unsigned long command, ...){
	va_list args;
	va_start(args, command);
	int EOFChar = va_arg(args, unsigned long);
	return syscall(SYS_IOCTL, fd, command, EOFChar);
}
