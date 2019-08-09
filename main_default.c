/*
 * Default main.c for rtos lab.
 * @author Andrew Morton, 2018
 */
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "rtos.h"

uint32_t msTicks = 0;

// ----- Semaphores and Mutexes for testing ------
sem_t s1;
mutex_t m1;
sem_t s2;
mutex_t m2;


//// ------ Shecheduler Test Functions ------

//void thread1(void) 
//{

//	while (1){
//	int a = 1;
//	a++;
//	
//  __disable_irq();
//	printf("Info: thread 1 running\n");
//	printf("Info: local variable a = %d\n",a);
//	__enable_irq();
//		
//	};
//}

//void thread2(void) 
//{
//	int b = 1;
//	while (1){
//	b++;
//		
//	__disable_irq();
//	printf("Info: thread 2 running\n");
//	printf("Info: local variable b = %d\n",b);
//	__enable_irq();
//	};
//	
//}

//void thread3(void) 
//{
//	int c = 1;
//	while (1){
//	c++;
//		
//	__disable_irq();
//	printf("Info: thread 3 running\n");
//	printf("Info: local variable c = %d\n",c);
//	__enable_irq();
//	};
//	
//}

//void thread4(void) 
//{
//	int d = 1;
//	while (1){
//	d++;
//		
//	__disable_irq();
//	printf("Info: thread 4 running\n");
//	printf("Info: local variable d = %d\n",d);
//	__enable_irq();
//	};
//	
//}

//void thread5(void) 
//{
//	int e = 1;
//	while (1){
//	e++;
//		
//	__disable_irq();
//	printf("Info: thread 5 running\n");
//	printf("Info: local variable e = %d\n",e);
//	__enable_irq();
//	};
//}

//void thread6(void) 
//{
//	int f = 1;
//	while (1){
//	f++;
//		
//	__disable_irq();
//	printf("Info: thread 6 running\n");
//	printf("Info: local variable f = %d\n",f);
//	__enable_irq();
//	};
//}

//void thread7(void) 
//{
//	int g = 1;
//	while (1){
//	g++;
//	printf("Info: thread 7 running\n");
//	printf("Info: local variable g = %d\n",g);
//	};
//}

// --------- Semaphore Test Functions ---------



//// Semaphore Blocking
//void thread_sem_low_p(void){
//	uint8_t a = 0;
//	while(1){
//		__disable_irq();
//		printf("Info: a = %d\n",a);
//		__enable_irq();
//		signal(&s1);
//		signal(&s1);
//		osDelay(20000);
//		a = a + 1;
//	}
//}

//void thread_sem_high_p(void){
//	uint8_t b = 0;
//	while(1){
//		__disable_irq();
//		printf("Info: b = %d\n",b);
//		__enable_irq();
//		wait(&s1);
//		b += 1;

//		osDelay(200);
//	}
//}
//void thread_sem_high_p2(void){
//	uint8_t c = 0;
//	while(1){
//		__disable_irq();
//		printf("Info: c = %d\n",c);
//		__enable_irq();
//		wait(&s1);
//		c += 1;
//		osDelay(200);
//	}
//}



//// ------- Mutex Test Functions -------

//uint8_t delay_acquire = 0;

//void thread_owner_same_p(void){\
//	
//	acquire_mutex(&m1);
//	while(1){
//		
//		//push button
//		if (!(LPC_GPIO2->FIOPIN & (1 << 10))){
//			release_mutex(&m1);
//		}
//		
//		__disable_irq();
//		printf("Info: task 0 ran\n");
//		__enable_irq();
//		
//	}
//}

//void thread_blocked_same_p(void) {
//	
//	while(1){
//		acquire_mutex(&m1);
//		__disable_irq();
//		printf("Info: task 1 ran\n");
//		__enable_irq();
//	}
//	

//}

//void thread_trying_to_release_same_p(void) {
//	while(1) {
//		release_mutex(&m1);
//		
//		__disable_irq();
//		printf("Info: task 2 ran\n");
//		__enable_irq();
//	}
//}

//// ------ Priority Inherit Test Functions ------

