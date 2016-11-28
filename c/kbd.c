#include <kbd.h>

int kbd_open(){
	return 0;
}

int kbd_close(){
	return 0;

}

int kbd_read(){
	return 0;
}

int kbd_write(void){
	kprintf(" DVWRITE ");
	return -1;
}

int kbd_iotcl(){

	return 0;
}

