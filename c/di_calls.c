#include <xeroskernel.h>
#include <xeroslib.h>

//for device calls
//for all below, must call corresponding dv functions

//device drivers
int keyboardOpen = 0;

//argument: devNo: major device number, index into device table

//todo: check what to do if one keyboard already open
int di_open(int devNo) {
	if(keyboardOpen == 1){
		kprintf("ERROR di_open: Keyboard Already Open\n");
		return -2;
	}
	keyboardOpen = 1;
	//kprintf("DI_OPEN:%d ", devNo);

	//check that devNo is in range
	//returns -1 if open fail
	if (devNo < 0 || devNo >= MAX_DEVICES) {
		return -1;
	}

	pcb *p = getCurrentProcess();
	int i;
	for (i = 0; i < MAX_DEVICES_PER_PROCESS; i++) {


		if (p->fdt[i] == NULL_POINTER) {
			p->fdt[i] = &device_table[devNo];
			p->fdt[i]->dvopen();
			return i; //returns filedescriptor 0-3 if success
		}
	}
	return -1;
}

int di_close(int fd) {
	keyboardOpen = 0;

	if (fd < 0 || fd > MAX_DEVICES_PER_PROCESS) {
		return -1;
	}

	pcb *p = getCurrentProcess();
	//if fd is already closed return failure
	if (p->fdt[fd] == NULL_POINTER) {
		return -2; //alread closed
	} else {
		int ret = -1;
		ret = p->fdt[fd]->dvclose();
		p->fdt[fd] = NULL_POINTER;
		return ret;
	}

	//other error
	return -3;
}

int di_write(int fd, void *buff, int bufflen) {
	//note: for keyboard this will always just return -1

	if (fd < 0 || fd > MAX_DEVICES_PER_PROCESS) {
		return -1;
	}
	pcb *p = getCurrentProcess();
	if (p->fdt[fd] == NULL_POINTER) {
		return -1;
	}

	return p->fdt[fd]->dvwrite(buff, bufflen);
}

int di_read(int fd, void *buff, int bufflen) {
	if (fd < 0 || fd > MAX_DEVICES_PER_PROCESS) {
		return -1;
	}
	pcb *p = getCurrentProcess();
	if (p->fdt[fd] == NULL_POINTER) {
		return -1;
	}

	return p->fdt[fd]->dvread(buff, bufflen);

}

int di_ioctl(int fd, unsigned long command, int EOFChar) {
	if ((fd < 0 || fd > MAX_DEVICES_PER_PROCESS)||(command < 0)) {
		kprintf("ioctl invalid arguments\n");
		return -1;
	}
	pcb *p = getCurrentProcess();
	if (p->fdt[fd] == NULL_POINTER) {
		return -1;
	}

	return p->fdt[fd]->dviotcl(command, EOFChar);

}

