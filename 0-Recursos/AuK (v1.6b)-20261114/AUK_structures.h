/* Structures required to use AuK */

#ifndef AUK_STRUCTURES_H
#define	AUK_STRUCTURES_H

#include "AuK_config.h"

/*##########################################################################*/

/* Struct to manage queues of process identifiers ordered by priority and 
 * FIFO within priorities 
 
 */

/* This structure is for exclusive use within AuK.  */
typedef struct
{   int *task_queue; // Pool for chained list.
    int out;  // Pointer to the first element in queue
    int total_in_queue; //Total number of processes in the queue.
} Ttask_queue;

/*##########################################################################*/

/* Struct to declare semaphores */
typedef struct
{
    int semaphore_counter; // Semaphore counter in Brinch Hansen sense.
    Ttask_queue BlockedTasks;
}Tsemaphore;

/*##########################################################################*/

/* Mutex are objects of exclusive access by tasks, with priority according 
 * to the immediate priority ceiling real time scheduler protocol */
typedef struct mutex
{
    Tsemaphore sem;
    int mutex_priority;
    int task_old_priority;
    int holder_task;
}Tmutex;

/*##########################################################################*/

#endif	/* AUK_STRUCTURES_H */

