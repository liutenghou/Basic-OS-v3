/* signal.c - support for signal handling
   This file is not used until Assignment 3
 */

#include <xeroskernel.h>
#include <xeroslib.h>

//registered provided newhandler as handler when signal
int sighandler(int signal, void (*newhandler)(void *), void (** oldHandler)(void *)){

	//if signal is invalid, return -1

	//if handler has invalid address, return -2

	//handler successfully installed
	return 0;
}
