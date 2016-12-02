#include <kbd.h>
#include <xeroskernel.h>

#define INTERNAL_BUFFER_LENGTH 4
#define MAX_APP_BUFFER_LENGTH 1000 //need to find out what this number should be
//char buff[4]; //I don't know why this is necessary
int isReading;
int isOpen;
char *appBuffer;
int appBufferLength;
char EOF_char_internal = (char)CONTROL_D;

char internalBuffer[INTERNAL_BUFFER_LENGTH];
unsigned int lengthCount;

//initialize device_table
int keyboardinit(void) {
	device_table[KEYBOARD_NOECHO].dvopen = &kbd_open_noecho;
	device_table[KEYBOARD_NOECHO].dvclose = &kbd_close;
	device_table[KEYBOARD_NOECHO].dvread = &kbd_read;
	device_table[KEYBOARD_NOECHO].dvwrite = &kbd_write;
	device_table[KEYBOARD_NOECHO].dviotcl = &kbd_iotcl;

	device_table[KEYBOARD_ECHO].dvopen = &kbd_open_echo;
	device_table[KEYBOARD_ECHO].dvclose = &kbd_close;
	device_table[KEYBOARD_ECHO].dvread = &kbd_read;
	device_table[KEYBOARD_ECHO].dvwrite = &kbd_write;
	device_table[KEYBOARD_ECHO].dviotcl = &kbd_iotcl;

	keyboardEchoOn = 0;
	//memset(buff, 0, sizeof(buff));
	isReading = 0;
	isOpen = 0;
	internalBuffer[0] = 0;
	lengthCount = 0;


	return 101;
}

void resetKeyboard(void){
	//memset(buff, 0, sizeof(buff));
	isReading = 0;
	internalBuffer[0] = 0;
	lengthCount = 0;
	isOpen = 0;
}

//copies data from one buffer to the other
void copyBuffer(char* sourceBuffer, char* destinationBuffer){
	int shorterLength;
	if(sizeof(sourceBuffer)<sizeof(destinationBuffer)){
		shorterLength = sizeof(sourceBuffer);
	}else{
		shorterLength = sizeof(destinationBuffer);
	}
	int i;
	for(i=0; i<shorterLength; i++){
		destinationBuffer[i] = sourceBuffer[i];
	}
}

//concatenates char from keyboard to application buffer
int addCharToAppBuffer(char c){
	//find where there is empty space in appBuffer
	int startIndex = 0;
	int i;
	for(i=0; i<appBufferLength; i++){
		if(appBuffer[i] == 0){
			startIndex = i;

			appBuffer[i] = c;

			return startIndex;
		}
	}



	kprintf("ERROR: app buffer full ");
	return -1; //is full

}

//for adding chars to the internal buffer, when open but not reading
int addCharToInternalBuffer(char c){
	//find where there is empty space in appBuffer
	int startIndex = 0;
	int i;
	for(i=0; i<INTERNAL_BUFFER_LENGTH; i++){
		if(internalBuffer[i] == 0){
			startIndex = i;

			internalBuffer[i] = c;

			return startIndex;
		}
	}

	return 1; //is full

}


