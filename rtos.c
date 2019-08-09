/*
* -----------------------------------------------------------
* Title:          OS definitions 
* Project:        RTOS (for MTE 241 Lab 5)
* Authors:        Cameron Haas, Yuchen He
* Date Created:   2019-07-08
* -----------------------------------------------------------
*/

#include "rtos.h"
#include "context.h"
#include <string.h>
#include "cmsis_armcc.h"
#include <stdio.h>
#include <LPC17xx.h>

// --------------------------- GLOBALS ---------------------------

//tcb list stores all tcbs
struct osTCB_s tcb_list[6] = {NULL,NULL,NULL,NULL,NULL,NULL};

//ready list
struct osTCB_s *tcb_ready_list[6] = {NULL,NULL,NULL,NULL,NULL,NULL};

//running task stored as index in tcb list 
uint8_t RUNNING_TASK = 255;
uint8_t **p_main_stack_base; //The content is the main stack base address
uint8_t fpp_bit_vec = 0;

uint8_t kernel_start = 0;


// --------------------------- INITALIZATION AND THREAD CREATION ---------------------------

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          osKernelInit
* Returns:        void
* Arguments       void
* Description:    initalizes the task stack base address of each thread					
* -----------------------------------------------------------
*/
void osKernelInit(void)
{

	p_main_stack_base = 0x0;
	printf("mainstack base address: %p\n", *p_main_stack_base);
	//intialize each TCB with the base address of its stack
	tcb_list[5].sp = *p_main_stack_base - oneKiB * 2; ///< Will be designated the new "main" stack
	printf("task 5 base address: %p\n", tcb_list[5].sp);
	tcb_list[4].sp = *p_main_stack_base - oneKiB * 3;
	printf("task 4 base address: %p\n", tcb_list[4].sp);
	tcb_list[3].sp = *p_main_stack_base - oneKiB * 4;
	printf("task 3 base address: %p\n", tcb_list[3].sp);
	tcb_list[2].sp = *p_main_stack_base - oneKiB * 5;
	printf("task 2 base address: %p\n", tcb_list[2].sp);
	tcb_list[1].sp = *p_main_stack_base - oneKiB * 6;
	printf("task 1 base address: %p\n", tcb_list[1].sp);
	tcb_list[0].sp = *p_main_stack_base - oneKiB * 7;
	printf("task 0 base address: %p\n", tcb_list[0].sp);
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          osThreadCreate
* Returns:        uint8_t
* Arguments       rtosFxn_t fxn, uint8_t priority, void *arg
* Description:    assigns task function to tcb, initalizes task parameters
									initializes task stack contents, adds task to ready list
* -----------------------------------------------------------
*/
uint8_t osThreadCreate(rtosFxn_t fxn, uint8_t priority, void *arg)
{

	//find first unused TCB - return osThreadCreateFailed if maximum number of threads created
	uint8_t tcb_index = 7;

	for (int i = 0; i < 5; i++)
	{
		if (tcb_list[i].assigned == osTCBUnassigned && tcb_index == 7)
			tcb_index = i;
	}
	if (tcb_index == 7)
		return osThreadCreateFailed;

	printf("\nSelected thread from list to assign: %u\n", tcb_index);

	//set TCB id, priority, and thread entry address
	tcb_list[tcb_index].assigned = osTCBAssigned;
	tcb_list[tcb_index].priority = priority;
	tcb_list[tcb_index].id = tcb_index;
	tcb_list[tcb_index].thread_addr = (uint32_t)fxn;
	tcb_list[tcb_index].mutex = NULL;

	printf("Assigned thread base address: %u\n", tcb_list[tcb_index].thread_addr);

	//intialize stack according to table 2
	*(uint32_t *)(tcb_list[tcb_index].sp - 4) = 0x01000000;		///< Set PSR default value
	*(uint32_t *)(tcb_list[tcb_index].sp - 8) = (uint32_t)fxn;  ///< Set PC default value
	*(uint32_t *)(tcb_list[tcb_index].sp - 32) = (uint32_t)arg; ///< Set R0 default value
	tcb_list[tcb_index].sp -= 64;								///< Offset sp so first context pop occurs in correct order

	printf("Stack pointer updated to: %p\n", tcb_list[tcb_index].sp);
	printf("Thread Priority Set to: %d\n", tcb_list[tcb_index].priority);

	//add to ready list based on priority
	add_to_ready_list(tcb_index);
	return osThreadCreateSuccess;
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          osKernelStart
* Returns:        void
* Arguments       void
* Description:    copies main stack contents to new main() task stack, 
									sets MSP back to base adr of main stack, sets PSP to 
									top of main() task stack
* -----------------------------------------------------------
*/
void osKernelStart(void)
{
	uint32_t control_Register_temp = 0;

	//create main() task and initialize
	tcb_list[5].assigned = osTCBAssigned;
	tcb_list[5].priority = osPriorityMentalHealth; ///< Set to lowest priority
	tcb_list[5].id = 5;
	tcb_list[5].thread_addr = NULL; ///< Should never run from the beginning
	tcb_list[5].state = osThreadReady;    ///< initialized for first context switch ****** TEMP ******* should be set to running eventually

	//intialize stack according to table 2
	*(uint32_t *)(tcb_list[5].sp - 4) = 0x01000000; ///< Set PSR default value
	*(uint32_t *)(tcb_list[5].sp - 8) = NULL;		///< Will store contents main() task is initially switched out of
	*(uint32_t *)(tcb_list[5].sp - 32) = NULL;		///< Will store contents main() task is initially switched out of
	tcb_list[5].sp -= 64;							///< Offset sp so first context pop occurs in correct order

	uint32_t newBase = 0;

	printf("\nStarted kernel\n");
	printf("\nMain Stack Pointer %p\n", (uint8_t *)__get_MSP());

	//print main stack contents
	printf("\nMain Stack Contents\n");
	printf("%u\n", *((uint8_t *)__get_MSP())); //top of stack
	printf("%u\n", *(((uint8_t *)__get_MSP()) + 1));
	printf("%u\n", **p_main_stack_base); //base of stack

	newBase = (uint32_t)memcpy((void *)(*p_main_stack_base - (oneKiB * 3 - 1)), (void *)(*p_main_stack_base - (oneKiB - 1)), oneKiB);
	tcb_list[5].sp = (uint8_t *)(__get_MSP() - (oneKiB * 2));

	printf("\n1 KiB of main stack contents copied to thread stack of task 5\n");
	printf("Thread five stack pointer: %p\n", tcb_list[5].sp);

	//print new main stack contents
	printf("\nNew Main Stack Contents\n");
	printf("%u\n", *tcb_list[5].sp); //top of stack
	printf("%u\n", *(tcb_list[5].sp + 1));
	printf("%u\n", *(*p_main_stack_base - (oneKiB * 3 - 1))); //base of stack

	//set MSP to main stack base address
	__set_MSP((uint32_t)*p_main_stack_base);

	printf("Updated MSP: %p\n", (uint8_t *)__get_MSP());

	//switch from MSP to PSP and set PSP to top of main() task stack
	control_Register_temp = __get_CONTROL();
	control_Register_temp |= SPSEL_SETPSP;
	__set_CONTROL(control_Register_temp);

	__set_PSP((uint32_t)tcb_list[5].sp);

	RUNNING_TASK = 5; ///< Set running task index to main() thread index

	printf("Updated PSP: %p\n", (uint8_t *)__get_PSP());
	
	printf("Initial bit vector value: %d\n", fpp_bit_vec);

	//enable interrupts from systick to run scheduler  ***TO DO***
	kernel_start = 1;
	printf("Info: Started kernel\n");
}

// --------------------------- SCHEDULER ---------------------------

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          Systick_Handler
* Returns:        void
* Arguments       void
* Description:    Handles SysTick interrupt, calls PenSV Handler 
									once the kernel has been started
* -----------------------------------------------------------
*/
void SysTick_Handler(void)
{
	
	if (kernel_start)
	{ //added -1 to allow fpp_bit_vec to be set in add_to_ready_list() -> execution stops otherwise
		//Do i need to set PendSv clear in PendSV_Handler()??
		//printf("\n\nInfo: triggering PendSV_Handler\n");
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	}
	
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          PendSV_Handler
* Returns:        void
* Arguments       void
* Description:    Interrupt handler triggered by Systick ISR, stores current 
									task context, runs sheduler routine, restores context of next
									task to run
* -----------------------------------------------------------
*/
void PendSV_Handler(void)
{
	
	//printf("Info: running PendSV_Handler\n");
	printf("\n\n\n");
	//store context of currently executing task and update the PSP
	tcb_list[RUNNING_TASK].sp = (uint8_t*)storeContext();
	//printf("Info: stored context\n");
	
	//run scheduler and determine next task to run
	osSchedulerFPP();
	//printf("Info: scheduler completed, running task set to : %d\n", RUNNING_TASK);
	//restore context for the next task to run
	restoreContext((uint32_t)(tcb_list[RUNNING_TASK].sp));
}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          osSchedulerFPP
* Returns:        uint8_t
* Arguments       void
* Description:    Maintains ready and blocked lists, adds running task to appropriate list, 
									pops next highest prioriy task from ready list and updates running task index
									in tcb_list
* -----------------------------------------------------------
*/
void osSchedulerFPP(void)
{

	//printf("Info: scheduler invoked\n");
	//add running task to ready/blocked list based on task state
	if (tcb_list[RUNNING_TASK].state == osThreadRunning){
		
		//priority inheritance
		if (tcb_list[RUNNING_TASK].mutex->owner_inherited_priority_flag == 1){
			//uint8_t temp = dequeue_ready_list(RUNNING_TASK);
			tcb_list[RUNNING_TASK].priority = tcb_list[RUNNING_TASK].mutex->inherited_priority;
		}
		
			add_to_ready_list(RUNNING_TASK);
		
		tcb_list[RUNNING_TASK].state = osThreadReady;
	}
	
	/* ------------------------- At this point, all lists should be up to date ------------------------- */

	//check bit vector for next non-empty priority list in ready list
	uint8_t leading_Zeros = __CLZ(fpp_bit_vec);
	uint8_t highest_Priority = 31 - leading_Zeros;

	//pop first task from selected priority list and update running task index
	RUNNING_TASK = dequeue_ready_list(highest_Priority);

	tcb_list[RUNNING_TASK].state = osThreadRunning;

}

/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          add_to_ready_list
* Returns:        void
* Arguments       struct osTCB_s *p_tcb
* Description:    Takes pointer to thread node to be added to list. 
*    	          Add the thread node to the tail of the priority list						
* -----------------------------------------------------------
*/
void add_to_ready_list(uint8_t task_index)
{
	uint8_t priority = tcb_list[task_index].priority;
	uint8_t id = tcb_list[task_index].id;

	if (tcb_ready_list[priority] == NULL)
	{
		tcb_ready_list[priority] = &tcb_list[task_index];
		tcb_list[task_index].state = osThreadReady;
		tcb_list[task_index].thread_next = NULL;
	}
	else
	{
		struct osTCB_s *tail = tcb_ready_list[priority];
		for (; tail->thread_next != NULL; tail = tail->thread_next);
		tail->thread_next = &tcb_list[task_index];
		tcb_list[task_index].state = osThreadReady;
		tcb_list[task_index].thread_next = NULL;
	}

	fpp_bit_vec |= (1 << priority); //update the bit vector
	//printf("Info: task %d added to the ready list\n",task_index);
}
/*
* -----------------------------------------------------------
* Funtion Definition
* Title:          dequeue_ready_list
* Returns:        threadNode_t *
* Arguments       struct osTCB_s **priority_HeadPtr
* Description:    Takes a priority index and pops and returns the first 
									task from the linked list at that priority
* -----------------------------------------------------------
*/

uint8_t dequeue_ready_list(uint8_t priority)
{
	
	//temp pointer to first task in priority list 
	struct osTCB_s* temp = tcb_ready_list[priority];
	
	//update head pointer to point to next task in list (NULL if that is the case)        
	//update the bit vector for non-empty priority levels	
	if(temp != NULL)
		tcb_ready_list[priority] = temp->thread_next;
	
	//update the bit vector
	if (tcb_ready_list[priority] == NULL)
		fpp_bit_vec &= ~(1 << priority);
	

	return temp->id;
}
