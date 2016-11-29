#include <kbd.h>
#include <xeroskernel.h>


//initialize device_table
int keyboardinit(void){
	device_table[KEYBOARD0].dvopen = &kbd_open;
	device_table[KEYBOARD0].dvclose = &kbd_close;
	device_table[KEYBOARD0].dvread = &kbd0_read;
	device_table[KEYBOARD0].dvwrite = &kbd_write;
	device_table[KEYBOARD0].dviotcl = &kbd_iotcl;

	device_table[KEYBOARD1].dvopen = &kbd_open;
	device_table[KEYBOARD1].dvclose = &kbd_close;
	device_table[KEYBOARD1].dvread = &kbd1_read;
	device_table[KEYBOARD1].dvwrite = &kbd_write;
	device_table[KEYBOARD1].dviotcl = &kbd_iotcl;

	return 0;
}


int kbd_open(){
	return 0;
}

int kbd_close(){
	return 0;

}

//no echo
int kbd0_read(void *buff, int bufflen){
	return 0;
}

//echos
int kbd1_read(void *buff, int bufflen){
	return 0;
}

int kbd_write(void *buff, int bufflen){
	kprintf(" DVWRITE ");
	return -1;
}

int kbd_iotcl(unsigned long command){

	return 0;
}

