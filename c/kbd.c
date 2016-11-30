#include <kbd.h>
#include <xeroskernel.h>


//initialize device_table
int keyboardinit(void){
	device_table[KEYBOARD_NOECHO].dvopen = &kbd_open;
	device_table[KEYBOARD_NOECHO].dvclose = &kbd_close;
	device_table[KEYBOARD_NOECHO].dvread = &kbd0_read;
	device_table[KEYBOARD_NOECHO].dvwrite = &kbd_write;
	device_table[KEYBOARD_NOECHO].dviotcl = &kbd_iotcl;

	device_table[KEYBOARD_ECHO].dvopen = &kbd_open;
	device_table[KEYBOARD_ECHO].dvclose = &kbd_close;
	device_table[KEYBOARD_ECHO].dvread = &kbd1_read;
	device_table[KEYBOARD_ECHO].dvwrite = &kbd_write;
	device_table[KEYBOARD_ECHO].dviotcl = &kbd_iotcl;

	return 101;
}


int kbd_open(){
	kprintf("IN KBD OPEN ");

	//read from port 0x60

	//control information read/written to port 0x64
	//check low order bit is 1, if it is, then there is data to be read from 0x60




	//irq for keyboard controller is 1
	//enables interrupts form keyboard
	enable_irq(1,0);

	//todo: install ISR

	//todo: convert scan codes to ASCII, two: one down, one up

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

