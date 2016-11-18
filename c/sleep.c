/* sleep.c : sleep device 
   This file does not need to modified until assignment 2
 */

#include <xeroskernel.h>
#include <xeroslib.h>


#define MILLISECONDS_TICK 10
static pcb	*sleepQ;


// Len is the length of time to sleep


/* This function works by mainting a delta list. This is where
   each element in the list has as its key a value that is its difference
   from the previous value. For example if we had the values 3, 7, 8, and 15 
   to put in a list a list with their actual values would look like:
       3->7->8->15   As a delta list this would be stored as:
       3->4->1->7    To get the value of a node we sum all the preceding 
                     values along with the value in that node. If these 
   values represent how long into the future to sleep then all we have to do
   is decrement the value of the head of the list on each tick. When that value
   gets to 0 the time has expired and we remove it from the list. The value 
   value of the element now at the head of the list represents how much 
   additional time has to elapse before that time expires.

   see: http://everything2.com/title/delta+list for additional information
*/


void	sleep( pcb *p, unsigned int len ) {
/****************************************/

    pcb	*tmp;


    if( len < 1 ) {
        ready( p );
        return;
    }

    // Convert the length of time to sleep in ticks
    // each tick is 10ms 
    len = len / MILLISECONDS_TICK;

    p->state = STATE_SLEEP;
    p->next = NULL;
    p->prev = NULL;
    if( !sleepQ ) { /* Empty sleep list */
        sleepQ = p;
        p->sleepdiff = len;
    } else if( sleepQ->sleepdiff > len ) { /* Add to front */
        p->next = sleepQ;
        sleepQ->sleepdiff -= len;
        p->sleepdiff = len;
        sleepQ = p;
    } else {  /* Goes after the head of the queue */
        len -= sleepQ->sleepdiff;

	/* Look for the spot in the sleep queue where this belongs */
        for( tmp = sleepQ; tmp->next; tmp = tmp->next ) {
            if( len < tmp->next->sleepdiff ) {
	      break; /* goes in front of next element */
            } else {
	      /* goes after next element, so update the time difference */
              /* and check the next element                             */
	      len -= tmp->next->sleepdiff;
            }
        }

	
        p->next = tmp->next;
        p->prev = tmp;
        p->sleepdiff = len;
        tmp->next = p;
        
	if( p->next ) { /* Not at that end of the list so insert it */
            p->next->prev = p;
            p->next->sleepdiff -= len;
        }
    }
}


extern void	tick( void ) {
/****************************/

	//increment tickCount for current process
	getCurrentProcess()->tickCount++;
	//kprintf("currenTicksInTick:%d ",getCurrentProcess()->tickCount);
	kprintf("%d.", getCurrentProcess()->pid);
    pcb	*tmp;

    if( !sleepQ ) {
        return;
    }

    //initially subtracts from sleepdiff in head
    //if the queue has something, and sleepdiff is 0 for a process in the queue
    //get it ready, loop for next sleepdiff in sleepqueue
    //this is for the case if there are multiple 0 values linked on the queue
    //pretty clever
    for( sleepQ->sleepdiff--; sleepQ && !sleepQ->sleepdiff; ) {
        tmp = sleepQ;
        sleepQ = tmp->next;

        tmp->state = STATE_READY;
        tmp->next = NULL;
        tmp->prev = NULL;
        tmp->ret = 0;
        ready( tmp );
    }
}

//returns the number of ticks a process has consumed so far
int getticks(int pid){
	//get the process
	pcb *p = findPCB(pid);
	pcb *pCurrent = getCurrentProcess();


	//if pid == -1, give ticks by current process
	if(pid == -1){
		return pCurrent->tickCount;
	}
	//if pid == 0, give ticks by idle process
	if(pid == 0){
		p = findPCB(getIdlePID());
		return p->tickCount;
	}

	//if process no longer exists, return -1
	if(!p) {
		return -1;
	}

	//kprintf("tickcountINgetTicks:%d ", p->tickCount);
	return p->tickCount;

}