//TODO: only one keyboard allowed to be opened at a time
void keyboard_print(void) {
	//read inb(ADDR) & outb(ADDR, val)
	//control information read/written to port 0x64
	//check low order bit is 1, if it is, then there is data to be read from 0x60
	if (inb(0x64)) { //checks 0x64 and if echo is on/off
		//note that two of these interrupts come with a keypress
		//the down and release
		unsigned char c = inb(0x60);
		unsigned char c_actual = kbtoa(c);
		if(keyboardEchoOn){
			//kprintf("kb:%x\n",c_actual); //this prints the hex code for the keyboard input
			if((int)c_actual != 4){ //no control-d printing
				//kprintf("<%c,%d>", c_actual, c_actual); //for testing
				kprintf("%c", c_actual);

			}

		}

		//save chars to internal buffer, do this if open, but not reading
		if(isOpen && (isReading==0)){
			//if enter or control-d pressed, done
			if((int)c_actual == ENTER){
				//kprintf("ENTER ");

				//copyBuffer(internalBuffer, appBuffer);
				//todo:if enter is pressed, also add \n to output
				addCharToAppBuffer("\\n"); //not sure about this
				ready(getReadingProcess());

				resetKeyboard();
				kbd_close();
			}else if((int)c_actual == EOF_char_internal){
				//close the keyboard
				//kprintf("CONTROL_D ");

				//copyBuffer(internalBuffer, appBuffer);
				ready(getReadingProcess());

				resetKeyboard();
				kbd_close();
			}else{
				//save keystrokes to app buffer
				if((int)c_actual != 4){ //no control-d printing


					addCharToInternalBuffer(c_actual);
				}
			}
		}

		//-----------------------
		//save chars to app buffer, do this when we do read
		if(isReading){
			//if enter or control-d pressed, done
			if((int)c_actual == ENTER){
				//kprintf("ENTER ");

				//copyBuffer(internalBuffer, appBuffer);
				//todo:if enter is pressed, also add \n to output
				//addCharToAppBuffer("\n"); //not sure about this
				ready(getReadingProcess());

				resetKeyboard();
				kbd_close();
			}else if((int)c_actual == EOF_char_internal){
				//close the keyboard
				//kprintf("CONTROL_D ");

				//copyBuffer(internalBuffer, appBuffer);
				ready(getReadingProcess());

				resetKeyboard();
				kbd_close();
			}else{
				//save keystrokes to app buffer
				if((int)c_actual != 4){ //no control-d printing
					addCharToAppBuffer(c_actual);
				}
			}
		}
		//-----------------------

	}

}

int kbd_open_echo() {
//	kprintf("IN KBD OPEN ");
	keyboardEchoOn = 1;
	isOpen = 1;


	//irq for keyboard controller is 1
	//enables interrupts form keyboard
	//args(irq, diable)
	enable_irq(1, 0);

	return 0;
}

int kbd_open_noecho() {
//	kprintf("IN KBD OPEN ");
	keyboardEchoOn = 0;
	isReading = 1;

	//irq for keyboard controller is 1
	//enables interrupts form keyboard
	//args(irq, diable)
	enable_irq(1, 0);

	return 0;
}

int kbd_close() {
	keyboardEchoOn = 0;
	resetKeyboard();
	enable_irq(1, 1);
	return 0;

}

int kbd_read(void *buff, int bufflen) {
	//kprintf("kbd_read ");

	appBuffer = (char*)buff;
	appBufferLength = bufflen;

	if(appBuffer <0){
		return -1;
	}
	//TODO: allow less than 4 chars later
	if(bufflen < 4){
		return -2;
	}
	//copy data from internalBuffer (started reading when open) to appBuffer
	copyBuffer(internalBuffer, appBuffer);

	isReading = 1;

	return 0;
}

//cannot write to keyboard, always return -1
int kbd_write(void *buff, int bufflen) {
	//kprintf(" DVWRITE ");
	return -1;
}

//either changes EOF indicator, or turns echo on/off
int kbd_iotcl(unsigned long command, int EOFChar) {
	switch (command) {

	case (CHANGE_EOF): //change character for EOF
		//with int value of command to become new EOF
		EOF_char_internal = (char)EOFChar;
		kprintf("EOFChar:%d ", EOFChar);
		break;
	case (ECHOOFF): //turn echoing off
		keyboardEchoOn = 0;
		break;

	case (ECHOON): //turn echoing on
		keyboardEchoOn = 1;
		break;
	default:
		;
	}
	return 0;
}

//given code from scancodesToAscii.txt

//changed extchar to ANSI compliant syntax, more readable
static int extchar(unsigned char code) {
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
unsigned int kbtoa(unsigned char code) {
	unsigned int ch;

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

	if (code < sizeof(kbcode)) {
		if (state & CAPSLOCK)
			ch = kbshift[code];
		else
			ch = kbcode[code];
	}
	if (state & INSHIFT) {
		if (code >= sizeof(kbshift))
			return NOCHAR;
		if (state & CAPSLOCK)
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

