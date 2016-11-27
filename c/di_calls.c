#include <xeroskernel.h>
#include <xeroslib.h>

//for device calls
//TODO: for all below, must call corresponding dv functions

//device drivers
//argument: devNo: major device number, index into device table
int di_open(int devNo) {
	kprintf("DI_OPEN ");

	//check that devNo is in range
	//returns -1 if open fail
	if(devNo<0 || devNo >= MAX_DEVICES){
		return -1;
	}

	pcb *p = getCurrentProcess();
	int i;
	for(i=0; i<MAX_DEVICES_PER_PROCESS; i++){
		if(p->fdt[i] == 0){
			p->fdt[i] = device_table[i];
			p->fdt[i].dvopen();
			return i; //returns filedescriptor 0-3 if success
		}
	}
	return -1;
}

int di_close(int fd) {


	if(fd<0 || fd>MAX_DEVICES_PER_PROCESS){
		return -1;
	}

	pcb *p = getCurrentProcess();
	//if fd is already closed return failure
	if(p->fdt[fd] == 0){
		return -2; //alread closed
	}else{
		p->fdt[fd].dvclose();
		p->fdt[fd] = 0;
		return 0;
	}

	//other error
	return -3;
}

int di_write(int fd, void *buff, int bufflen){

	return 0;

}

int di_read(int fd, void *buff, int bufflen){

	return 0;
}

int di_ioctl(int fd, unsigned long command, ...){

	return 0;
}