//uint8_t signalled = 0;
//uint8_t printed_promoted = 0;
//	
//void high_priority_thread (void) {
//	while (1){
//		
//		wait(&s2);
//		
//		acquire_mutex(&m2);
//		
//		__disable_irq();
//		printf("Info: task 2 ran\n");
//		__enable_irq();
//		
//		printf("Info: Task 2 mutex acquired\n");
//	}
//}	
//	
//void mid_priority_thread (void) {
//	wait(&s2);
//	while (1){
//		__disable_irq();
//		printf("Info: task 1 ran\n");
//		__enable_irq();
//	}
//}

//void low_priority_thread (void) {
//	
//	acquire_mutex(&m2);
//	while (1){
//		
//		if(signalled == 0){
//			signal(&s2);
//			signal(&s2);
//			signalled = 1;
//		}
//		
//		__disable_irq();
//		printf("Info: task 0 ran\n");
//		__enable_irq();
//		
//		__disable_irq();
//		if(tcb_list[RUNNING_TASK].mutex->owner_inherited_priority_flag == 1 && printed_promoted == 0){
//			printf("Info: task %d promoted to priority %d\n", RUNNING_TASK,tcb_list[RUNNING_TASK].mutex->inherited_priority);
//			printed_promoted = 1;
//		}
//		__enable_irq();
//		
//		if (!(LPC_GPIO2->FIOPIN & (1 << 10))){
//			release_mutex(&m2);
//			printf("Info: Released mutex\n");
//		}
//		

//		
//	}
//}

