

int kbd_open(void);
int kbd_close(void);

//no echo
int kbd0_read(void *buff, int bufflen);
//echos
int kbd1_read(void *buff, int bufflen);
int kbd_write(void *buff, int bufflen);
int kbd_iotcl(unsigned long command);
