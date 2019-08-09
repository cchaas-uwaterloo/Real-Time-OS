/*
* -----------------------------------------------------------
* Title:          OS declarations
* Project:        RTOS (for MTE 241 Lab 5)
* Authors:        Cameron Haas, Yuchen He
* Date Created:   2019-07-08
* -----------------------------------------------------------
*/

#ifndef RTOS_H_
#define RTOS_H_

#include <stdint.h>
#include "semaphore.h"
#include "mutex.h"

//------- OS Statuses -------

#define osOK 											((uint8_t)1)
#define osFaulted 								((uint8_t)0)

//------- OS Priorities -------

#define osPriorityMentalHealth    ((uint8_t)0)
#define osPrioritySocialLife      ((uint8_t)1)
#define osPriorityCleanliness     ((uint8_t)2)
#define osPriorityGPA             ((uint8_t)3)
#define osPriorityYoutube         ((uint8_t)4)
#define osPriorityGains           ((uint8_t)5)

//------- Thread Attributes -------

/// Thread stack byte offset
#define oneKiB                     1024

/// Control Register Thread Select 
#define SPSEL_SETPSP               0x02            ///< Used to set the second bit in the control register

/// Thread States

#define osThreadInactive 					((uint8_t)0)
#define osThreadReady    					((uint8_t)1)
#define osThreadRunning  					((uint8_t)2)
#define osThreadBlocked  					((uint8_t)3)
#define osThreadTerminated 				((uint8_t)4)
#define osThreadFaulted 				  ((uint8_t)5)

/// Assigned States
#define osTCBUnassigned           ((uint8_t)0)
#define osTCBAssigned							((uint8_t)1)

/// Return Codes
#define osThreadCreateFailed 			((uint8_t)0)
#define osThreadCreateSuccess     ((uint8_t)1)

/// Thread Function Type
typedef void (*rtosFxn_t)(void *args);

extern uint8_t RUNNING_TASK;
extern uint8_t fpp_bit_vec;
/// Thread Control Block

struct osTCB_s{
  uint8_t                           id; 	 ///< Object Identifier
  uint8_t                        state; 	 ///< Object State
  uint8_t                     assigned; 	 ///< Object Flags
	struct osTCB_s  				*thread_next; 	 ///< Link pointer to next Thread in Object list
	struct osTCB_s  				*thread_prev; 	 ///< Link pointer to previous Thread in Object list
  int8_t                      priority; 	 ///< Thread Priority
  mutex_t                       *mutex; 	 ///< Link pointer to list of owned Mutexes
  uint32_t                  stack_size; 	 ///< Stack Size
  uint8_t                          *sp; 	 ///< Current Stack Pointer
  uint32_t                 thread_addr; 	 ///< Thread entry address
};



extern struct osTCB_s tcb_list[6];
extern struct osTCB_s *tcb_ready_list[6];
	
// ------ Function Declarations -------                         

void osDelay(uint32_t ms_time);


void osKernelInit(void);
uint8_t osThreadCreate(rtosFxn_t fxn, uint8_t priority, void *arg);
void osKernelStart(void);

void osSchedulerFPP (void);
void add_to_ready_list(uint8_t task_index);
uint8_t dequeue_ready_list (uint8_t priority);


#endif
