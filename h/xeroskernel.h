/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max */

#ifndef XEROSKERNEL_H
#define XEROSKERNEL_H

/* make sure interrupts are armed later on in the kernel development  */
#define STARTING_EFLAGS         0x00003000
#define ARM_INTERRUPTS          0x00000200

/* Symbolic constants used throughout Xinu */

typedef char Bool; /* Boolean type                  */
typedef unsigned int size_t; /* Something that can hold the value of
 * theoretical maximum number of bytes
 * addressable in this architecture.
 */
#define	FALSE   0       /* Boolean constants             */
#define	TRUE    1
#define	EMPTY   (-1)    /* an illegal gpq                */
#define	NULL    0       /* Null pointer for linked lists */
#define	NULLCH '\0'     /* The null character            */
#define NULL_POINTER	((void*)0)

#define CREATE_FAILURE -1  /* Process creation failed     */

/* Universal return constants */

#define	OK            1         /* system call ok               */
#define	SYSERR       -1         /* system call failed           */
#define	EOF          -2         /* End-of-file (usu. from read)	*/
#define	TIMEOUT      -3         /* time out  (usu. recvtim)     */
#define	INTRMSG      -4         /* keyboard "intr" key pressed	*/
/*  (usu. defined as ^B)        */
#define	BLOCKERR     -5         /* non-blocking op would block  */

/* Functions defined by startup code */

void bzero(void *base, int cnt);
void bcopy(const void *src, void *dest, unsigned int n);
void disable(void);
unsigned short getCS(void);
unsigned char inb(unsigned int);
void init8259(void);
int kprintf(char * fmt, ...);
void lidt(void);
void outb(unsigned int, unsigned char);

/* Some constants involved with process creation and managment */

/* Maximum number of processes */
#define MAX_PROC        64           
/* Kernel trap number          */
#define KERNEL_INT      80
/* Interrupt number for the timer */
#define TIMER_INT      (TIMER_IRQ + 32)
/* Minimum size of a stack when a process is created */
#define PROC_STACK      (4096 * 4)    
/* Number of milliseconds in a tick */
#define MILLISECONDS_TICK 10

//A3
#define MAX_SIGNALS	32
#define MAX_DEVICES 32
#define MAX_DEVICES_PER_PROCESS 4

/* Constants to track states that a process is in */
#define STATE_STOPPED   0
#define STATE_READY     1
#define STATE_SIGWAIT	2
#define STATE_SLEEP     22
#define STATE_RUNNING   23

/* System call identifiers */
#define SYS_STOP        10
#define SYS_YIELD       11
#define SYS_CREATE      22
#define SYS_TIMER       33
#define SYS_GETPID      144
#define SYS_PUTS        155
#define SYS_SLEEP       166
#define SYS_KILL        177
#define SYS_CPUTIMES    178
#define SYS_OPEN		179
#define SYS_CLOSE		180
#define SYS_WRITE		181
#define SYS_READ		182
#define SYS_IOCTL		183
//signals
#define SYS_SIGHANDLER	184
#define SYS_KEYBOARD	185
#define SYS_SIGRETURN	186
#define SYS_WAIT		187

/* A typedef for the signature of the function passed to syscreate */
typedef void (*funcptr)(void);
typedef void (*funcptr_signal)(void*);

//pointers to functions for device
typedef struct devsw {
	int dvnum;
	char *dvname;
	int (*dvinit)(void);
	int (*dvopen)(void);
	int (*dvclose)(void);
	int (*dvread)(void* buff, int bufflen);
	int (*dvwrite)(void* buff, int bufflen);
	int (*dvseek)(void);
	int (*dvgetc)(void);
	int (*dvputc)(void);
	int (*dvcntl)(void);
	void *dvcsr;
	void *dvivec;
	void *dvovec;
	int (*dviint)(void);
	int (*dvoint)(void);
	void *dvioblk;
	int dvminor;
	int (*dviotcl)(unsigned long command, int ChangeEOF);
} device;

/* Structure to track the information associated with a single process */
typedef struct struct_pcb pcb;
struct struct_pcb {
	void *esp; /* Pointer to top of saved stack           */
	pcb *next; /* Next process in the list, if applicable */
	pcb *prev; /* Previous proccess in list, if applicable*/
	int state; /* State the process is in, see above      */
	unsigned int pid; /* The process's ID                        */
	int ret; /* Return value of system call             */
	/* if process interrupted because of system*/
	/* call                                    */
	long args;
	int validProcess;
	unsigned int otherpid;
	void *buffer;
	int bufferlen;
	int sleepdiff;
	unsigned int tickCount;
	long cpuTime; /* CPU time  consumed                    */

