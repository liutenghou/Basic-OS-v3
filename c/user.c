/* user.c : User processes
 */

#include <xeroskernel.h>
#include <xeroslib.h>

/*
int proc_pid, con_pid;
void busy( void ) {
  int myPid;
  char buff[100];
  int i;
  int count = 0;

  myPid = sysgetpid();
  
  for (i = 0; i < 10; i++) {
    sprintf(buff, "busy pid is %d\n", myPid);
    sysputs(buff);
    if (myPid == 2 && count == 1) syskill(3);
    count++;
    sysyield();
  }
}



void sleep1( void ) {
  int myPid;
  char buff[100];

  myPid = sysgetpid();
  sprintf(buff, "Sleeping 1000 is %d\n", myPid);
  sysputs(buff);
  syssleep(1000);
  sprintf(buff, "Awoke 1000 from my nap %d\n", myPid);
  sysputs(buff);
}



void sleep2( void ) {
  int myPid;
  char buff[100];

  myPid = sysgetpid();
  sprintf(buff, "Sleeping 2000 pid is %d\n", myPid);
  sysputs(buff);
  syssleep(2000);
  sprintf(buff, "Awoke 2000 from my nap %d\n", myPid);
  sysputs(buff);
}



void sleep3( void ) {
  int myPid;
  char buff[100];

  myPid = sysgetpid();
  sprintf(buff, "Sleeping 3000 pid is %d\n", myPid);
  sysputs(buff);
  syssleep(3000);
  sprintf(buff, "Awoke 3000 from my nap %d\n", myPid);
  sysputs(buff);
}






void producer( void ) {


    int         i;
    char        buff[100];


    // Sping to get some cpu time
    for(i = 0; i < 100000; i++);

    syssleep(3000);
    for( i = 0; i < 20; i++ ) {
      
      sprintf(buff, "Producer %x and in hex %x %d\n", i+1, i, i+1);
      sysputs(buff);

      syssleep(1500);

    }
    for (i = 0; i < 15; i++) {
      sysputs("P");
      syssleep(1500);
    }
    sprintf(buff, "Producer %x and in hex %x %d\n", i+1, i, i+1);
    sysputs(buff);
    sprintf(buff, "Producer finished\n");
    sysputs( buff );
    sysstop();
}

void consumer( void ) {

    int         i;
    char        buff[100];

    for(i = 0; i < 50000; i++);
    syssleep(3000);
    for( i = 0; i < 10; i++ ) {
      sprintf(buff, "Consumer %d\n", i);
      sysputs( buff );
      syssleep(1500);
      sysyield();
    }

    for (i = 0; i < 40; i++) {
      sysputs("C");
      syssleep(700);
    }

    sprintf(buff, "Consumer %d\n", i);
    sysputs( buff );
    sprintf(buff, "Consumer finished\n");
    sysputs( buff );
    sysstop();
}
*/

void t(void){
	while(1){
		sysputs("T\n");
		syssleep(10000);
	}
	sysstop();
}


void shell(void){
	char cmdBuffer[100];
	cmdBuffer[0]=0;
	char currentCommand[100];
	currentCommand[0]=0;
	char cmd[3];
	char option[3];
	processStatuses pstat;

/* note:
	#define STATE_READY     1
	#define STATE_SLEEP     22
	#define STATE_RUNNING   23
*/
	char *stateString;

	int fd_keyboard_Echo = sysopen(KEYBOARD_ECHO);
	kprintf("fd_keybaordEcho:%d\n", fd_keyboard_Echo);
	while(strcmp(currentCommand, "ex")!=0){
		memset(cmd, '\0', sizeof(cmd));
		memset(option, '\0', sizeof(option));

		sysputs(">");
		sysread(fd_keyboard_Echo, cmdBuffer, sizeof(cmdBuffer));
		//note strcpy(dest, source)
		strcpy(currentCommand, cmdBuffer);
		memset(cmdBuffer, '\0', sizeof(cmdBuffer));
		//kprintf("cmdBuffer:%s, currentCommand:%s \n", cmdBuffer, currentCommand);
		//todo: parse first word for command
		//actually will hardcode this

		//convert state to something human readable
		if(strcmp(currentCommand, "ps") == 0){
			sysputs("PID	STATE	TIME_ALIVE\n");
			sysgetcputimes(&pstat);
			int i;
			for(i=0; i<sizeof(pstat.pid);i++){
				if((pstat.pid[i]>=0)&&((pstat.pid[i] != 0)||(pstat.status[i]==STATE_READY))){ //lots of pid 0s, not sure why
					switch(pstat.status[i]){

						case(1):
								stateString = "STATE_READY";
						break;
						case(22):
								stateString = "STATE_SLEEP";
						break;
						case(23):
								stateString = "STATE_RUNNING";
						break;
					}

					kprintf("%d	%s	%d\n",pstat.pid[i], stateString, pstat.cpuTime[i]);
				}
			}
		}else if(strcmp(currentCommand, "ex")==0){
			break;
		}else if(currentCommand[0] == 'k' && currentCommand[1] == ' '){
			sysputs("cmd:k ");
			//TODO: implement
			//syskill(1);
		}else if((strlen(currentCommand)==1 && currentCommand[0] == 'a')||(currentCommand[0] == 't' && currentCommand[1] == ' ')){
			sysputs("cmd:a ");
		}else if((strlen(currentCommand)==1 && currentCommand[0] == 't')||(currentCommand[0] == 't' && currentCommand[1] == ' ')){
			//sysputs("cmd:t ");
			int t_pid = syscreate(&t, 1024);
		}
	}
	sysclose(fd_keyboard_Echo);
	sysstop();


}

