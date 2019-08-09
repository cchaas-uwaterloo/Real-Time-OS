/*
* -----------------------------------------------------------
* Title:          Mutex definitions
* Project:        RTOS (for MTE 241 Lab 5)
* Authors:        Cameron Haas, Yuchen He
* Date Created:   2019-07-08
* -----------------------------------------------------------
*/

#include "mutex.h"
#include "rtos.h"
#include "cmsis_armcc.h"
#include <stdio.h>
#include "LPC17xx.h"
#include<stdlib.h>

// ------ prototypes for blocked list functions ------
void add_to_blocked_list_mutex(mutex_t *m);
uint8_t dequeue_block_list_mutex(mutex_t *m);

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          mutex_init
* Returns:        void
* Arguments       mutex_t *m
* Description:    Initializes blocked list, bit vector, count, 
									owner_index and inherited priority flag 
* -----------------------------------------------------------
*/
void mutex_init(mutex_t *m){
	  m->block_list_bit_vec = 0;
    for(uint8_t i = 0;i < 6;i++){
        m->block_list[i] = NULL;
    }
    m->count = 1; //intialize count to zero for mutex
		m->owner_index = 255;
		m->owner_inherited_priority_flag  = 0;
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          acquire_mutex
* Returns:        void
* Arguments       mutex_t *m
* Description:    decrements the mutex counter and links it with
									the owner (running) thread, if acquisition unsuccessful, 
									adds running thread to mutex blocked list and checks for 
									priority inheritance, fires scheduler on exit
* -----------------------------------------------------------
*/
void acquire_mutex(mutex_t *m){
		
    __disable_irq();
		printf("Info: task %d trying to acquire the mutex\n",RUNNING_TASK);
		if (m->count>=0)
			m->count --;
	
		//if the mutex is available, acquire it, otherwise add task to that mutex's blocked list
    if ((*m).count >= 0)
    {
			printf("Info: task %d successfully acquired mutex\n",RUNNING_TASK);
			
			//store the owner of the mutex on acquisition
			m->owner_index = RUNNING_TASK;
			m->owner_priority = tcb_list[RUNNING_TASK].priority;
			
			tcb_list[RUNNING_TASK].mutex = m;
			
			__enable_irq();
      SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
      return;
    }
		if(RUNNING_TASK != m->owner_index){
			add_to_blocked_list_mutex(m);
			
			
			//flag priority inherit 
			if (tcb_list[RUNNING_TASK].priority > m->owner_priority){
				m->owner_inherited_priority_flag = 1;
				m->inherited_priority = tcb_list[RUNNING_TASK].priority;
			}
				
			
			printf("Info: Task %d added to mutex blocked list\n", RUNNING_TASK);
		}
    printf("Info: task %d could not acquire mutex\n",RUNNING_TASK);
		__enable_irq();
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          release_mutex
* Returns:        void
* Arguments       mutex_t *m
* Description:    increments the mutex and restores blocked tasks
									to ready list, demotes owner priority if necessary
									fires scheduler on exit 
* -----------------------------------------------------------
*/
void release_mutex(mutex_t *m){
    __disable_irq();
		printf("Info: task %d releasing the mutex\n",RUNNING_TASK);
		//owner test on release, also cannot realease if mutex is available already
		if(RUNNING_TASK == m->owner_index && m->count <= 1) {
			(*m).count++;
			while((*m).count >= 0 && m->block_list_bit_vec > 0){
					m->count = 1;
					uint8_t ready_task = dequeue_block_list_mutex(m);//pop the first task of the highest priority from the blocked list
					printf("Info: removing task %d from the block list\n",ready_task);
					add_to_ready_list(ready_task);
					m->owner_index = 255;
				
					tcb_list[RUNNING_TASK].mutex = NULL;
				
					//demote priority
					if(m->owner_inherited_priority_flag == 1){
						tcb_list[RUNNING_TASK].priority = m->owner_priority;
						m->owner_inherited_priority_flag = 0;
					}
				
			}

		}
		else {
			printf("Info: task %d failed to release the mutex\n", RUNNING_TASK);
		}
		__enable_irq();
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}


/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          add_to_blocked_list_mutex
* Returns:        void
* Arguments       mutex_t *m
* Description:    adds runnig task to mutex blocked list and
									updates blocked list bit vector
* -----------------------------------------------------------
*/
void add_to_blocked_list_mutex(mutex_t *m){
    uint8_t priority = tcb_list[RUNNING_TASK].priority;
    
	//list of this priority is NULL
  if ((*m).block_list[priority] == NULL)
	{
		(*m).block_list[priority] = &tcb_list[RUNNING_TASK];
		tcb_list[RUNNING_TASK].state = osThreadBlocked;
		tcb_list[RUNNING_TASK].thread_next = NULL;
	}
    //when the list of this proirity actually has entries
	else
	{
		struct osTCB_s *tail = (*m).block_list[priority];
		for (; tail->thread_next != NULL; tail = tail->thread_next);
		tail->thread_next = &tcb_list[RUNNING_TASK];
		tcb_list[RUNNING_TASK].state = osThreadBlocked;
		tcb_list[RUNNING_TASK].thread_next = NULL;
	}
    m->block_list_bit_vec |= (1 << priority); //update the bit vector
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          dequeue_block_list_mutex
* Returns:        void
* Arguments       mutex_t *m
* Description:    removes the higest priority blocked task from
									the mutex blocked list and updates the blocked
									list bit vector
* -----------------------------------------------------------
*/
uint8_t dequeue_block_list_mutex(mutex_t *m)
{
  uint8_t leading_Zeros = __CLZ(m->block_list_bit_vec); //BUG FIX
	uint8_t highest_priority = 31 - leading_Zeros;
	//temp pointer to first task in priority list
	struct osTCB_s *temp = (*m).block_list[highest_priority];
	//update head pointer to point to next task in list (NULL if that is the case)
	//update the bit vector for non-empty priority levels																***TODO***
	(*m).block_list[highest_priority] = temp->thread_next; //BUG, check for null pointer here..... Not really a bug if this function is only called when the list is not empty
	temp->state = osThreadReady;
	//update the bit vector
	if ((*m).block_list[highest_priority] == NULL)
		m->block_list_bit_vec &= ~(1 << highest_priority);
	return temp->id;
}
