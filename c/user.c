/* user.c : User processes
 */

#include <xeroskernel.h>
#include <xeroslib.h>

void t(void);
void shell(void);
void alarm(void);

void alarm(void){
	sysputs("ALARM ALARM ALARM\n");
	//disables signal 15

}

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
			sysputs("PID	STATE		TIME_ALIVE\n");
			sysgetcputimes(&pstat);
			int i;
			for(i=0; i<sizeof(pstat.pid);i++){
				if((pstat.pid[i]>=0)&&((pstat.pid[i] != 0)||(pstat.status[i]==STATE_READY))){ //lots of pid 0s, not sure why
					switch(pstat.status[i]){

						case(1):
							stateString = "STATE_READY";
							break;
						case(2):
							stateString = "STATE_SIGWAIT";
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



    sysstop();
    
    for( ;; ) {
     sysyield();
    }
    
}

