#include <xeroskernel.h>
#include <xeroslib.h>

//for device calls

//device drivers
//argument: devNo: major device number, index into device table
int di_open(int devNo) {
	kprintf("DI_OPEN ");

	//returns -1 if open fail

	//returns filedescriptor 0-3 if success
	return 0;
}

int di_close(int fd) {

	//if fd is already closed return failure

	//else return success
	return 0;
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






