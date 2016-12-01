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
/*********************************************/

    return( syscall( SYS_CREATE, fp, stack ) );
}

void sysyield( void ) {
/***************************/
  syscall( SYS_YIELD );
}

 void sysstop( void ) {
/**************************/

   syscall( SYS_STOP );
}

unsigned int sysgetpid( void ) {
/****************************/

    return( syscall( SYS_GETPID ) );
}

void sysputs( char *str ) {
/********************************/

    syscall( SYS_PUTS, str );
}

unsigned int syssleep( unsigned int t ) {
/*****************************/

    return syscall( SYS_SLEEP, t );
}



int syssighandler(int signal, void (*newhandler)(void *), void (** oldHandler)(void *)){
	return syscall(SYS_SIGHANDLER, signal, newhandler, oldHandler);
}

int syskill(int pid) {
  return syscall(SYS_KILL, pid);
}

int sysgetcputimes(processStatuses *ps) {
  return syscall(SYS_CPUTIMES, ps);
}

//device driver

//where device_no is Major device number
int sysopen(int device_no){
	return syscall(SYS_OPEN, device_no);
}

int sysclose(int fd){
	return syscall(SYS_CLOSE, fd);
}


extern int syswrite(int fd, void *buff, int bufflen){
	return syscall(SYS_WRITE, fd, buff, bufflen);
}
extern int sysread(int fd, void *buff, int bufflen){
	kprintf("SYSREAD ");
	return syscall(SYS_READ, fd, buff, bufflen);
}
extern int sysioctl(int fd, unsigned long command, ...){
	va_list args;
	va_start(args, command);
	int EOFChar = va_arg(args, unsigned long);
	return syscall(SYS_IOCTL, fd, command, EOFChar);
}