int main(void) {
	SysTick_Config(SystemCoreClock/10);
	printf("\nStarting...\n\n");
	
	
	osKernelInit();
	
	
	// ----- Scheduler test -----
	
	/*
	* Expected Functionality: 
	* tasks 1-5 created successfully 
	* tasks 6,7 fail to create
	* scheduler cycles through tasks 1-5 and local variables incremented successfully
	*/
	
//	uint8_t threadCreateStatus_TEST1 = 0;
//	threadCreateStatus_TEST1 = osThreadCreate((rtosFxn_t)thread1,osPriorityGains,NULL);
//	if (threadCreateStatus_TEST1 == osThreadCreateSuccess)
//	{
//		printf("Thread Create Success\n");
//	}
//	else{
//		printf("Thread Create Failed\n");
//	}

//	uint8_t threadCreateStatus_TEST2 = 0;	
//	threadCreateStatus_TEST2 = osThreadCreate((rtosFxn_t)thread2,osPriorityGains,NULL);
//	if (threadCreateStatus_TEST2 == osThreadCreateSuccess)
//	{
//		printf("Thread Create Success\n");
//	}
//	else{
//		printf("Thread Create Failed\n");
//	}

//	uint8_t threadCreateStatus_TEST3 = 0;
//	threadCreateStatus_TEST3 = osThreadCreate((rtosFxn_t)thread3,osPriorityGains,NULL);
//	if (threadCreateStatus_TEST3 == osThreadCreateSuccess)
//	{
//		printf("Thread Create Success\n");
//	}
//	else{
//		printf("Thread Create Failed\n");
//	}

//	uint8_t threadCreateStatus_TEST4 = 0;	
//	threadCreateStatus_TEST4 = osThreadCreate((rtosFxn_t)thread4,osPriorityGains,NULL);
//	if (threadCreateStatus_TEST4 == osThreadCreateSuccess)
//	{
//		printf("Thread Create Success\n");
//	}
//	else{
//		printf("Thread Create Failed\n");
//	}

//	uint8_t threadCreateStatus_TEST5 = 0;
//	threadCreateStatus_TEST5 = osThreadCreate((rtosFxn_t)thread5,osPriorityGains,NULL);
//	if (threadCreateStatus_TEST5 == osThreadCreateSuccess)
//	{
//		printf("Thread Create Success\n");
//	}
//	else{
//		printf("Thread Create Failed\n");
//	}

//	uint8_t threadCreateStatus_TEST6 = 0;
//	threadCreateStatus_TEST6 = osThreadCreate((rtosFxn_t)thread6,osPriorityGains,NULL);
//	if (threadCreateStatus_TEST6 == osThreadCreateSuccess)
//	{
//		printf("Thread Create Success\n");
//	}
//	else{
//		printf("Thread Create Failed\n");
//	}

//	uint8_t threadCreateStatus_TEST7 = 0;
//	threadCreateStatus_TEST7 = osThreadCreate((rtosFxn_t)thread7,osPriorityGains,NULL);
//	if (threadCreateStatus_TEST7 == osThreadCreateSuccess)
//	{
//		printf("Thread Create Success\n");
//	}
//	else{
//		printf("Thread Create Failed\n");
//	}
	
	
	// ----- Semaphore Blocking test ------
	
	/*
	* Expected Functionality: 
	* all tasks created successfully 
	* high priority task(s) block on semaphore with high frequency
	* low priority task signals to unblock high priorities with low frequency
	* pattern of blocking and unblocking repeats peridically 
	* local variables update successfully
	*/
	
//	sem_init(&s1, 0);
//	uint8_t status = 0;
//	status = osThreadCreate((rtosFxn_t)thread_sem_low_p, 2, NULL);
//	if(status == osThreadCreateSuccess) printf("Info: thread_sem_low_p created\n");
//	else if(status == osThreadCreateSuccess) printf("Info: thread_sem_low_p creation failed\n");
//	
//	status = osThreadCreate((rtosFxn_t)thread_sem_high_p, 4, NULL);
//	if(status == osThreadCreateSuccess) printf("Info: thread_sem_high_p created\n");
//	else if(status == osThreadCreateSuccess) printf("Info: thread_sem_high_p creation failed\n");
//	
//	status = osThreadCreate((rtosFxn_t)thread_sem_high_p, 4, NULL);
//	if(status == osThreadCreateSuccess) printf("Info: thread_sem_high_p2 created\n");
//	else if(status == osThreadCreateSuccess) printf("Info: thread_sem_high_p2 creation failed\n");
	//*/
	
	// ----- Mutex test -----
	
	/*
	* Expected Functionality: 
	* all tasks created successfully 
	* thread 0 acquires mutex intially and blocks thread 1
	* thread 2 repeatedly tries to release mutex but cannot (does not own mutex)
	* thread 0 and 2 alternate while thread 1 blocked on mutex
	* thread 0 releases mutex on button press 
	* once thread 1 is unblocked, threads 0,1,2 cycle repeatedly
	*/
	
//	mutex_init(&m1);
//	uint8_t status_1  =  0;
//	
//	status_1 = osThreadCreate((rtosFxn_t)thread_owner_same_p,osPriorityCleanliness,NULL);
//	status_1 = osThreadCreate((rtosFxn_t)thread_blocked_same_p,osPriorityCleanliness,NULL);
//	status_1 = osThreadCreate((rtosFxn_t)thread_trying_to_release_same_p,osPriorityCleanliness,NULL);
	
	// ------ Priority Inheritance test ------
	
	/*
	* all tasks created successfully
	* threads 1,2 (high and mid priority) intially block on semaphore
	* thread 0 (low priority) acquires mutex
	* thread 0 signals semaphore to unblock thread 2 (high priority)
	* thread 2 blocks on mutex owned by thread 0
	* thread 0 inherits thread 2's priority and runs repeatedly, signals mutex to unblock thread 1
	* thread 0 releases mutex on button press 
	* thread 2 is unblocked and runs, thread 0 is demoted to low priority 
	* thread 2 blocks on semaphore again 
	* thread 1 runs repeatedly (highest priority unblocked task)
	*/
	
//	sem_init(&s2,0);
//	mutex_init(&m2);
//	
//	uint8_t status_1 = 0;
//	
//	status_1 = osThreadCreate((rtosFxn_t)low_priority_thread,osPriorityCleanliness,NULL);
//	status_1 = osThreadCreate((rtosFxn_t)mid_priority_thread,osPriorityGPA,NULL);
//	status_1 = osThreadCreate((rtosFxn_t)high_priority_thread,osPriorityGains,NULL);
//	
//	

	// ----- Kernel Start Test Code -----
	
	osKernelStart();
	
	
	
	while(1);
		
}

