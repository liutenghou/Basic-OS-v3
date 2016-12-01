#include <kbd.h>
#include <xeroskernel.h>



//initialize device_table
int keyboardinit(void){
	device_table[KEYBOARD_NOECHO].dvopen = &kbd_open_noecho;
	device_table[KEYBOARD_NOECHO].dvclose = &kbd_close;
	device_table[KEYBOARD_NOECHO].dvread = &kbd0_read;
	device_table[KEYBOARD_NOECHO].dvwrite = &kbd_write;
	device_table[KEYBOARD_NOECHO].dviotcl = &kbd_iotcl;

	device_table[KEYBOARD_ECHO].dvopen = &kbd_open_echo;
	device_table[KEYBOARD_ECHO].dvclose = &kbd_close;
	device_table[KEYBOARD_ECHO].dvread = &kbd1_read;
	device_table[KEYBOARD_ECHO].dvwrite = &kbd_write;
	device_table[KEYBOARD_ECHO].dviotcl = &kbd_iotcl;

	keyboardEchoOn = 0;
	return 101;
}


int kbd_open_echo(){
//	kprintf("IN KBD OPEN ");
	keyboardEchoOn = 1;

	//irq for keyboard controller is 1
	//enables interrupts form keyboard
	//args(irq, diable)
	enable_irq(1,0);

	return 0;
}

int kbd_open_noecho(){
//	kprintf("IN KBD OPEN ");
	keyboardEchoOn = 0;

	//irq for keyboard controller is 1
	//enables interrupts form keyboard
	//args(irq, diable)
	enable_irq(1,0);

	return 0;
}

int kbd_close(){
	enable_irq(1,1);
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

//cannot write to keyboard, always return -1
int kbd_write(void *buff, int bufflen){
	//kprintf(" DVWRITE ");
	return -1;
}


int kbd_iotcl(unsigned long command, int EOFChar){
	switch(command) {

	   case (CHANGE_EOF): //change character for EOF
		   //with int value of command to become new EOF
		  kprintf("EOFChar:%d ", EOFChar);
	      break;
	   case (ECHOOFF): //turn echoing off
		   keyboardEchoOn = 0;
	      break;

	   case (ECHOON): //turn echoing on
		   keyboardEchoOn = 1;
		   break;
	   default :
		   ;
	}
	return 0;
}

//given code from scancodesToAscii.txt

//changed extchar to ANSI compliant syntax, more readable
static int extchar(unsigned char code)
{
	state = state & ~EXTENDED;
	return state;
}

//HOW DOES THIS EVEN COMPILE???
//static int
//extchar(code)
//unsigned char   code;
//{
//        state &= ~EXTENDED;
//}

//converts keyboard interrupt code to ASCII
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
      //kprintf("Capslock off detected\n");
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
    //kprintf("shift detected!\n");
    return NOCHAR;
  case CAPSL:
    state |= CAPSLOCK;
    //kprintf("Capslock ON detected!\n");
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