	//signals, ordered by priority, actual value will be the handler for the signal
	funcptr_signal signaltable[MAX_SIGNALS]; //signal table, for what to do when a signal is sent to the process
	//each pcb has a fixed sized file descriptor table
	//open returns entry in fdt
	device* fdt[MAX_DEVICES_PER_PROCESS];
};

typedef struct struct_ps processStatuses;
struct struct_ps {
	int pid[MAX_PROC];      // The process ID
	int status[MAX_PROC];   // The process status
	long cpuTime[MAX_PROC]; // CPU time used in milliseconds
};

//populated in keyboardinit()
device device_table[MAX_DEVICES];

//only one keyboard allowed to be open at once
#define KEYBOARD_NOECHO    	0 //no echo, process will have to display characters
#define KEYBOARD_ECHO    	1 //echos before process accesses char
#define KBMON      	 		2 //krab monster
#define TTY0         		3 //talk to you never
#define IRQ_NUMBER			1
#define KEYBOARD_INT		32+IRQ_NUMBER

/* The actual space is set aside in create.c */
extern pcb proctab[MAX_PROC];

#pragma pack(1)

/* What the set of pushed registers looks like on the stack */
typedef struct context_frame {
	unsigned long edi;
	unsigned long esi;
	unsigned long ebp;
	unsigned long esp;
	unsigned long ebx;
	unsigned long edx;
	unsigned long ecx;
	unsigned long eax;
	unsigned long iret_eip;
	unsigned long iret_cs;
	unsigned long eflags;
	unsigned long stackSlots[];
} context_frame;

/* Memory mangement system functions, it is OK for user level   */
/* processes to call these.                                     */

void kfree(void *ptr);
void kmeminit(void);
void *kmalloc(size_t);

/* Internal functions for the kernel, applications must never  */
/* call these.                                                 */
extern pcb *findPCB(int pid);
void dispatch(void);
void dispatchinit(void);
void ready(pcb *p);
pcb *next(void);
void contextinit(void);
int keyboardinit(void);
int contextswitch(pcb *p);
int create(funcptr fp, size_t stack);
void set_evec(unsigned int xnum, unsigned long handler);
void printCF(void * stack); /* print the call frame */
int syscall(int call, ...); /* Used in the system call stub */
void sleep(pcb *, unsigned int);
void removeFromSleep(pcb * p);
void tick(void);
pcb* getCurrentProcess(void);
int getIdlePID(void);
int getCPUtimes(pcb * p, processStatuses *ps);
//device drivers kernel
int di_open(int devNo);
int di_close(int fd);
int di_write(int fd, void *buff, int bufflen);
int di_read(int fd, void *buff, int bufflen);
int di_ioctl(int fd, unsigned long command, int EOFChar);
pcb* getReadingProcess(void);

//signals kernel side
int sighandler(int signal, void (*newhandler)(void *),
		void (**oldHandler)(void *));
void sigreturn(void *old_sp);
int syskill(int pcb, int signalNumber);
int kill(pcb *currP, int pid, int signalNumber);
void sigtramp(void (*handler)(void*), void *cntx);
int wait(int PID);
int signal(int dest_pid, int sig_no);
int syswait(int PID);

/* Function prototypes for system calls as called by the application */
int syscreate(funcptr fp, size_t stack);
void sysyield(void);
void sysstop(void);
unsigned int sysgetpid(void);
unsigned int syssleep(unsigned int);
void sysputs(char *str);
int syssighandler(int signal, void (*newhandler)(void *),void (**oldHandler)(void *));
void sysputs(char *str);
int sysgetcputimes(processStatuses *ps);
//device drivers syscall
extern int sysopen(int device_no);
int sysclose(int fd);
extern int syswrite(int fd, void *buff, int bufflen);
extern int sysread(int fd, void *buff, int bufflen);
extern int sysioctl(int fd, unsigned long command, ...);
int keyboard(void);
unsigned int kbtoa( unsigned char code );
int keyboardEchoOn;
void keyboard_print(void);
//keyboard ioctl stuff
#define CHANGE_EOF 	53
#define ECHOOFF		55
#define ECHOON		56


/* The initial process that the system creates and schedules */
void root(void);

void set_evec(unsigned int xnum, unsigned long handler);

/* Anything you add must be between the #define and this comment */
#endif