void     root( void ) {
/****************************/

    char  UNBuff[100];
    char PWBuff[100];
    int successfulLogin = 0;

    sysputs("Welcome to Xeros - an experimental OS\n");


	while(!successfulLogin){
		memset(UNBuff, '\0', sizeof(UNBuff));
		memset(PWBuff, '\0', sizeof(UNBuff));

		int fd_keyboardNoEcho = sysopen(KEYBOARD_NOECHO); //opens keyboard
		//kprintf("filesdescriptor:%d \n", fd_keyboardNoEcho);
		sysioctl(fd_keyboardNoEcho, ECHOON);


		//close no echo keyboard
		//turn echo on in no echo keyboard
		sysputs("Username:");
		sysread(fd_keyboardNoEcho, UNBuff, sizeof(UNBuff));

		sysioctl(fd_keyboardNoEcho, ECHOOFF);
		sysputs("Password:");
		sysread(fd_keyboardNoEcho, PWBuff, sizeof(PWBuff));
		//sysputs(PWBuff);
		sysclose(fd_keyboardNoEcho); //closes keyboard
		if((strcmp(UNBuff,"cs415")==0) && (strcmp(PWBuff, "EveryoneGetsAnA")==0)){
			sysputs("\nLogin Successful\n");
			successfulLogin = 1;
		}else{
			sysputs(PWBuff); //check
			sysputs("\nInvalid Credentials\n");
		}
	}

	//create the shell program
	int shellPID = syscreate(&shell, 1024);
//		sysyield();





//    sysputs("buff: ");
//    sysputs(buff);



    //TODO: open echo keyboard
//    int fd_keyboardecho = sysopen(KEYBOARD_ECHO);
//	kprintf("filedescriptor:%d \n", fd_keyboardecho);
	//sysclose(KEYBOARD_ECHO);
	//syssleep(5000);



    // Test for ready queue removal. 
   
//    proc_pid = syscreate(&busy, 1024);
//    con_pid = syscreate(&busy, 1024);





    /*
    syssighandler(12, &producer, 0);
    sysyield();
    syskill(proc_pid);
    sysyield();
    syskill(con_pid);


    for(i = 0; i < 5; i++) {
      pids[i] = syscreate(&busy, 1024);
    }

    sysyield();
    
    syskill(pids[3]);
    sysyield();
    syskill(pids[2]);
    syskill(pids[4]);
    sysyield();
    syskill(pids[0]);
    sysyield();
    syskill(pids[1]);
    sysyield();

    syssleep(8000);;



    kprintf("***********Sleeping no kills *****\n");
    // Now test for sleeping processes
    pids[0] = syscreate(&sleep1, 1024);
    pids[1] = syscreate(&sleep2, 1024);
    pids[2] = syscreate(&sleep3, 1024);

    sysyield();
    syssleep(8000);;



    kprintf("***********Sleeping kill 2000 *****\n");
    // Now test for removing middle sleeping processes
    pids[0] = syscreate(&sleep1, 1024);
    pids[1] = syscreate(&sleep2, 1024);
    pids[2] = syscreate(&sleep3, 1024);

    syssleep(110);
    syskill(pids[1]);
    syssleep(8000);;

    kprintf("***********Sleeping kill last 3000 *****\n");
    // Now test for removing last sleeping processes
    pids[0] = syscreate(&sleep1, 1024);
    pids[1] = syscreate(&sleep2, 1024);
    pids[2] = syscreate(&sleep3, 1024);

    sysyield();
    syskill(pids[2]);
    syssleep(8000);;

    kprintf("***********Sleeping kill first process 1000*****\n");
    // Now test for first sleeping processes
    pids[0] = syscreate(&sleep1, 1024);
    pids[1] = syscreate(&sleep2, 1024);
    pids[2] = syscreate(&sleep3, 1024);

    syssleep(100);
    syskill(pids[0]);
    syssleep(8000);;

    // Now test for 1 process


    kprintf("***********One sleeping process, killed***\n");
    pids[0] = syscreate(&sleep2, 1024);

    sysyield();
    syskill(pids[0]);
    syssleep(8000);;

    kprintf("***********One sleeping process, not killed***\n");
    pids[0] = syscreate(&sleep2, 1024);

    sysyield();
    syssleep(8000);;



    kprintf("***********Three sleeping processes***\n");    // 
    pids[0] = syscreate(&sleep1, 1024);
    pids[1] = syscreate(&sleep2, 1024);
    pids[2] = syscreate(&sleep3, 1024);


    // Producer and consumer started too
    proc_pid = syscreate( &producer, 4096 );
    con_pid = syscreate( &consumer, 4096 );
    sprintf(buff, "Proc pid = %d Con pid = %d\n", proc_pid, con_pid);
    sysputs( buff );


    processStatuses psTab;
    int procs;
    



    syssleep(500);
    procs = sysgetcputimes(&psTab);

    for(int j = 0; j <= procs; j++) {
      sprintf(buff, "%4d    %4d    %10d\n", psTab.pid[j], psTab.status[j], 
	      psTab.cpuTime[j]);
      kprintf(buff);
    }


    syssleep(10000);
    procs = sysgetcputimes(&psTab);

    for(int j = 0; j <= procs; j++) {
      sprintf(buff, "%4d    %4d    %10d\n", psTab.pid[j], psTab.status[j], 
	      psTab.cpuTime[j]);
      kprintf(buff);
    }

    sprintf(buff, "Root finished\n");
    sysputs( buff );

    */

    sysstop();
    
    for( ;; ) {
     sysyield();
    }
    
}

