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

//given code

//the heck does this do?
//unsigned char   code;
//static int extchar(unsigned char   code)
//{
//        return state &= ~EXTENDED;
//}

//HOW DOES THIS EVEN COMPILE???
static int
extchar(code)
unsigned char   code;
{
        state &= ~EXTENDED;
}

unsigned int kbtoa( unsigned char code )
{
  unsigned int    ch;

  if (state & EXTENDED)
    return extchar(code);
  if (code & KEY_UP) {
    switch (code & 0x7f) {
    case LSHIFT:
    case RSHIFT:
      state &= ~INSHIFT;
      break;
    case CAPSL:
      kprintf("Capslock off detected\n");
      state &= ~CAPSLOCK;
      break;
    case LCTL:
      state &= ~INCTL;
      break;
    case LMETA:
      state &= ~INMETA;
      break;
    }

    return NOCHAR;
  }


  /* check for special keys */
  switch (code) {
  case LSHIFT:
  case RSHIFT:
    state |= INSHIFT;
    kprintf("shift detected!\n");
    return NOCHAR;
  case CAPSL:
    state |= CAPSLOCK;
    kprintf("Capslock ON detected!\n");
    return NOCHAR;
  case LCTL:
    state |= INCTL;
    return NOCHAR;
  case LMETA:
    state |= INMETA;
    return NOCHAR;
  case EXTESC:
    state |= EXTENDED;
    return NOCHAR;
  }

  ch = NOCHAR;

  if (code < sizeof(kbcode)){
    if ( state & CAPSLOCK )
      ch = kbshift[code];
	  else
	    ch = kbcode[code];
  }
  if (state & INSHIFT) {
    if (code >= sizeof(kbshift))
      return NOCHAR;
    if ( state & CAPSLOCK )
      ch = kbcode[code];
    else
      ch = kbshift[code];
  }
  if (state & INCTL) {
    if (code >= sizeof(kbctl))
      return NOCHAR;
    ch = kbctl[code];
  }
  if (state & INMETA)
    ch += 0x80;
  return ch;
}

