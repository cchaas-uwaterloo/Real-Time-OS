/*
* -----------------------------------------------------------
* Title:          Semaphore definitions
* Project:        RTOS (for MTE 241 Lab 5)
* Authors:        Cameron Haas, Yuchen He
* Date Created:   2019-07-08
* -----------------------------------------------------------
*/

#include "semaphore.h"
#include "rtos.h"
#include "cmsis_armcc.h"
#include <stdio.h>
#include "LPC17xx.h"
#include<stdlib.h>

// ------ prototypes for blocked list functions ------
void add_to_blocked_list_sem(sem_t *s);
uint8_t dequeue_block_list_sem(sem_t *s);

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          sem_init
* Returns:        void
* Arguments       sem_t *s, int8_t count
* Description:    Initializes blocked list, bit vector and count of 
									Semaphore
* -----------------------------------------------------------
*/
void sem_init(sem_t *s, int8_t count){
	  s->block_list_bit_vec = 0;
    for(uint8_t i = 0;i < 6;i++){
        s->block_list[i] = NULL;
    }
    s->count = count;
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          wait
* Returns:        void
* Arguments       sem_t *s
* Description:    waits on the semaphore, if unsuccessful, adds running
									task to semaphore's blocked list, fires scheduler on 
									completion
* -----------------------------------------------------------
*/
void wait(sem_t *s){
    __disable_irq();
		printf("Info: task %d trying to decrement the semaphore\n",RUNNING_TASK);
		(s->count) --;
    if ((*s).count >= 0)
    {
			printf("Info: task %d successfully decremented semaphore\n",RUNNING_TASK);
			__enable_irq();
      //enqueue the running task to the block list
      return;
    }
		printf("Info: task %d waiting on semaphore\n",RUNNING_TASK);
    add_to_blocked_list_sem(s);
    __enable_irq();
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; //So that blocked task would immediately stop executing
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          signal
* Returns:        void
* Arguments       sem_t *s
* Description:    signals the semaphore, removes highest priority
									task from semaphore blocked list, fires scheduler
									on completion
* -----------------------------------------------------------
*/
void signal(sem_t *s){
    __disable_irq();
		printf("Info: task %d signaling the semaphore\n",RUNNING_TASK);
    (*s).count++;
    if(s->block_list_bit_vec > 0){
				(s->count)--;
        uint8_t ready_task = dequeue_block_list_sem(s);//pop the first task of the highest priority from the blocked list
				printf("Info: removing task %d from the block list\n",ready_task);
        add_to_ready_list(ready_task);
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
			__enable_irq();
    }
    __enable_irq();
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          add_to_blocked_list_sem
* Returns:        void
* Arguments       sem_t *s
* Description:    adds running task to semaphore blocked list
									based on priority, updates blocked list bit vector
* -----------------------------------------------------------
*/
void add_to_blocked_list_sem(sem_t *s){
    uint8_t priority = tcb_list[RUNNING_TASK].priority;
    //list of this priority is NULL
    if ((*s).block_list[priority] == NULL)
	{
		(*s).block_list[priority] = &tcb_list[RUNNING_TASK];
		tcb_list[RUNNING_TASK].state = osThreadBlocked;
		tcb_list[RUNNING_TASK].thread_next = NULL;
	}
    //when the list of this proirity actually has entries
	else
	{
		struct osTCB_s *tail = (*s).block_list[priority];
		for (; tail->thread_next != NULL; tail = tail->thread_next);
		tail->thread_next = &tcb_list[RUNNING_TASK];
		tcb_list[RUNNING_TASK].state = osThreadBlocked;
		tcb_list[RUNNING_TASK].thread_next = NULL;
	}
    s->block_list_bit_vec |= (1 << priority); //update the bit vector
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          dequeue_block_list_sem
* Returns:        uint8_t
* Arguments       sem_t *s
* Description:    removes highest priority task from semaphore blocked
									list and returns its index
* -----------------------------------------------------------
*/
uint8_t dequeue_block_list_sem(sem_t *s)
{
  uint8_t leading_Zeros = __CLZ(s->block_list_bit_vec); //BUG FIX
	uint8_t highest_priority = 31-leading_Zeros;
	//temp pointer to first task in priority list
	struct osTCB_s *temp = (*s).block_list[highest_priority];
	//update head pointer to point to next task in list (NULL if that is the case)
	//update the bit vector for non-empty priority levels																***TODO***
	(*s).block_list[highest_priority] = temp->thread_next; //BUG, check for null pointer here..... Not really a bug if this function is only called when the list is not empty
	temp->state = osThreadReady;
	//update the bit vector
	if ((*s).block_list[highest_priority] == NULL)
		s->block_list_bit_vec &= ~(1 << highest_priority);
	return temp->id;
}
